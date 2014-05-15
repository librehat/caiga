#include "workspace.h"
#include "workhistequalise.h"
#include "workmedianblur.h"
#include "workboxfilter.h"
#include "workaptbilateralfilter.h"
#include "workbinaryzation.h"
#include "workcanny.h"
#include "workfloodfill.h"
#include "workcontours.h"
#include "workpencil.h"
#include "workeraser.h"
#include "workinvertgrayscale.h"
#include "workwatershed.h"
#include "workgradient.h"
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
    emit workFinished();
}

void WorkSpace::redo()
{
    if (undoneList.isEmpty()) {
        emit workStatusStringUpdated("Abort. Nothing available to be redone.");
        return;
    }
    workList.append(undoneList.takeLast());
    emit workFinished();
}

void WorkSpace::append(WorkBase *w)
{
    workList.append(w);
    this->clearUndoneList();
    emit workFinished();
}

void WorkSpace::append(QList<WorkBase *> l)
{
    //do not take in duplicates
    for (int i = 0; i < l.size(); ++i) {
        if (workList.contains(l.at(i))) {
            delete l.takeAt(i);
        }
        else {
            workList.append(l.takeAt(i));
        }
    }
    this->clearUndoneList();
    emit workFinished();
}

void WorkSpace::appendAndClone(WorkBase *w)
{
    WorkBase *nw = new WorkBase(w);
    workList.append(nw);
    this->clearUndoneList();
}

QList<WorkBase *> WorkSpace::takeAll()
{
    QList<WorkBase *> ta;
    while (!workList.isEmpty()) {
        ta.append(workList.takeFirst());
    }
    return ta;
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
    emit workFinished();
}

void WorkSpace::reset(Mat *s)
{
    this->clear();
    WorkBase *w = new WorkBase(s);
    workList.append(w);
    emit workFinished();
}

void WorkSpace::reset()
{
    if (workList.size() > 1) {
        for (QList<WorkBase *>::iterator it = workList.begin() + 1; it != workList.end(); it = workList.erase(it)) {
            delete (*it);
        }
    }
    this->clearUndoneList();
    emit workFinished();
}

void WorkSpace::newInvertGrayscaleWork()
{
    WorkBase *w = new WorkInvertGrayscale(workList.last()->dst);
    this->newGenericWork(w);
}

void WorkSpace::newHistogramEqualiseWork()
{
    if (workList.last()->dst->type() != CV_8UC1) {
        emit workStatusStringUpdated("Abort. 8-bit single channel image needed.");
        return;
    }
    WorkBase *w = new WorkHistEqualise(workList.last()->dst);
    this->newGenericWork(w);
}

void WorkSpace::newBoxFilterWork(int size, bool cont)
{
    cv::Mat *s = cont ? workList.last()->dst : workList.first()->dst;
    WorkBase *w = new WorkBoxFilter(s, size);
    this->newGenericWork(w);
}

void WorkSpace::newAdaptiveBilateralFilterWork(int size, double space, double colour, bool cont)
{
    cv::Mat *s = cont ? workList.last()->dst : workList.first()->dst;
    if (s->type() != CV_8UC1 && s->type() != CV_8UC3) {
        emit workStatusStringUpdated("Abort. 8-bit single or 3-channel image needed.");
        return;
    }
    WorkBase *w = new WorkAptBilateralFilter(s, size, space, colour);
    this->newGenericWork(w);
}

void WorkSpace::newMedianBlurWork(int kSize)
{
    WorkBase *w = new WorkMedianBlur(workList.last()->dst, kSize);
    this->newGenericWork(w);
}

void WorkSpace::newBinaryzationWork(int method, int type, int size, double constant, bool cont)
{
    cv::Mat *s = cont ? workList.last()->dst : workList.first()->dst;
    WorkBase *w = new WorkBinaryzation(s, method, type, size, constant);
    this->newGenericWork(w);
}

void WorkSpace::newCannyWork(int aSize, double high, double low, bool l2, bool cont)
{
    cv::Mat *s = cont ? workList.last()->dst : workList.first()->dst;
    WorkBase *w = new WorkCanny(s, aSize, high, low, l2);
    this->newGenericWork(w);
}

