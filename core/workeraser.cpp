#include "workeraser.h"
using namespace CAIGA;

void WorkEraser::Func()
{
    cv::Scalar colour = b ? cv::Scalar(255, 255, 255) : cv::Scalar(0, 0, 0);
    for (std::vector<cv::Point>::iterator it = pointVec.begin(); it != pointVec.end(); ++it) {
        cv::circle(*dst, *it, 2, colour, 3, CV_AA);//CV_AA ensures the consistency and fluency
    }
}
