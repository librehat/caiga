#include <QtConcurrent>
#include "workspace.h"
#include "workhistequalise.h"
#include "workmedianblur.h"
#include "workaptbilateralfilter.h"
#include "workbinaryzation.h"
#include "workcanny.h"
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
}

void WorkSpace::redo()
{
    if (undoneList.isEmpty()) {
        emit workStatusStringUpdated("Abort. Nothing available to be redone.");
        return;
    }
    workList.append(undoneList.takeLast());
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
    //clear undoneList to avoid corrupted redo action.
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

void WorkSpace::newContoursWork()
{
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
