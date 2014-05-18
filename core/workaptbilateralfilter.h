#ifndef WORKAPTBILATERALFILTER_H
#define WORKAPTBILATERALFILTER_H
#include "workbase.h"

namespace CAIGA {
class WorkAptBilateralFilter : public WorkBase
{
public:
    WorkAptBilateralFilter(const cv::Mat *s, int k, double space, double colour);
    void Func();
};
}
#endif // WORKAPTBILATERALFILTER_H
