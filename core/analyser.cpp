#include "analyser.h"
#include <opencv2/imgproc/imgproc.hpp>
using namespace CAIGA;

Analyser::Analyser(QObject *parent) :
    QObject(parent)
{
}

Analyser::Analyser(std::vector<std::vector<cv::Point> > contours, QObject *parent) :
    QObject(parent)
{
    m_contours = contours;
}

void Analyser::findContourHasPoint(const QPoint &pt)
{
    cv::Point cvPt(pt.x(), pt.y());
    int size = static_cast<int>(m_contours.size());
    int idx = 0;
    bool found = false;
    for (; idx < size; ++idx) {
        int dist = cv::pointPolygonTest(m_contours[idx], cvPt, false);
        if (dist > 0) {
            found = true;
            break;
        }
    }
    if (found) {
        calculateAndSendContourInformation(m_contours[idx]);
        emit statusString(QString("Contour Found. ID ") + QString::number(idx));
    }
    else {
        emit statusString("Error. No contour contains the point.");
    }
}

void Analyser::calculateAndSendContourInformation(const std::vector<cv::Point> &contour)
{
    //contourArea uses Green formula
    double area = cv::contourArea(contour, false);
    double perimeter = cv::arcLength(contour, true);//get perimeter of closed contour
    QString info = QString("Area: ") + QString::number(area) + QString("\nPerimeter: ") + QString::number(perimeter);
    emit foundContourGeoInformation(info);
}
