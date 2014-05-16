#ifndef WORKSCHARR_H
#define WORKSCHARR_H

#include "workbase.h"

namespace CAIGA
{
class WorkScharr : public WorkBase
{
public:
    WorkScharr(const cv::Mat *const src) : WorkBase(src) { workType = Scharr; }
    void Func();
};
}

#endif // WORKSCHARR_H
