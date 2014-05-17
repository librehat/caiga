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
    isCircle = img == NULL ? NULL : &img->m_isCircle;
    scaleValue = img == NULL ? NULL : &img->m_scale;
}

void CCSpace::cropImage()
{
    if (m_image == NULL) {
        qWarning() << "Abort. Image pointer is NULL.";
        return;
    }
    if(getIsCircle()) {
        cv::Point tl(circleCentre.x() - circleRadius, circleCentre.y() - circleRadius);
        cv::Point br(circleCentre.x() + circleRadius, circleCentre.y() + circleRadius);
        cv::Rect enclosingRect(tl, br);
        m_image->croppedImage = m_image->rawImage(enclosingRect).clone();
        m_image->croppedCircularImage = Image::makeInCircle(m_image->croppedImage);
    }
    else {
        cv::Point tl(qrect.topLeft().x(), qrect.topLeft().y());
        cv::Point br(qrect.bottomRight().x(), qrect.bottomRight().y());
        cv::Rect rect(tl, br);
        m_image->croppedImage = m_image->rawImage(rect).clone();
    }
}

void CCSpace::reset()
{
    circleCentre.setX(0);
    circleCentre.setY(0);
    circleRadius = 0;
    qrect.setTopLeft(QPoint(0, 0));
    qrect.setBottomRight(QPoint(0, 0));
}
