#ifndef WORKPENCIL_H
#define WORKPENCIL_H

#include "workbase.h"

namespace CAIGA
{
class WorkPencil : public WorkBase
{
public:
    WorkPencil(const cv::Mat *src, std::vector<cv::Point_<qreal> > p, int r, int g, int b)
        : WorkBase(src)
    {
        workType = Pencil;
        pointVec = p;
        oddSize = r;
        size = g;
        method = b;
    }
    void Func();
};
}

#endif // WORKPENCIL_H
