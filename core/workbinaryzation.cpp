#include "workbinaryzation.h"
using namespace CAIGA;

WorkBinaryzation::WorkBinaryzation(cv::Mat *s, int m, int t, int size, double C) :
    WorkBase(s)
{
    method = m;
    type = t;
    oddSize = size;
    constant = C;
}

void WorkBinaryzation::Func()
{
    cv::adaptiveThreshold(*src, *dst, 255, method, type, oddSize, constant);
}
