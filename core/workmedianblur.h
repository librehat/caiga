#ifndef WORKMEDIANBLUR_H
#define WORKMEDIANBLUR_H
#include "workbase.h"
namespace CAIGA {
class CORE_LIB WorkMedianBlur : public WorkBase
{
public:
    WorkMedianBlur(cv::Mat *s, int k) : WorkBase(s) { oddSize = k; }
    void Func();
};
}
#endif // WORKMEDIANBLUR_H
