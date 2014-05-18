#ifndef WORKSCHARR_H
#define WORKSCHARR_H

#include "workbase.h"

namespace CAIGA
{
class WorkScharr : public WorkBase
{
public:
    WorkScharr(const cv::Mat *src) : WorkBase(src) { workType = Scharr; }
    void Func();
};
}

#endif // WORKSCHARR_H
