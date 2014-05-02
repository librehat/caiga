#include <QtConcurrent>
#include "workspace.h"
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
        qWarning() << "Abort. Insufficient items to undo.";
        return;
    }
    undoneList.append(workList.takeLast());
}

void WorkSpace::redo()
{
    if (undoneList.isEmpty()) {
        qWarning() << "Abort. undoneList is empty.";
        return;
    }
    workList.append(undoneList.takeFirst());
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

void WorkSpace::newHistogramEqualiseWork()
{
    //use latest dst as new work's src
    WorkBase *w = new WorkHistEqualise(workList.last()->dst);
    workList.append(w);
    future = QtConcurrent::run(w, &WorkBase::Func);
    watcher.setFuture(future);
}

void WorkSpace::newAdaptiveBilateralFilterWork(int size, double space, double colour)
{
    WorkBase *w = new WorkAptBilateralFilter(workList.last()->dst, size, space, colour);
    workList.append(w);
    future = QtConcurrent::run(w, &WorkBase::Func);
    watcher.setFuture(future);
}

void WorkSpace::newMedianBlurWork(int kSize)
{
    WorkBase *w = new WorkMedianBlur(workList.last()->dst, kSize);
    workList.append(w);
    future = QtConcurrent::run(w, &WorkBase::Func);
    watcher.setFuture(future);
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
