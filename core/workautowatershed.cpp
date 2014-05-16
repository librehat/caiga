#include "workautowatershed.h"

using namespace CAIGA;

void WorkAutoWatershed::Func()
{
    //code is based on Page 109 of Pratical OpenCV

    cv::Mat temp = dst->clone();
    /*
     * euqalise the histogram to improve contrast. but noises grow as well.
     * just disable it for now.
     * we should use adaptive histogram equalisation if it's usable in future version of openCV
     */
    //cv::equalizeHist(*src, temp);

    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::erode(temp, *dst, element);//erode to remove small white spots
    /*
     * dilate to remove small black spots, but our grains are black.
     * so we use erode above to remove white spots instead of dialte.
     */
    //cv::dilate(temp, *dst, element);

    //open and close to highlight the objects
    element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(13, 13));
    cv::morphologyEx(*dst, temp, cv::MORPH_OPEN, element);
    cv::morphologyEx(temp, *dst, cv::MORPH_CLOSE, element);

    //create binary image
    cv::adaptiveThreshold(*dst, temp, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 105, 0);

    //erode twice to seperate regions
    cv::erode(temp, *dst, element, cv::Point(-1, -1), 2);

    /*
     * note dst becomes a Mat which contains only the generated seeds.
     * we could find a way to display *dst on watershed dialogue.
     */

    std::vector<std::vector<cv::Point> > c;
    cv::findContours(*dst, c, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS);

    for (int i = 0; i < static_cast<int>(c.size()); ++i) {
        if (cv::contourArea(c[i], false) > 20) {
            contours.push_back(c[i]);
        }
    }

    const int compCount = static_cast<int>(contours.size());
    markerMatrix = new cv::Mat(dst->size(), CV_32S);
    *markerMatrix = cv::Scalar::all(0);//0-value pixels' relation to outlined regions are determined by watershed algorithm
    std::vector<cv::Vec3b> colourTab;//store random colour values

    for (int idx = 0; idx < compCount; ++idx) {
        cv::drawContours(*markerMatrix, contours, idx, cv::Scalar::all(idx + 1), CV_FILLED, 8);//idx + 1 because 0 is reserved for watershed algorithm
        uchar b = static_cast<uchar>(cv::theRNG().uniform(0, 255));
        uchar g = static_cast<uchar>(cv::theRNG().uniform(0, 255));
        uchar r = static_cast<uchar>(cv::theRNG().uniform(0, 255));
        colourTab.push_back(cv::Vec3b(b, g, r));
    }

    cv::Mat imgColor;
    cv::cvtColor(*src, imgColor, CV_GRAY2BGR);

    cv::watershed(imgColor, *markerMatrix);

    display = new cv::Mat(markerMatrix->size(), CV_8UC3);
    // paint the watershed image
    for(int i = 0; i < markerMatrix->rows; ++i) {
        for(int j = 0; j < markerMatrix->cols; ++j) {
            int index = markerMatrix->at<int>(i, j);
            if(index == -1) {//means edges
                display->at<cv::Vec3b>(i, j) = cv::Vec3b(255, 255, 255);//white line
            }
            else if( index <= 0 || index > compCount ) {//should not get here
                display->at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
            }
            else {//inside a region
                display->at<cv::Vec3b>(i, j) = colourTab[index - 1];//defined with its idx
            }
        }
    }
    *display = (*display) * 0.5 + imgColor * 0.5;
}
