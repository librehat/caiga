#ifndef WORKMEDIANBLUR_H
#define WORKMEDIANBLUR_H
#include "workbase.h"
namespace CAIGA {
class WorkMedianBlur : public WorkBase
{
public:
    WorkMedianBlur(const cv::Mat *const s, int k) : WorkBase(s) { oddSize = k; }
    void Func();
};
}
#endif // WORKMEDIANBLUR_H
