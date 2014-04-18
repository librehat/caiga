#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <QObject>
#include <QFuture>
#include <QFutureWatcher>
#include "core_lib.h"

class CORE_LIB WorkerThread : public QObject
{
    Q_OBJECT
public:
    explicit WorkerThread(QObject *parent = 0);
    void getToWork(void *functionPointer());

signals:
    void workFinished(bool);
    void workStarted(bool);

public slots:

private:
    QFutureWatcher<void> watcher;
};

#endif // WORKERTHREAD_H
