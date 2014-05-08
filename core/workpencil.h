#ifndef WORKPENCIL_H
#define WORKPENCIL_H

#include "workbase.h"

namespace CAIGA
{
class WorkPencil : public WorkBase
{
public:
    WorkPencil(const cv::Mat *const src, std::vector<cv::Point> p, bool white) : WorkBase(src) { pointVec = p; b = white; }
    void Func();
};
}

#endif // WORKPENCIL_H
