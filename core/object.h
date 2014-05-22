/*
 * Object includes but not just grains.
 * use this class to simplify the calculate procedure.
 */

#ifndef OBJECT_H
#define OBJECT_H

#include <QtCore>

namespace CAIGA
{
class Object
{
public:
    Object();
    Object(bool b, qreal a, qreal p, qreal d, qreal f) : m_boundary(b), m_area(a), m_perimeter(p), m_diameter(d), m_flattening(f) {}
    inline bool boundary() const { return m_boundary; }
    inline qreal area() const { return m_area; }
    inline qreal perimeter() const { return m_perimeter; }
    inline qreal diameter() const { return m_diameter; }
    inline qreal flattening() const { return m_flattening; }

    inline void setBoundary(bool b) { m_boundary = b; }
    inline void setArea(qreal a) { m_area = a; }
    inline void setPerimeter(qreal p) { m_perimeter = p; }
    inline void setDiameter(qreal d) { m_diameter = d; }
    inline void setFlattening(qreal f) { m_flattening = f; }

private:
    bool m_boundary;
    qreal m_area;
    qreal m_perimeter;
    qreal m_diameter;
    qreal m_flattening;
};

}
#endif // OBJECT_H
