#include "workscharr.h"
using namespace CAIGA;

void WorkScharr::Func()
{
    cv::Scharr(*src, *dst, -1, 1, 0);
}
