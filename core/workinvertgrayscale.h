#ifndef WORKINVERTGRAYSCALE_H
#define WORKINVERTGRAYSCALE_H

#include "workbase.h"
namespace CAIGA
{
class WorkInvertGrayscale : public WorkBase
{
public:
    WorkInvertGrayscale(const cv::Mat *const src) : WorkBase(src) { workType = InvertGrayscale; }
    void Func();
};
}

#endif // WORKINVERTGRAYSCALE_H
