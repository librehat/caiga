#ifndef WORKBOXFILTER_H
#define WORKBOXFILTER_H
#include "workbase.h"

namespace CAIGA
{
class WorkBoxFilter : public WorkBase
{
public:
    WorkBoxFilter(const cv::Mat *src, int size)
        : WorkBase(src)
    {
        cvSize = cv::Size(size, size);
        workType = BoxFilter;
    }
    void Func();
};
}

#endif // WORKBOXFILTER_H
