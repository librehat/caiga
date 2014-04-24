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
    QFuture<void> future = QtConcurrent::run(&WorkerThread::doBinaryzationWork, &cimg.preprocessedImage, &cimg.edgesImage, para);
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
    QFuture<void> future = QtConcurrent::run(&WorkerThread::doFloodFillSegmentWork, &cimg.edgesImage, seed, para);
    watcher.setFuture(future);
}

void WorkerThread::contoursFindDrawWork(Image &cimg)
{
    QFuture<void> future = QtConcurrent::run(&WorkerThread::doContoursFindDrawWork, cimg.edgesImage, &cimg.processedImage);
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
    int flags = (p.b ? 4 : 8) + (255 << 8) + (p.i1 == 0 ? 0 : CV_FLOODFILL_FIXED_RANGE);
    int b = (unsigned)cv::theRNG() & 255;
    int g = (unsigned)cv::theRNG() & 255;
    int r = (unsigned)cv::theRNG() & 255;
    Scalar colour(b, g, r);
    //flood fill
    cv::Rect ccomp;
    cv::floodFill(*dst, seed, colour, &ccomp, cv::Scalar(p.i2, p.i2, p.i2), cv::Scalar(p.i1, p.i1, p.i1), flags);
}

void WorkerThread::doContoursFindDrawWork(Mat &src, Mat *dst)
{
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(src, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_L1);//use CV_RETR_EXTERNAL mode to avoid holes caused by preProcessing. anyway, we don't have grains inside grains.

    *dst = cv::Mat::zeros(src.rows, src.cols, CV_8UC3);
    //draw each contour in its own random colour
    int idx = 0;
    for (; idx >= 0 ; idx = hierarchy[idx][0]) {
        Scalar colour(rand() & 255, rand() & 255, rand() & 255);
        cv::drawContours(*dst, contours, idx, colour, CV_FILLED, CV_AA, hierarchy);
    }
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
