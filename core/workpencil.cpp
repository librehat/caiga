#include "workpencil.h"
using namespace CAIGA;

void WorkPencil::Func()
{
    cv::Scalar colour = b ? cv::Scalar(255, 255, 255) : cv::Scalar(0, 0, 0);
    for (std::vector<cv::Point>::iterator it = pointVec.begin(); it != pointVec.end() - 1; ++it) {
        cv::line(*dst, *it, *(it + 1), colour, 2, CV_AA);
    }
}
