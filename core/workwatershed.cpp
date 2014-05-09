#include "workwatershed.h"
using namespace CAIGA;

WorkWatershed::WorkWatershed(const cv::Mat * const src, std::vector<std::vector<cv::Point> > m) : WorkBase (src)
{
    if (dst->type() == CV_8UC1) {
        cv::cvtColor(*src, *dst, CV_GRAY2RGB);
    }

    markers = m;
}

void WorkWatershed::Func()
{
    cv::Mat markerMask;
    cv::cvtColor(*dst, markerMask, CV_RGB2GRAY);
    cv::Mat img(dst->clone());
    markerMask = cv::Scalar::all(0);
    for(std::vector<std::vector<cv::Point> >::iterator it = markers.begin(); it != markers.end(); ++it) {
        if ((*it).size() < 2) {
            continue;
        }
        for(std::vector<cv::Point>::iterator pit = (*it).begin(); pit != (*it).end() - 1; ++pit) {
            cv::line(markerMask, *pit, *(pit + 1), cv::Scalar::all(255), 2, CV_AA);
        }
    }

    int compCount = 0;
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;

    cv::findContours(markerMask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS);

    if (contours.empty()) {
        qWarning() << "Watershed Aborted. Cannot find contours.";
    }
    cv::Mat markerMatrix(markerMask.size(), CV_32S);
    markerMatrix = cv::Scalar::all(0);
    int idx = 0;
    for (; idx >= 0; idx = hierarchy[idx][0], ++compCount) {
        cv::drawContours(markerMatrix, contours, idx, cv::Scalar::all(compCount + 1), -1, 8, hierarchy, INT_MAX);
    }

    if (compCount == 0) {
        qWarning() << "Watershed Aborted. Cannot find contours.";
    }

    std::vector<cv::Vec3b> colorTab;
    for (int i = 0; i < compCount; ++i) {
        int b = cv::theRNG().uniform(0, 255);
        int g = cv::theRNG().uniform(0, 255);
        int r = cv::theRNG().uniform(0, 255);
        colorTab.push_back(cv::Vec3b((uchar)b, (uchar)g, (uchar)r));
    }

    cv::watershed(img, markerMatrix);
    *dst = cv::Mat(markerMatrix.size(), CV_8UC3);
    // paint the watershed image
    for(int i = 0; i < markerMatrix.rows; ++i) {
        for(int j = 0; j < markerMatrix.cols; ++j) {
            int index = markerMatrix.at<int>(i, j);
            if(index == -1) {
                dst->at<cv::Vec3b>(i, j) = cv::Vec3b(255,255,255);
            }
            else if( index <= 0 || index > compCount ) {
                dst->at<cv::Vec3b>(i, j) = cv::Vec3b(0,0,0);
            }
            else {
                dst->at<cv::Vec3b>(i, j) = colorTab[index - 1];
            }
        }
    }
    *dst = (*dst) * 0.5 + img * 0.5;
}
