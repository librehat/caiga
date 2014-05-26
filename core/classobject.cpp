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
    qreal avg = total / (static_cast<qreal>(count()) - static_cast<qreal>(boundaryCount())/2.0 - 1.0);
    return avg;
}

qreal ClassObject::averagePerimeter()
{
    qreal total = 0, avg = 0;
    for (QMap<int, Object>::iterator it = objects.begin(); it != objects.end(); ++it) {
        total += it->perimeter();
    }
    avg = total / (static_cast<qreal>(count()) - static_cast<qreal>(boundaryCount())/2.0 - 1.0);
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

qreal ClassObject::sizeLevel() const
{
    /*
     * calculate the grain size level using intercept method
     * which is noted as a standard way in GB/T 6394-2002
     * intercepts length need to be converted to minimeter (divided by 1000)
     */
    return (-6.643856 * log10(m_averageIntercept / 1000)) - 3.288;
}
