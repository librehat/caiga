#include "workwatershed.h"
using namespace CAIGA;

WorkWatershed::WorkWatershed(const cv::Mat * const src, std::vector<std::vector<cv::Point> > m) : WorkBase (src)
{
    workType = Watershed;
    markers = m;
}

void WorkWatershed::Func()
{
    cv::Mat markerMask = dst->clone();
    cv::Mat imgColourful;
    cv::cvtColor(*dst, imgColourful, CV_GRAY2RGB);
    markerMask = cv::Scalar::all(0);
    for(std::vector<std::vector<cv::Point> >::iterator it = markers.begin(); it != markers.end(); ++it) {
        if ((*it).size() < 2) {
            continue;
        }
        for(std::vector<cv::Point>::iterator pit = (*it).begin(); pit != (*it).end() - 1; ++pit) {
            cv::line(markerMask, *pit, *(pit + 1), cv::Scalar::all(255), 2, CV_AA);
        }
    }

    cv::findContours(markerMask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS);

    if (contours.empty()) {
        qWarning() << "Watershed Aborted. Cannot find contours.";
        return;
    }

    const int compCount = static_cast<int>(contours.size());
    cv::Mat markerMatrix(markerMask.size(), CV_32S);
    markerMatrix = cv::Scalar::all(0);
    std::vector<cv::Vec3b> colourTab;//store random colour values

    for (int idx = 0; idx < compCount; ++idx) {
        cv::drawContours(markerMatrix, contours, idx, cv::Scalar::all(idx + 1), CV_FILLED, 8);
        uchar b = static_cast<uchar>(cv::theRNG().uniform(0, 255));
        uchar g = static_cast<uchar>(cv::theRNG().uniform(0, 255));
        uchar r = static_cast<uchar>(cv::theRNG().uniform(0, 255));
        colourTab.push_back(cv::Vec3b(b, g, r));
    }

    cv::watershed(imgColourful, markerMatrix);

    display = new cv::Mat(markerMatrix.size(), CV_8UC3);
    // paint the watershed image
    for(int i = 0; i < markerMatrix.rows; ++i) {
        for(int j = 0; j < markerMatrix.cols; ++j) {
            int index = markerMatrix.at<int>(i, j);
            if(index == -1) {//means edges
                display->at<cv::Vec3b>(i, j) = cv::Vec3b(255, 255, 255);
            }
            else if( index <= 0 || index > compCount ) {//should not get here
                display->at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
            }
            else {//inside a region
                display->at<cv::Vec3b>(i, j) = colourTab[index - 1];
            }
        }
    }
    *display = (*display) * 0.6 + imgColourful * 0.4;
}
