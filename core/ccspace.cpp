#include "ccspace.h"
using namespace CAIGA;

CCSpace::CCSpace(Image *img, QObject *parent) :
    QObject(parent)
{
    m_image = img;
    drawMode = -2;
    isCircle = true;
    circleRadius = 0;
    scaleValue = 0;
}

void CCSpace::cropImage()
{
    if(isCircle) {
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
    //TODO
}
