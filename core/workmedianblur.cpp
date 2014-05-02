#include "workmedianblur.h"
using namespace CAIGA;

void WorkMedianBlur::Func()
{
    cv::medianBlur(*src, *dst, oddSize);
}
