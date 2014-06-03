#include "workpencil.h"
using namespace CAIGA;

void WorkPencil::Func()
{
    cv::Scalar colour(oddSize, size, method);
    for (std::vector<cv::Point_<qreal> >::iterator it = pointVec.begin(); it != pointVec.end() - 1; ++it) {
        cv::line(*dst, *it, *(it + 1), colour, 2, 8);
    }
}
