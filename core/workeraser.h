#ifndef WORKERASER_H
#define WORKERASER_H

#include "workbase.h"

namespace CAIGA
{
class WorkEraser : public WorkBase
{
public:
    WorkEraser(const cv::Mat *src, std::vector<cv::Point> p, bool white)
        : WorkBase(src)
    {
        workType = Eraser;
        pointVec = p;
        b = white;
    }
    void Func();
};
}

#endif // WORKERASER_H
