#include "ccspace.h"
#include <QDebug>
using namespace CAIGA;

CCSpace::CCSpace(Image *img, QObject *parent) :
    QObject(parent)
{
    m_image = img;
    scaleValue = &img->m_scale;
    reset();
}

void CCSpace::cropImage()
{
    cv::Point tl, br;
    bool ok;
    QTransform iTrans = transformer.inverted(&ok);
    if (!ok) {
        qCritical() << "Criticical. Coordinate transforming failed.";
        return;
    }
    QPointF qtl = iTrans.map(qrect.topLeft());
    QPointF qbr = iTrans.map(qrect.bottomRight());
    tl.x = qtl.x();
    tl.y = qtl.y();
    br.x = qbr.x();
    br.y = qbr.y();
    cv::Rect_<qreal> rect(tl, br);
    m_image->croppedImage = m_image->rawImage(rect).clone();
}

void CCSpace::reset()
{
    qrect.setTopLeft(QPointF(0, 0));
    qrect.setBottomRight(QPointF(0, 0));
}
