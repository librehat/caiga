#include "ccspace.h"
#include <QDebug>
using namespace CAIGA;

CCSpace::CCSpace(Image *img, QObject *parent) :
    QObject(parent)
{
    setImage(img);
    reset();
}

void CCSpace::setImage(Image *img)
{
    m_image = img;
    scaleValue = img == NULL ? NULL : &img->m_scale;
}

void CCSpace::cropImage()
{
    if (m_image == NULL) {
        qWarning() << "Abort. Image pointer is NULL.";
        return;
    }
    cv::Point tl, br;
    cv::Rect enclosingRect;
    tl.x = qrect.topLeft().x();
    tl.y = qrect.topLeft().y();
    br.x = qrect.bottomRight().x();
    br.y = qrect.bottomRight().y();
    enclosingRect = cv::Rect(tl, br);
    m_image->croppedImage = m_image->rawImage(enclosingRect).clone();
}

void CCSpace::reset()
{
    qrect.setTopLeft(QPoint(0, 0));
    qrect.setBottomRight(QPoint(0, 0));
}
