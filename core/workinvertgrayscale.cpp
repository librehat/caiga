#include "workinvertgrayscale.h"
using namespace CAIGA;

void WorkInvertGrayscale::Func()
{
    cv::bitwise_not(*src, *dst);
}
