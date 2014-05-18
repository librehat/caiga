#ifndef WORKAUTOWATERSHED_H
#define WORKAUTOWATERSHED_H

#include "workbase.h"

namespace CAIGA
{
class WorkAutoWatershed : public WorkBase
{
public:
    WorkAutoWatershed(const cv::Mat *src) : WorkBase(src) { workType = AutoWatershed; }
    void Func();
};
}

#endif // WORKAUTOWATERSHED_H
