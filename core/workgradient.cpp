#include "workgradient.h"
using namespace CAIGA;

WorkGradient::WorkGradient(const cv::Mat * const src, int kSize) :
    WorkBase(src)
{
    workType = Gradient;
    cvSize = cv::Size(kSize, kSize);
}

void WorkGradient::Func()
{
    cv::Mat kernel = cv::getStructuringElement(1, cvSize);//0: Rect; 1: Cross; 2: Ellipse
    cv::morphologyEx(*src, *dst, cv::MORPH_GRADIENT, kernel);
}
