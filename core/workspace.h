#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QObject>
#include <QList>
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include "core_lib.h"
#include "workbase.h"
#include "image.h"

namespace CAIGA
{
class CORE_LIB WorkSpace : public QObject
{
    Q_OBJECT

signals:
    void workStarted();
    void workFinished();
    void workStatusStringUpdated(const QString &);

public:
    explicit WorkSpace(QObject *parent = 0);
    ~WorkSpace();
    void append(WorkBase *);//this will emit workFinished
    void append(QList<WorkBase *>);//this will also emit workFinished. WARN: this function will manipulate the QList directly, use it with caution!
    void appendAndClone(WorkBase *);//this won't emit workFinished
    void pop();//pop out the last from worklist
    WorkBase *last();
    WorkBase *takeLast();
    QList<WorkBase *> takeAll();
    void simplified();
    void clear();
    void reset(CAIGA::Image &cgimg);//reset and use the cropped image from cgimg
    void reset(cv::Mat *s);
    void reset();//erase everything except for workList.first()
    int count();
    void newInvertGrayscaleWork();
    void newHistogramEqualiseWork();
    void newBoxFilterWork(int size);
    void newAdaptiveBilateralFilterWork(int size, double space, double colour);
    void newMedianBlurWork(int kSize);
    void newBinaryzationWork(int method, int type, int size, double constant);
    void newCannyWork(int aSize, double high, double low, bool l2);
    void setFloodFillWorkParameters(double high, double low, bool con8);//this is not a generic work. it'll be finished by member function lastFloodFillWorkClicked.
    void newFloodFillWork(int x, int y);
    void newContoursWork();
    void newPencilWork(const QVector<QPoint> &pts, bool white);
    void newPencilWork(const QVector<QPoint> &pts, QColor colour);
    void newEraserWork(const QVector<QPoint> &pts, bool white);
    /*
     * pass cont = true if you want to use workList.last()->dst as the src image as usual.
     * it uses the workList.first()->dst as the src image by default
     */
    void newWatershedWork(const QVector<QVector<QPoint> > &markerPts, bool cont = false);
    Mat *getLatestMat();
    QImage getLatestQImg();

public slots:
    void undo();
    void redo();

private:
    QList<WorkBase *> workList;
    QList<WorkBase *> undoneList;
    QFuture<void> future;
    QFutureWatcher<void> watcher;
    void clearUndoneList();
    inline void newGenericWork(WorkBase *work) {
        workList.append(work);
        this->clearUndoneList();
        future = QtConcurrent::run(work, &WorkBase::Func);
        watcher.setFuture(future);
    }

private slots:
    void onLowLevelWorkStarted();
    void onLowLevelWorkFinished();

protected:
    double m_d1;
    double m_d2;
    bool m_bool;
};

}

#endif // WORKSPACE_H
