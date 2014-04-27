/*
 * This is an abstract base class for varies of works.
 */

#ifndef WORKBASE_H
#define WORKBASE_H
#include <iostream>

namespace CAIGA {
class WorkBase
{
public:
    enum type {HistEqualise, AdaBilateralFilter, MedianBlur};
    virtual void Func() = 0;
};

}
#endif // WORKBASE_H
