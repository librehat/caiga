#ifndef WORKERASER_H
#define WORKERASER_H

#include "workbase.h"

namespace CAIGA
{
class WorkEraser : public WorkBase
{
public:
    WorkEraser(const cv::Mat *const src, std::vector<cv::Point> p, bool white) : WorkBase(src) { pointVec = p; b = white; }
    void Func();
};
}

#endif // WORKERASER_H
