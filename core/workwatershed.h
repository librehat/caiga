#ifndef WORKWATERSHED_H
#define WORKWATERSHED_H
#include "workbase.h"

namespace CAIGA
{
class WorkWatershed : public WorkBase
{
public:
    WorkWatershed(const cv::Mat *src, std::vector<std::vector<cv::Point> > m);
    void Func();
};
}

#endif // WORKWATERSHED_H
