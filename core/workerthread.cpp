#include "workerthread.h"
#include <QDebug>
using namespace CAIGA;

WorkerThread::WorkerThread(QObject *parent) :
    QObject(parent)
{
    connect(&watcher, &QFutureWatcher<void>::finished, this, &WorkerThread::handleWorkFinished);
    connect(&watcher, &QFutureWatcher<void>::started, this, &WorkerThread::handleWorkStarted);
}

//functions used to "get hired" by main thread.

void WorkerThread::histogramEqualiseWork(Image &cimg)
{
    QFuture<void> future = QtConcurrent::run(&WorkerThread::doHistogramEqualiseWork, &cimg.croppedImage, &cimg.preprocessedImage);
    watcher.setFuture(future);
}

void WorkerThread::adaptiveBilateralFilterWork(Image &cimg, int k, double space, double colour)
{
    cv::Size kSize(k, k);
    QFuture<void> future = QtConcurrent::run(&WorkerThread::doAdaptiveBilateralFilterWork, &cimg.croppedImage, &cimg.preprocessedImage, kSize, space, colour);
    watcher.setFuture(future);
}

void WorkerThread::gaussianBlurWork(Image &cimg, int k, double sx, double sy)
{
    cv::Size kSize(k, k);
    QFuture<void> future = QtConcurrent::run(&WorkerThread::doGaussianBlurWork, &cimg.croppedImage, &cimg.preprocessedImage, kSize, sx, sy);
    watcher.setFuture(future);
}

void WorkerThread::medianBlurWork(Image &cimg, int k)
{
    QFuture<void> future = QtConcurrent::run(&WorkerThread::doMedianBlurWork, &cimg.croppedImage, &cimg.preprocessedImage, k);
    watcher.setFuture(future);
}

void WorkerThread::binaryzationWork(Image &cimg, int method, int type, int blockSize, double C)
{
    parameterS para;
    para.i1 = method;
    para.i2 = type;
    para.i3 = blockSize;
    para.d1 = C;
    QFuture<void> future = QtConcurrent::run(&WorkerThread::doBinaryzationWork, &cimg.croppedImage, &cimg.preprocessedImage, para);
    watcher.setFuture(future);
}

void WorkerThread::cannyEdgesWork(Image &cimg, double ht, double lt, int aSize, bool l2)
{
    parameterS para;
    para.d1 = ht;
    para.d2 = lt;
    para.i1 = aSize;
    para.b = l2;
    QFuture<void> future = QtConcurrent::run(&WorkerThread::doCannyEdgesWork, &cimg.preprocessedImage, &cimg.edgesImage, para);
    watcher.setFuture(future);
}

void WorkerThread::floodfillSegmentWork(Image &cimg, QPoint p, int hd, int ld, bool c4)
{
    cv::Point seed(p.x(), p.y());
    parameterS para;
    para.i1 = hd;
    para.i2 = ld;
    para.b = c4;
    QFuture<void> future = QtConcurrent::run(&WorkerThread::doFloodFillSegmentWork, &cimg.processedImage, seed, para);
    watcher.setFuture(future);
}

//actual work be done by functions below

void WorkerThread::doHistogramEqualiseWork(cv::Mat *src, cv::Mat *out)
{
    cv::equalizeHist(*src, *out);
}

void WorkerThread::doAdaptiveBilateralFilterWork(cv::Mat *src, cv::Mat *out, cv::Size kSize, double space, double colour)
{
    cv::adaptiveBilateralFilter(*src, *out, kSize, space, colour);
}

void WorkerThread::doGaussianBlurWork(cv::Mat *src, cv::Mat *out, cv::Size kSize, double sx, double sy)
{
    cv::GaussianBlur(*src, *out, kSize, sx, sy);
}

void WorkerThread::doMedianBlurWork(cv::Mat *src, cv::Mat *out, int kSize)
{
    cv::medianBlur(*src, *out, kSize);
}

void WorkerThread::doBinaryzationWork(cv::Mat *src, cv::Mat *out, parameterS p)
{
    cv::adaptiveThreshold(*src, *out, 255, p.i1, p.i2, p.i3, p.d1);
}

void WorkerThread::doCannyEdgesWork(cv::Mat *src, cv::Mat *out, parameterS p)
{
    cv::Canny(*src, *out, p.d1, p.d2, p.i1, p.b);
}

void WorkerThread::doFloodFillSegmentWork(cv::Mat *dst, cv::Point seed, parameterS p)
{
    //FIXME
    int flags = (p.b ? 4 : 8) + (255 << 8) + (p.i1 == 0 ? 0 : CV_FLOODFILL_FIXED_RANGE);
    int b = (unsigned)cv::theRNG() & 255;
    int g = (unsigned)cv::theRNG() & 255;
    int r = (unsigned)cv::theRNG() & 255;
    Scalar colour(b, g, r);
    //flood fill
    cv::Rect ccomp;
    cv::floodFill(*dst, seed, colour, &ccomp, cv::Scalar(p.i2, p.i2, p.i2), cv::Scalar(p.i1, p.i1, p.i1), flags);
}

//some slots

void WorkerThread::handleWorkStarted()
{
    emit workStatusUpdated(QString("Worker thread has been started. Please wait."));
}

void WorkerThread::handleWorkFinished()
{
    emit workFinished();
    emit workStatusUpdated(QString("Worker thread finished."));
}
