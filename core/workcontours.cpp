#include "workcontours.h"
using namespace CAIGA;

void WorkContours::Func()
{
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::Mat temp = src->clone();
    cv::findContours(temp, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_L1);//use CV_RETR_EXTERNAL mode to avoid holes caused by preProcessing. anyway, we don't have grains inside grains.

    *dst = cv::Mat::zeros(temp.rows, temp.cols, CV_8UC3);
    //draw each contour in its own random colour
    int idx = 0;
    for (; idx >= 0 ; idx = hierarchy[idx][0]) {
        cv::Scalar colour(rand() & 255, rand() & 255, rand() & 255);
        cv::drawContours(*dst, contours, idx, colour, CV_FILLED, CV_AA, hierarchy);
    }
}
