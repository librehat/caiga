#include <QtConcurrent>
#include "workspace.h"
#include "workhistequalise.h"
#include "workmedianblur.h"
#include "workaptbilateralfilter.h"
#include "workbinaryzation.h"
#include "workcanny.h"
#include "workfloodfill.h"
#include "workcontours.h"
using namespace CAIGA;

WorkSpace::WorkSpace(QObject *parent) :
    QObject(parent)
{
    connect(&watcher, &QFutureWatcher<void>::started, this, &WorkSpace::onLowLevelWorkStarted);
    connect(&watcher, &QFutureWatcher<void>::finished, this, &WorkSpace::onLowLevelWorkFinished);
    workList.append(new WorkBase);
}

WorkSpace::~WorkSpace()
{
    this->clear();
}

void WorkSpace::undo()
{
    if (workList.count() <= 1) {
        emit workStatusStringUpdated("Abort. Nothing available to be undone.");
        return;
    }
    undoneList.append(workList.takeLast());
    emit workFinished();
}

void WorkSpace::redo()
{
    if (undoneList.isEmpty()) {
        emit workStatusStringUpdated("Abort. Nothing available to be redone.");
        return;
    }
    workList.append(undoneList.takeLast());
    emit workFinished();
}

void WorkSpace::append(WorkBase *w)
{
    workList.append(w);
    this->clearUndoneList();
    emit workFinished();
}

void WorkSpace::append(QList<WorkBase *> l)
{
    //do not take in duplicates
    for (int i = 0; i < l.size(); ++i) {
        if (workList.contains(l.at(i))) {
            delete l.takeAt(i);
        }
        else {
            workList.append(l.takeAt(i));
        }
    }
    this->clearUndoneList();
    emit workFinished();
}

void WorkSpace::appendAndClone(WorkBase *w)
{
    WorkBase *nw = new WorkBase(*w);
    workList.append(nw);
    this->clearUndoneList();
}

void WorkSpace::pop()
{
    delete workList.takeLast();
}

WorkBase *WorkSpace::last()
{
    return workList.last();
}

WorkBase *WorkSpace::takeLast()
{
    return workList.takeLast();
}

QList<WorkBase *> WorkSpace::takeAll()
{
    QList<WorkBase *> ta;
    while (!workList.isEmpty()) {
        ta.append(workList.takeFirst());
    }
    return ta;
}

void WorkSpace::simplified()
{
    //when user click "save" button.
    //use this function to remove WorkBase(s) from workList except for current one
}

void WorkSpace::clear()
{
    if (!workList.isEmpty()) {
        for (QList<WorkBase *>::iterator it = workList.begin(); it != workList.end(); ++it) {
            delete (*it);
        }
        workList.clear();
    }
    this->clearUndoneList();
}

void WorkSpace::clearUndoneList()
{
    if (!undoneList.isEmpty()) {
        for (QList<WorkBase *>::iterator it = undoneList.begin(); it != undoneList.end(); ++it) {
            delete (*it);
        }
        undoneList.clear();
    }
}

void WorkSpace::reset(Image &cgimg)
{
    this->clear();
    WorkBase *w = new WorkBase(&cgimg.croppedImage);
    workList.append(w);
}

void WorkSpace::reset(Mat *s)
{
    this->clear();
    WorkBase *w = new WorkBase(s);
    workList.append(w);
}

int WorkSpace::count()
{
    return workList.size();
}

Mat *WorkSpace::getLatestMat()
{
    return workList.last()->dst;
}

QImage WorkSpace::getLatestQImg()
{
    return Image::convertMat2QImage(*workList.last()->dst);
}

void WorkSpace::newGenericWork(WorkBase *work)
{
    workList.append(work);
    this->clearUndoneList();
    future = QtConcurrent::run(work, &WorkBase::Func);
    watcher.setFuture(future);
}

void WorkSpace::newHistogramEqualiseWork()
{
    WorkBase *w = new WorkHistEqualise(workList.last()->dst);
    this->newGenericWork(w);
}

void WorkSpace::newAdaptiveBilateralFilterWork(int size, double space, double colour)
{
    WorkBase *w = new WorkAptBilateralFilter(workList.last()->dst, size, space, colour);
    this->newGenericWork(w);
}

void WorkSpace::newMedianBlurWork(int kSize)
{
    WorkBase *w = new WorkMedianBlur(workList.last()->dst, kSize);
    this->newGenericWork(w);
}

void WorkSpace::newBinaryzationWork(int method, int type, int size, double constant)
{
    WorkBase *w = new WorkBinaryzation(workList.last()->dst, method, type, size, constant);
    this->newGenericWork(w);
}

void WorkSpace::newCannyWork(int aSize, double high, double low, bool l2)
{
    WorkBase *w = new WorkCanny(workList.last()->dst, aSize, high, low, l2);
    this->newGenericWork(w);
}

void WorkSpace::setFloodFillWorkParameters(double high, double low, bool con8)
{
    m_d1 = high;
    m_d2 = low;
    m_bool = con8;
}

void WorkSpace::newFloodFillWork(int x, int y)
{
    WorkBase *w = new WorkFloodFill(workList.last()->dst, m_d1, m_d2, m_bool, x, y);
    this->newGenericWork(w);
}

void WorkSpace::newContoursWork()
{
    if (workList.last()->dst->type() != CV_8UC1) {
        emit workStatusStringUpdated("Abort. 8-bit single channel image needed.");
        return;
    }
    WorkBase *w = new WorkContours(workList.last()->dst);
    this->newGenericWork(w);
}

void WorkSpace::onLowLevelWorkStarted()
{
    emit workStarted();
    emit workStatusStringUpdated("Work has been started. Please wait......");
}

void WorkSpace::onLowLevelWorkFinished()
{
    emit workFinished();
    emit workStatusStringUpdated("Work Finished.");
}
