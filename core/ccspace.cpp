#include "ccspace.h"
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
    cv::Rect rect;
    tl.x = qrect.topLeft().x();
    tl.y = qrect.topLeft().y();
    br.x = qrect.bottomRight().x();
    br.y = qrect.bottomRight().y();
    rect = cv::Rect(tl, br);
    m_image->croppedImage = m_image->rawImage(rect).clone();
}

void CCSpace::reset()
{
    qrect.setTopLeft(QPoint(0, 0));
    qrect.setBottomRight(QPoint(0, 0));
}
