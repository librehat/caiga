#ifndef WORKFLOODFILL_H
#define WORKFLOODFILL_H
#include "workbase.h"

namespace CAIGA
{
class WorkFloodFill : public WorkBase
{
public:
    WorkFloodFill(const cv::Mat *const src, double high, double low, bool con8, int x, int y);
    void Func();
};
}

#endif // WORKFLOODFILL_H
