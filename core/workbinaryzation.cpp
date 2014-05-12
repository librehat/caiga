#include "workbinaryzation.h"
using namespace CAIGA;

WorkBinaryzation::WorkBinaryzation(const cv::Mat * const s, int m, int t, int size, double C) :
    WorkBase(s)
{
    workType = Binaryzation;
    method = m;
    type = t;
    oddSize = size;
    constant = C;
}

void WorkBinaryzation::Func()
{
    if (oddSize % 2 == 1 && oddSize > 1) {
        cv::adaptiveThreshold(*src, *dst, 255, method, type, oddSize, constant);
    }
    else
        qWarning() << "Assertion failed (blockSize % 2 == 1 && blockSize > 1) blockSize is " << oddSize;
}
