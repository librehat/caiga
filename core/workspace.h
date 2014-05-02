#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QObject>
#include <QList>
#include <QFuture>
#include <QFutureWatcher>
#include "core_lib.h"
#include "workbase.h"
#include "workhistequalise.h"
#include "workmedianblur.h"
#include "workaptbilateralfilter.h"
#include "image.h"

namespace CAIGA
{
class CORE_LIB WorkSpace : public QObject
{
    Q_OBJECT
public:
    explicit WorkSpace(QObject *parent = 0);
    ~WorkSpace();
    void undo();
    void redo();
    void simplified();
    void clear();
    void reset(CAIGA::Image &cgimg);//reset and use the cropped image from cgimg
    int count();
    void newHistogramEqualiseWork();
    void newAdaptiveBilateralFilterWork(int size, double space, double colour);
    void newMedianBlurWork(int kSize);
    Mat *getLatestMat();
    QImage getLatestQImg();

signals:
    void workStarted();
    void workFinished();
    void workStatusStringUpdated(const QString &);

private slots:
    void onLowLevelWorkStarted();
    void onLowLevelWorkFinished();

private:
    QList<WorkBase *> workList;
    QList<WorkBase *> undoneList;
    QFuture<void> future;
    QFutureWatcher<void> watcher;
};

}

#endif // WORKSPACE_H
