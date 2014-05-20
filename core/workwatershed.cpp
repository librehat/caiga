#include "workwatershed.h"
using namespace CAIGA;

WorkWatershed::WorkWatershed(const cv::Mat *src, const cv::Mat *marker) : WorkBase (src)
{
    workType = Watershed;
    inputMarker = marker;
}

void WorkWatershed::Func()
{
    cv::Mat imgColourful;
    cv::cvtColor(*dst, imgColourful, CV_GRAY2BGR);
    cv::Mat temp = inputMarker->clone();
    cv::findContours(temp, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    if (contours.empty()) {
        qWarning() << "Watershed Aborted. Cannot find contours.";
        return;
    }

    const int compCount = static_cast<int>(contours.size());
    markerMatrix = new cv::Mat(temp.size(), CV_32S);
    *markerMatrix = cv::Scalar::all(0);//0-value pixels' relation to outlined regions are determined by watershed algorithm
    std::vector<cv::Vec3b> colourTab;//store random colour values

    for (int idx = 0; idx < compCount; ++idx) {
        cv::drawContours(*markerMatrix, contours, idx, cv::Scalar::all(idx + 1), CV_FILLED, 8);//idx + 1 because 0 is reserved for watershed algorithm
        uchar b = static_cast<uchar>(cv::theRNG().uniform(0, 255));
        uchar g = static_cast<uchar>(cv::theRNG().uniform(0, 255));
        uchar r = static_cast<uchar>(cv::theRNG().uniform(0, 255));
        colourTab.push_back(cv::Vec3b(b, g, r));
    }

    cv::watershed(imgColourful, *markerMatrix);

    display = new cv::Mat(markerMatrix->size(), CV_8UC3);
    // paint the watershed image
    for(int i = 0; i < markerMatrix->rows; ++i) {
        for(int j = 0; j < markerMatrix->cols; ++j) {
            int index = markerMatrix->at<int>(i, j);
            if(index == -1) {//means edges
                display->at<cv::Vec3b>(i, j) = cv::Vec3b(255, 255, 255);//white line
                //dst->at<uchar>(i, j) = static_cast<uchar>(255);
            }
            else if( index <= 0 || index > compCount ) {//should not get here
                display->at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
                //dst->at<uchar>(i, j) = static_cast<uchar>(0);
            }
            else {//inside a region
                display->at<cv::Vec3b>(i, j) = colourTab[index - 1];//defined with its idx
                //dst->at<uchar>(i, j) = static_cast<uchar>(0);
            }
        }
    }
}
