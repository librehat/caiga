#include "workfloodfill.h"
using namespace CAIGA;

WorkFloodFill::WorkFloodFill(const cv::Mat *src, double high, double low, bool con8, std::vector<cv::Point> pts) :
    WorkBase(src)
{
    workType = FloodFill;
    sigmaX = high;
    sigmaY = low;
    b = con8;
    pointVec = pts;
}

void WorkFloodFill::Func()
{
    int flags = (b ? 8 : 4) + (255 << 8) + (sigmaY == 0 ? 0 : CV_FLOODFILL_FIXED_RANGE);
    cv::Rect ccomp;
    display = new cv::Mat(dst->size(), CV_8UC3);
    cv::cvtColor(*src, *display, CV_GRAY2BGR);
    delete dst;
    dst = new cv::Mat(src->rows + 2, src->cols + 2, CV_8UC1);
    //using display as mask
    //using point vector to iterate
    for (std::vector<cv::Point>::iterator it = pointVec.begin(); it != pointVec.end(); ++it) {
        cv::Point seed(it->x, it->y);
        uchar b = static_cast<uchar>(cv::theRNG().uniform(0, 255));
        uchar g = static_cast<uchar>(cv::theRNG().uniform(0, 255));
        uchar r = static_cast<uchar>(cv::theRNG().uniform(0, 255));
        cv::Scalar colour(b, g, r);
        cv::floodFill(*display, *dst, seed, colour, &ccomp, cv::Scalar(sigmaY, sigmaY, sigmaY), cv::Scalar(sigmaX, sigmaX, sigmaX), flags);
    }
}
