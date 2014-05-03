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
class CORE_LIB  WorkBase
{
public:
    //enum type {Raw, HistEqualise, AdaBilateralFilter, MedianBlur};//seems useless
    WorkBase() {
        src = new cv::Mat();
        dst = new cv::Mat();
    }
    WorkBase(cv::Mat *s) {
        src = new cv::Mat(s->clone());
        dst = new cv::Mat(s->clone());
    }
    virtual ~WorkBase() { delete src; delete dst; }
    virtual void Func() {}
    cv::Mat *src;
    cv::Mat *dst;

    int oddSize;//should always be an odd number
    int size;//may be odd or even
    cv::Size cvSize;
    int method;
    int type;
    double constant;
    double sigmaX;//sigmaSpace in adaptiveBilateralFilter
    double sigmaY;//sigmaColor in adaptiveBilateralFilter
    bool b;
};

}
#endif // WORKBASE_H
