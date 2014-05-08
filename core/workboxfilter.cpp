#include "workboxfilter.h"
using namespace CAIGA;

void WorkBoxFilter::Func()
{
    cv::boxFilter(*src, *dst, -1, cvSize);
}
