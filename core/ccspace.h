#ifndef CCSPACE_H
#define CCSPACE_H

#include <QObject>
#include "core_lib.h"
#include "image.h"

namespace CAIGA
{
class CORE_LIB CCSpace : public QObject
{
    Q_OBJECT
public:
    explicit CCSpace(Image *img = NULL, QObject *parent = 0);
    inline void setImage(Image *img) { m_image = img; }
    inline void setCircle(bool c) { isCircle = c; m_image->m_isCircle = c; }
    inline void setDrawMode(int m) { drawMode = m; }
    inline void setMousePressed(QPoint p) { mousePressed = p; }
    inline void setMouseReleased(QPoint r) { mouseReleased = r; }
    inline void setCircleCentre(QPoint c) { circleCentre = c; }
    inline void setCircleRadius(int r) { circleRadius = r; }
    inline void setRectangle(QRect r) { qrect = r; }
    inline void setCalibreLine(QLine l) { calibreLine = l; }
    inline void setScaleValue(qreal s) { scaleValue = s; }
    void cropImage();
    void reset();

    Image *m_image;
    int drawMode;
    bool isCircle;
    QPoint mousePressed;
    QPoint mouseReleased;
    QPoint circleCentre;
    int circleRadius;
    QRect qrect;
    QLine calibreLine;
    qreal scaleValue;//pixel/μm

signals:

public slots:

private:


};
}

#endif // CCSPACE_H
