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

void CCSpace::setScaleValue(qreal bar, qreal realLength)
{
     *scaleValue = bar / realLength;
    emit scaleValueChanged(*scaleValue);
}

void CCSpace::cropImage()
{
    cv::Point_<qreal> tl, br;
    QPointF qtl = qrect.topLeft();
    QPointF qbr = qrect.bottomRight();
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
