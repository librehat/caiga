#ifndef WORKHISTEQUALISE_H
#define WORKHISTEQUALISE_H
#include "workbase.h"
namespace CAIGA {
class CORE_LIB WorkHistEqualise : public WorkBase
{
public:
    WorkHistEqualise(const cv::Mat *const s) : WorkBase(s) {}
    void Func();
};
}
#endif // WORKHISTEQUALISE_H
