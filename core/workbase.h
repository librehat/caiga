/*
 * This is an abstract base class for varies of works.
 */

#ifndef WORKBASE_H
#define WORKBASE_H
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "core_lib.h"
#include <QDebug>

namespace CAIGA {
class WorkBase
{
public:
    enum WorkTypes {Raw, AptBilateralFilter, Binaryzation, BoxFilter, Canny, Contours, Eraser, FloodFill, HistEqualise, InvertGrayscale, MedianBlur, Pencil, Watershed, AutoWatershed, Gradient, Scharr};

    WorkBase() : src(NULL) {
        workType = Raw;
        dst = new cv::Mat();
        display = dst;
        markerMatrix = NULL;
        inputMarker = NULL;
    }

    WorkBase(const cv::Mat *s) : src(s) {
        workType = Raw;
        dst = new cv::Mat(s->clone());
        display = dst;
        markerMatrix = NULL;
        inputMarker = NULL;
    }

    WorkBase(const cv::Mat *s, cv::Mat *d) : src(s) {
        workType = Raw;
        dst = new cv::Mat(d->clone());
        display = dst;
        markerMatrix = NULL;
        inputMarker = NULL;
    }

    WorkBase(WorkBase *base) : src(base->src) {
        workType = base->workType;
        dst = new cv::Mat(base->dst->clone());
        display = dst;
        inputMarker = base->inputMarker;
        oddSize = base->oddSize;
        size = base->size;
        cvSize = base->cvSize;
        method = base->method;
        type = base->type;
        constant = base->constant;
        sigmaX = base->sigmaX;
        sigmaY = base->sigmaY;
        b = base->b;
        pointVec = base->pointVec;
        markers = base->markers;
        contours = base->contours;
        markerMatrix = base->markerMatrix;
    }

    virtual ~WorkBase()
    {
        if (display != dst) {//don't delete the same pointer twice
            delete display;
        }
        if (markerMatrix != NULL) {
            delete markerMatrix;
        }
        delete dst;
    }

    virtual void Func() {}

    inline bool operator == (const WorkBase &w) const {
        if (this->src != w.src || this->dst != w.dst || this->display != w.display || this->workType != w.workType) {
            return false;
        }
        else
            return true;
    }

    WorkTypes workType;
    const cv::Mat *src;//share memory with other WorkBase(s)
    cv::Mat *dst;//to be next work's source Mat
    const cv::Mat *inputMarker;//inputMarker is drawed manually and (or) automatically. it's used to watershed and you should not delete it externally during the process.
    /*
     * sometimes *display may be the same Mat as *dst
     * if it's not, commonly seen where there is a mask, handle it carefully.
     */
    cv::Mat *display;//used to display on screen
    cv::Mat *markerMatrix;//keep watershed output

    int oddSize;//should always be an odd number //red
    int size;//may be odd or even //green
    cv::Size cvSize;
    int method;//point.x in ffill //blue
    int type;//point.y in ffill
    double constant;
    double sigmaX;//sigmaSpace in adaptiveBilateralFilter; high_diff in ffill
    double sigmaY;//sigmaColor in adaptiveBilateralFilter; low_diff in ffill
    bool b;
    std::vector<cv::Point> pointVec;
    std::vector<std::vector<cv::Point> > markers;

    //analysis is based on contours
    std::vector<std::vector<cv::Point> > contours;
};

}
#endif // WORKBASE_H
