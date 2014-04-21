#include <QPoint>
#include <QRect>
#include <QLine>
#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H
namespace CAIGA {
/*
 * ccStruct contains informations during crop & calibre
 */
struct ccStruct
{
    int drawMode;
    bool isCircle;
    QPoint pressed;
    QPoint released;
    QPoint centre;
    int radius;
    QRect rect;
    QLine calibreLine;
    qreal calibre;//pixel/μm

    ccStruct() : drawMode(-2), isCircle(true), pressed(), released(), centre(), radius(0), rect(), calibreLine(), calibre(0) {}
};

/*
 * QtConcurrent taks at most 5 parameteres.
 * Thus, we defined this struct to pass more parameters if needed.
 * notice "S" is upper-case.
 */
struct parameterS{
    int i1;
    int i2;
    int i3;
    double d1;
    double d2;
    bool b;

    parameterS() : i1(0), i2(0), i3(0), d1(0), d2(0), b(false) {}
};
}
#endif // DATASTRUCTURE_H
