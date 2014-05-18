#ifndef WORKMEDIANBLUR_H
#define WORKMEDIANBLUR_H
#include "workbase.h"
namespace CAIGA {
class WorkMedianBlur : public WorkBase
{
public:
    WorkMedianBlur(const cv::Mat *s, int k) : WorkBase(s) { oddSize = k; workType = MedianBlur; }
    void Func();
};
}
#endif // WORKMEDIANBLUR_H