void WorkSpace::newFloodFillWork(int x, int y, bool cont)
{
    cv::Mat *s = cont ? workList.last()->dst : workList.first()->dst;
    WorkBase *w = new WorkFloodFill(s, m_d1, m_d2, m_bool, x, y);
    this->newGenericWork(w);
}

void WorkSpace::newContoursWork()
{
    if (workList.last()->dst->type() != CV_8UC1) {
        emit workStatusStringUpdated("Abort. 8-bit single channel image needed.");
        return;
    }
    WorkBase *w = new WorkContours(workList.last()->dst);
    this->newGenericWork(w);
}

void WorkSpace::newPencilWork(const QVector<QPoint> &pts, bool white)
{
    if (pts.size() < 2) {
        emit workStatusStringUpdated("Abort. Insufficient points.");
        return;
    }

    std::vector<cv::Point> cvPts;
    for (QVector<QPoint>::const_iterator it = pts.begin(); it != pts.end(); ++it) {
        cv::Point p(it->x(), it->y());
        cvPts.push_back(p);
    }
    int rgb = white ? 255 : 0;
    WorkBase *w = new WorkPencil(workList.last()->dst, cvPts, rgb, rgb, rgb);
    this->newGenericWork(w);
}

void WorkSpace::newPencilWork(const QVector<QPoint> &pts, QColor colour)
{
    if (pts.size() < 2) {
        emit workStatusStringUpdated("Abort. Insufficient points.");
        return;
    }

    std::vector<cv::Point> cvPts;
    for (QVector<QPoint>::const_iterator it = pts.begin(); it != pts.end(); ++it) {
        cv::Point p(it->x(), it->y());
        cvPts.push_back(p);
    }

    int r = colour.blue();//swap red and blue for QImage to cv::Mat
    int b = colour.red();
    int g = colour.green();
    WorkBase *w = new WorkPencil(workList.last()->dst, cvPts, r, g, b);
    this->newGenericWork(w);
}

void WorkSpace::newEraserWork(const QVector<QPoint> &pts, bool white)
{
    std::vector<cv::Point> cvPts;
    for (QVector<QPoint>::const_iterator it = pts.begin(); it != pts.end(); ++it) {
        cv::Point p(it->x(), it->y());
        cvPts.push_back(p);
    }
    WorkBase *w = new WorkEraser(workList.last()->dst, cvPts, white);
    this->newGenericWork(w);
}

void WorkSpace::newWatershedWork(const QVector<QVector<QPoint> > &markerPts, bool cont)
{
    if (markerPts.isEmpty()) {
        emit workStatusStringUpdated("Abort. Empty markers.");
        return;
    }
    cv::Mat *s = cont ? workList.last()->dst : workList.first()->dst;
    if (s->type() != CV_8UC1) {
        emit workStatusStringUpdated("Abort. 8-bit single channel image needed.");
        return;
    }
    std::vector<std::vector<cv::Point> > cvpvv;
    for (QVector<QVector<QPoint> >::const_iterator it = markerPts.begin(); it != markerPts.end(); ++it) {
        std::vector<cv::Point> tempV;
        for (QVector<QPoint>::const_iterator pit = it->begin(); pit != it->end(); ++pit) {
            tempV.push_back(cv::Point(pit->x(), pit->y()));
        }
        cvpvv.push_back(tempV);
    }

    WorkBase *w = new WorkWatershed(s, cvpvv);
    this->newGenericWork(w);
}

void WorkSpace::newGradientWork(int k, bool cont)
{
    cv::Mat *s = cont ? workList.last()->dst : workList.first()->dst;
    WorkBase *w = new WorkGradient(s, k);
    this->newGenericWork(w);
}

QImage WorkSpace::getLastDisplayImage()
{
    return Image::convertMat2QImage(*workList.last()->display);
}

std::vector<std::vector<cv::Point> > WorkSpace::getContours()
{
    return workList.last()->contours;
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
