#ifndef WORKFLOODFILL_H
#define WORKFLOODFILL_H
#include "workbase.h"

namespace CAIGA
{
class WorkFloodFill : public WorkBase
{
public:
    WorkFloodFill(const cv::Mat *src, double high, double low, bool con8, std::vector<cv::Point_<qreal> > pts);
    void Func();
};
}

#endif // WORKFLOODFILL_H
