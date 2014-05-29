#include "classobject.h"
using namespace CAIGA;

ClassObject::ClassObject()
{
    m_percentage = 0;
    m_averageIntercept = 0;
}

int ClassObject::boundaryCount()
{
    int b = 0;
    for (QMap<int, Object>::iterator it = objects.begin(); it != objects.end(); ++it) {
        if (it->boundary()) {
            ++b;
        }
    }
    return b;
}

qreal ClassObject::totalArea()
{
    qreal A = 0;
    for (QMap<int, Object>::iterator it = objects.begin(); it != objects.end(); ++it) {
        A += it->area();
    }
    return A;
}

qreal ClassObject::averageArea()
{
    qreal total = totalArea();
    qreal avg = total / (static_cast<qreal>(count()) - static_cast<qreal>(boundaryCount())/2.0 + 1.0);//ASTM E112-12 Eq.5
    return avg;
}

qreal ClassObject::averagePerimeter()
{
    qreal total = 0, avg = 0;
    for (QMap<int, Object>::iterator it = objects.begin(); it != objects.end(); ++it) {
        total += it->perimeter();
    }
    avg = total / (static_cast<qreal>(count()) - static_cast<qreal>(boundaryCount())/2.0 + 1.0);
    return avg;
}

qreal ClassObject::averageFlattening()
{
    qreal total = 0, avg = 0;
    for (QMap<int, Object>::iterator it = objects.begin(); it != objects.end(); ++it) {
        total += it->flattening();
    }
    avg = total / static_cast<qreal>(count());
    return avg;
}

qreal ClassObject::sizeNumberByIntercept() const
{
    /*
     * calculate the grain size level using intercept procedure
     * defined in GB/T 6394-2002 and ASTM E112-12
     * intercepts length need to be converted to minimeter (divided by 1000)
     */
    return (-6.643856 * log10(m_averageIntercept / 1000)) - 3.288;
}

qreal ClassObject::sizeNumberByPlanimetric()
{
    //the planimetric procedure shall be the referee procedure in all cases (ASTM E112-13 4.3)
    qreal area = totalArea() / 1000000;//convert to minimeter's square
    //return (3.321928 * log10((static_cast<qreal>(count()) - static_cast<qreal>(boundaryCount())/2.0 + 1.0) / area) - 2.954);
    //ASTM E112-12 Table 6
    return 3.321928 * log10((static_cast<qreal>(count()) - static_cast<qreal>(boundaryCount())/2.0 + 1.0) / area) - 2.954;
}
