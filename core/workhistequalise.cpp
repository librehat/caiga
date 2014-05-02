#include "workhistequalise.h"
using namespace CAIGA;

void WorkHistEqualise::Func()
{
    cv::equalizeHist(*src, *dst);
}
