#include "workcontours.h"
using namespace CAIGA;

void WorkContours::Func()
{
    /*
     * use CV_RETR_EXTERNAL mode to avoid holes caused by preProcessing.
     * anyway, we don't have objects inside an object.
     * thus, we don't need hierarchy neither.
     */
    std::vector<std::vector<cv::Point> > allContours;
    cv::Mat temp = dst->clone();
    cv::findContours(temp, allContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    //remove those tiny contours since they usually are artefacts.
    for (int i = 0; i < static_cast<int>(allContours.size()); ++i) {
        if (cv::contourArea(allContours[i], false) > 20) {
            contours.push_back(allContours[i]);
        }
    }

    display = new cv::Mat(cv::Mat::zeros(dst->rows, dst->cols, CV_8UC3));
    //draw each contour in its own random colour
    const int size = static_cast<int>(contours.size());

    markerMatrix = new cv::Mat(dst->size(), CV_32S);
    *markerMatrix = cv::Scalar::all(-1);//set background to -1. those area that won't be filled are the boundary

    for (int idx = 0; idx < size ; ++idx) {
        cv::Scalar colour(rand() & 255, rand() & 255, rand() & 255);
        cv::drawContours(*display, contours, idx, colour, CV_FILLED, 8);
        cv::drawContours(*markerMatrix, contours, idx, cv::Scalar::all(idx + 1), CV_FILLED, 8);
    }
}
