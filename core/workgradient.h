#ifndef WORKGRADIENT_H
#define WORKGRADIENT_H

#include "workbase.h"

namespace CAIGA
{
class WorkGradient : public WorkBase
{
public:
    WorkGradient(const cv::Mat *const src, int kSize);
    void Func();
};
}

#endif // WORKGRADIENT_H
