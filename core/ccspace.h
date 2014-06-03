#ifndef CCSPACE_H
#define CCSPACE_H

#include <QObject>
#include <QTransform>
#include "core_lib.h"
#include "image.h"

namespace CAIGA
{
class CORE_LIB CCSpace : public QObject
{
    Q_OBJECT
public:
    explicit CCSpace(Image *img, QObject *parent = 0);
    inline void setRectangle(QRectF r) { qrect = r; }
    void setScaleValue(qreal bar, qreal realLength);

    inline qreal getScaleValue() { return *scaleValue; }
    inline QTransform getInvertedTransform() const { return invertedTransformer; }
    inline QTransform getTransform() const { return transformer; }
    inline void setTransform(const QTransform &t) { transformer = t; invertedTransformer = t.inverted(); }

    void cropImage();
    void reset();
    QRectF qrect;//using float coordinates

signals:
    void scaleValueChanged(qreal);

public slots:
    inline void setScaleValue(qreal r) { *scaleValue = r; emit scaleValueChanged(r); }

private:
    Image *m_image;
    qreal *scaleValue;//pixel/μm
    QTransform transformer;
    QTransform invertedTransformer;

};
}

#endif // CCSPACE_H
