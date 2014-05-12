#include "workaptbilateralfilter.h"
using namespace CAIGA;

WorkAptBilateralFilter::WorkAptBilateralFilter(cv::Mat *s, int k, double space, double colour) : WorkBase(s)
{
    workType = AptBilateralFilter;
    cvSize = cv::Size(k, k);
    sigmaX = space;
    sigmaY = colour;
}

void WorkAptBilateralFilter::Func()
{
    cv::adaptiveBilateralFilter(*src, *dst, cvSize, sigmaX, sigmaY);
}
