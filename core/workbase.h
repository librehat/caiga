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
    //enum type {Raw, HistEqualise, AdaBilateralFilter, MedianBlur};//seems useless
    WorkBase() : src(NULL) {
        dst = new cv::Mat();
    }

    WorkBase(const cv::Mat *const s) : src(s) {
        dst = new cv::Mat(s->clone());
    }

    WorkBase(const cv::Mat *const s, cv::Mat *d) : src(s) {
        dst = new cv::Mat(d->clone());
    }

    WorkBase(WorkBase *base) : src(base->src) {
        dst = new cv::Mat(base->dst->clone());
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
    }

    virtual ~WorkBase() { delete dst; }
    virtual void Func() {}

    inline bool operator == (const WorkBase &w) const {
        if (this->src != w.src || this->dst != w.dst) {
            return false;
        }
        else
            return true;
    }

    const cv::Mat *const src;//share memory with other WorkBase(s)
    cv::Mat *dst;

    int oddSize;//should always be an odd number
    int size;//may be odd or even
    cv::Size cvSize;
    int method;//point.x in ffill
    int type;//point.y in ffill
    double constant;
    double sigmaX;//sigmaSpace in adaptiveBilateralFilter; high_diff in ffill
    double sigmaY;//sigmaColor in adaptiveBilateralFilter; low_diff in ffill
    bool b;
    std::vector<cv::Point> pointVec;
};

}
#endif // WORKBASE_H
