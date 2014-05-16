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
    void setImage(Image *img);
    inline void setCircle(bool c) { *isCircle = c; }
    inline void setCircleCentre(QPoint c) { circleCentre = c; }
    inline void setCircleRadius(int r) { circleRadius = r; }
    inline void setRectangle(QRect r) { qrect = r; }
    inline void setScaleValue(qreal s) { *scaleValue = s; emit scaleValueChanged(*scaleValue); }

    inline bool getIsCircle() { return *isCircle; }
    inline qreal getScaleValue() { return *scaleValue; }

    void cropImage();
    void reset();

    QPoint circleCentre;
    int circleRadius;
    QRect qrect;

signals:
    void scaleValueChanged(qreal);

public slots:

private:
    Image *m_image;
    bool *isCircle;
    qreal *scaleValue;//pixel/μm

};
}

#endif // CCSPACE_H
