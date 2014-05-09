#ifndef WORKCONTOURS_H
#define WORKCONTOURS_H
#include "workbase.h"
namespace CAIGA
{
class WorkContours : public WorkBase
{
public:
    WorkContours(const cv::Mat *const s) : WorkBase(s) {}
    void Func();
};
}
#endif // WORKCONTOURS_H
