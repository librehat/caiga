#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H
#include <QPoint>
#include <QRect>
#include <QLine>
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

}
#endif // DATASTRUCTURE_H
