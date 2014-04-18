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
};
}
#endif // DATASTRUCTURE_H
