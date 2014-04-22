#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <QObject>
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "core_lib.h"
#include "datastructure.h"
#include "image.h"
namespace CAIGA {
class CORE_LIB WorkerThread : public QObject
{
    Q_OBJECT
public:
    explicit WorkerThread(QObject *parent = 0);
    void histogramEqualiseWork(CAIGA::Image &cimg);
    void adaptiveBilateralFilterWork(CAIGA::Image &cimg, int k, double space, double colour);
    void gaussianBlurWork(CAIGA::Image &cimg, int k, double sx, double sy);
    void medianBlurWork(CAIGA::Image &cimg, int k);
    void binaryzationWork(CAIGA::Image &cimg, int method, int type, int blockSize, double C);
    void cannyEdgesWork(CAIGA::Image &cimg, double ht, double lt, int aSize, bool l2);
    void floodfillSegmentWork(CAIGA::Image &cimg, QPoint p, int hd, int ld, bool c4);
    void contoursFindDrawWork(CAIGA::Image &cimg);

signals:
    void workFinished();
    void workStatusUpdated(const QString &);

private slots:
    void handleWorkFinished();
    void handleWorkStarted();

private:
    QFutureWatcher<void> watcher;
    static void doHistogramEqualiseWork(cv::Mat *src, cv::Mat *out);
    static void doAdaptiveBilateralFilterWork(cv::Mat *src, cv::Mat *out, cv::Size kSize, double space, double colour);
    static void doGaussianBlurWork(cv::Mat *src, cv::Mat *out, cv::Size kSize, double sx, double sy);
    static void doMedianBlurWork(cv::Mat *src, cv::Mat *out, int kSize);
    static void doBinaryzationWork(cv::Mat *src, cv::Mat *out, parameterS p);
    static void doCannyEdgesWork(cv::Mat *src, cv::Mat *out, parameterS p);
    static void doFloodFillSegmentWork(cv::Mat *dst, cv::Point seed, parameterS p);
    static void doContoursFindDrawWork(cv::Mat &src, cv::Mat *dst);
};
}

#endif // WORKERTHREAD_H
