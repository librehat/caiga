#include "workerthread.h"
#include <QtConcurrent/QtConcurrent>

WorkerThread::WorkerThread(QObject *parent) :
    QObject(parent)
{
}

void WorkerThread::getToWork(void *functionPointer())
{
    watcher = QtConcurrent::run(*functionPointer());
}
