/*
 * Class Object is defined as a group of objects, which belong to the same class.
 * This class is used to get some class values as well as specific grain's value.
 * no need to store the information, just calculate and return them on the fly. (more CPU, less RAM)
 */

#ifndef CLASSOBJECT_H
#define CLASSOBJECT_H

#include "object.h"
#include "core_lib.h"

namespace CAIGA
{
class CORE_LIB ClassObject
{
public:
    ClassObject();

    int insideCount();
    int interceptedCount();
    inline int count() { return objects.size(); }
    qreal totalArea();
    qreal averageArea();//defined in ASTM E112-13
    qreal averagePerimeter();
    qreal averageFlattening();
    inline qreal averageDiameter() { return qSqrt(averageArea()); }
    inline qreal averageIntercept() const { return m_averageIntercept; }
    qreal sizeNumberByIntercept() const;//Heyn Lineal Intercept Procedure as defined in ASTM E112-13
    qreal sizeNumberByPlanimetric();//Planimetric procedure as defined in ASTM E112-13
    inline qreal percentage() const { return m_percentage; }
    inline qreal relativePercentage() const { return m_relativePercentage; }

    inline void setAverageIntercept(qreal a) { m_averageIntercept = a; }
    inline void setPercentage(qreal p) { m_percentage = p; }
    inline void setRelativePercentage(qreal p) { m_relativePercentage = p; }

    inline bool contains(int key) { return objects.contains(key); }
    inline void insert(int key, Object obj) { objects[key] = obj; }
    inline Object takeAt(int key) { return objects.take(key); }
    inline Object ObjectAt(int key) const { return objects.value(key); }
    inline QMap<int, Object> &rObjects() { return objects; }//return reference to objects

private:
    QMap<int, Object> objects;//key(id) is the index
    qreal m_percentage;
    qreal m_relativePercentage;// this totalArea / all classes' totalArea
    qreal m_averageIntercept;
};
}

#endif // CLASSOBJECT_H
