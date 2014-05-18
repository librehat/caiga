#include "workcanny.h"
using namespace CAIGA;

WorkCanny::WorkCanny(const cv::Mat *s, int aSize, double high, double low, bool l2) :
    WorkBase(s)
{
    workType = Canny;
    oddSize = aSize;
    sigmaX = high;
    sigmaY = low;
    b = l2;
}

void WorkCanny::Func()
{
    cv::Canny(*src, *dst, sigmaX, sigmaY, oddSize, b);
}
