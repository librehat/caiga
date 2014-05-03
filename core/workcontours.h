#ifndef WORKCONTOURS_H
#define WORKCONTOURS_H
#include "workbase.h"
namespace CAIGA
{
class CORE_LIB WorkContours : public WorkBase
{
public:
    WorkContours(cv::Mat *s) : WorkBase(s) {}
    void Func();
};
}
#endif // WORKCONTOURS_H
