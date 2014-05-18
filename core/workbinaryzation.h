#ifndef WORKBINARYZATION_H
#define WORKBINARYZATION_H
#include "workbase.h"
namespace CAIGA
{
class WorkBinaryzation : public WorkBase
{
public:
    WorkBinaryzation(const cv::Mat *s, int m, int t, int size, double C);
    void Func();
};
}
#endif // WORKBINARYZATION_H
