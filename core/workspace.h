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
    explicit WorkSpace(cv::Mat *src = NULL, int steps = 10, bool restrictList = true, bool keepFirst = false, QObject *parent = 0);
    ~WorkSpace();
    void append(WorkBase *);//this will emit workFinished
    void append(QList<WorkBase *>);//this will also emit workFinished. WARN: this function will manipulate the QList directly, use it with caution!
    void appendAndClone(WorkBase *);//this won't emit workFinished
    void pop() { delete workList.takeLast(); }//pop out the last from worklist
    inline WorkBase *first() { return workList.first(); }
    inline WorkBase *last() { return workList.last(); }
    inline WorkBase *takeLast() { return workList.takeLast(); }
    QList<WorkBase *> takeAll();
    void clear();//erase everything including the first
    void reset(WorkBase *base);//workSpace won't use the pointer directly. instead, it will clone a new WorkBase.
    void reset(cv::Mat *s);
    void reset();//erase everything except for workList.first()
    inline int count() { return workList.size(); }
    inline int countUndone() { return undoneList.size(); }
    void newInvertGrayscaleWork();
    void newHistogramEqualiseWork();
    /*
     * pass cont = true if you want to use workList.last()->dst as the src image as usual.
     * it uses the workList.first()->dst as the src image by default
     */
    void newBoxFilterWork(int size, bool cont = false);
    void newAdaptiveBilateralFilterWork(int size, double space, double colour, bool cont = false);
    void newMedianBlurWork(int kSize, bool cont = false);
    void newBinaryzationWork(int method, int type, int size, double constant, bool cont = false);
    void newCannyWork(int aSize, double high, double low, bool l2, bool cont = false);
    inline void setFloodFillWorkParameters(double high, double low, bool con8)
    {
        m_d1 = high;
        m_d2 = low;
        m_bool = con8;
    } //this is not a generic work. it'll be finished by member function lastFloodFillWorkClicked.
    void newFloodFillWork(QVector<QPointF> pts, bool cont = false);
    void newContoursWork();
    void newPencilWork(const QVector<QPointF> &pts, bool white);
    void newPencilWork(const QVector<QPointF> &pts, QColor colour);
    void newEraserWork(const QVector<QPointF> &pts, bool white);
    void newWatershedWork(const Mat *input, bool cont = false);
    void newGradientWork(int k, bool cont = false);
    inline Mat *getLastMatrix() { return workList.last()->dst; }
    inline QImage getLastDstImage() { return Image::convertMat2QImage(*workList.last()->dst); }
    QImage getLastDisplayImage();
    std::vector<std::vector<Point> > getContours();
    cv::Mat *getMarkerMatrix();

public slots:
    bool undo();//move the last of workList to undoneList. return true if successful
    bool redo();//move the last of undoneList to workList. return true if successful
    void setUndoSteps(int s) { m_steps = s; }

private:
    QList<WorkBase *> workList;
    QList<WorkBase *> undoneList;
    QFuture<void> future;
    QFutureWatcher<void> watcher;
    bool m_isRestrict;
    bool m_restrictKeepFirst;//whether to keep the first WorkBase* in list when restrict the workList
    void clearUndoneList();
    void restrictWorkList();
    inline void newGenericWork(WorkBase *work)
    {
        workList.append(work);
        this->clearUndoneList();
        future = QtConcurrent::run(work, &WorkBase::Func);
        watcher.setFuture(future);
        if (workList.count() > m_steps) {
            restrictWorkList();
        }
    }
    cv::Mat displayMat;//used to display on screen

private slots:
    void onLowLevelWorkStarted();
    void onLowLevelWorkFinished();

protected:
    double m_d1;
    double m_d2;
    bool m_bool;
    int m_steps;
};

}

#endif // WORKSPACE_H
