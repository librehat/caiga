#include "workcontours.h"
using namespace CAIGA;

void WorkContours::Func()
{
    /*
     * use CV_RETR_EXTERNAL mode to avoid holes caused by preProcessing.
     * anyway, we don't have objects inside an object.
     * thus, we don't need hierarchy neither.
     */
    cv::findContours(*dst, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS);

    display = new cv::Mat(cv::Mat::zeros(dst->rows, dst->cols, CV_8UC3));
    //draw each contour in its own random colour
    const int size = static_cast<int>(contours.size());
    for (int idx = 0; idx < size ; ++idx) {
        cv::Scalar colour(rand() & 255, rand() & 255, rand() & 255);
        cv::drawContours(*display, contours, idx, colour, CV_FILLED, 8);
    }
}
