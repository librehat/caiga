#ifndef ANALYSER_H
#define ANALYSER_H

#include <QObject>
#include <opencv2/core/core.hpp>
#include <QPoint>
#include "core_lib.h"

namespace CAIGA
{
class CORE_LIB Analyser : public QObject
{
    Q_OBJECT
public:
    explicit Analyser(QObject *parent = 0);
    Analyser(std::vector<std::vector <cv::Point> > contours, QObject *parent = 0);

signals:
    void foundContourGeoInformation(const QString &);
    void statusString(const QString &);

public slots:
    void findContourHasPoint(const QPoint &pt);

private:
    std::vector<std::vector <cv::Point> > m_contours;
    void calculateAndSendContourInformation(const std::vector<cv::Point> &contour);

};
}

#endif // ANALYSER_H
