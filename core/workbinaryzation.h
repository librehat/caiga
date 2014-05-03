#ifndef WORKBINARYZATION_H
#define WORKBINARYZATION_H
#include "workbase.h"
namespace CAIGA
{
class CORE_LIB WorkBinaryzation : public WorkBase
{
public:
    WorkBinaryzation(cv::Mat *s, int m, int t, int size, double C);
    void Func();
};
}
#endif // WORKBINARYZATION_H
