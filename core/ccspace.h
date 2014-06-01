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
    explicit CCSpace(Image *img, QObject *parent = 0);
    inline void setRectangle(QRect r) { qrect = r; }
    inline void setScaleValue(qreal s) { *scaleValue = s; emit scaleValueChanged(*scaleValue); }

    inline qreal getScaleValue() { return *scaleValue; }

    void cropImage();
    void reset();
    QRect qrect;

signals:
    void scaleValueChanged(qreal);

public slots:

private:
    Image *m_image;
    qreal *scaleValue;//pixel/μm

};
}

#endif // CCSPACE_H
