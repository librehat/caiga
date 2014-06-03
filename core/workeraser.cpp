#include "workeraser.h"
using namespace CAIGA;

void WorkEraser::Func()
{
    cv::Scalar colour = b ? cv::Scalar(255, 255, 255) : cv::Scalar(0, 0, 0);
    for (std::vector<cv::Point_<qreal> >::iterator it = pointVec.begin(); it != pointVec.end() - 1; ++it) {
        cv::line(*dst, *it, *(it + 1), colour, 7, 8);
    }
}
