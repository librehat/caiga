#ifndef WORKCANNY_H
#define WORKCANNY_H
#include "workbase.h"

namespace CAIGA
{
class WorkCanny : public WorkBase
{
public:
    WorkCanny(const cv::Mat *s, int aSize, double high, double low, bool l2);
    void Func();
};
}
#endif // WORKCANNY_H
