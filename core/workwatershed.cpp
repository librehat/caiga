#include "workwatershed.h"
using namespace CAIGA;

WorkWatershed::WorkWatershed(const cv::Mat *src, const cv::Mat *marker) : WorkBase (src)
{
    inputMarker = new cv::Mat(marker->clone());
    workType = Watershed;
}

void WorkWatershed::Func()
{
    cv::Mat imgColourful;
    cv::cvtColor(*dst, imgColourful, CV_GRAY2BGR);
    cv::Mat temp = inputMarker->clone();

    //remove those contours whose are are smaller than 5 pixels, which causes fitEllipse failed.
    std::vector<std::vector<cv::Point> > tempContours;
    //findContours doesn't take in array of float-point array
    cv::findContours(temp, tempContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    for (int i = 0; i < static_cast<int>(tempContours.size()); ++i) {
        if (cv::contourArea(tempContours[i]) > 5) {
            contours.push_back(tempContours[i]);
        }
    }

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
    /*
     * paint the watershed image
     * using STL-style iterator
     */
    cv::MatIterator_<cv::Vec3b> dit = display->begin<cv::Vec3b>();
    for (cv::MatConstIterator_<int> it = markerMatrix->begin<int>(); it != markerMatrix->end<int>(); ++it, ++dit) {
        if ((*it) == -1) {//means edges
            (*dit) = cv::Vec3b(255, 255, 255);//white boundary
        }
        else if ((*it) <=0 || (*it) > compCount) {//should not get here
            (*dit) = cv::Vec3b(0, 0, 0);
        }
        else {
            (*dit) = colourTab[(*it) - 1];//defined with its idx
        }
    }
}
