#include "workfloodfill.h"
using namespace CAIGA;

WorkFloodFill::WorkFloodFill(const cv::Mat * const src, double high, double low, bool con8, int x, int y) :
    WorkBase(src)
{
    if (dst->type() == CV_8UC1) {
        cv::cvtColor(*src, *dst, CV_GRAY2RGB);
    }
    sigmaX = high;
    sigmaY = low;
    b = con8;
    method = x;
    type = y;
}

void WorkFloodFill::Func()
{
    cv::Point seed(method, type);
    int flags = (b ? 8 : 4) + (255 << 8) + (sigmaY == 0 ? 0 : CV_FLOODFILL_FIXED_RANGE);
    int blue = (unsigned)cv::theRNG() & 255;
    int green = (unsigned)cv::theRNG() & 255;
    int red = (unsigned)cv::theRNG() & 255;
    cv::Scalar colour(blue, green, red);
    cv::Rect ccomp;
    cv::floodFill(*dst, seed, colour, &ccomp, cv::Scalar(sigmaY, sigmaY, sigmaY), cv::Scalar(sigmaX, sigmaX, sigmaX), flags);
}
