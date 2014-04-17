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
};
}
#endif // DATASTRUCTURE_H
