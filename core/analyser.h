#ifndef ANALYSER_H
#define ANALYSER_H

#include <QObject>
#include <QPoint>
#include <QStandardItemModel>
#include <QStringList>
#include <QStringListModel>
#include <QList>
#include <opencv2/core/core.hpp>
#include "core_lib.h"

namespace CAIGA
{
class CORE_LIB Analyser : public QObject
{
    Q_OBJECT
public:
    explicit Analyser(QObject *parent = 0);
    ~Analyser();
    void setContours(const std::vector<std::vector <cv::Point> > &contours);
    void setMarkers(cv::Mat * const markersMatrix);//setMarkers if the result is obtained by watershed, if markersMatrix is set, then contours won't be used when querying the contour id
    QStandardItemModel *getDataModel();
    QStringList getClassesList() const;
    void addClass(const QString &);
    void deleteClass(int classIndex);
    inline void setScaleValue(qreal s) { scaleValue = s; }

signals:
    void foundContourIndex(const QModelIndex &);
    void foundContourClass(const QString &);
    void statusString(const QString &);
    void classesChanged(const QStringList &);

public slots:
    void findContourHasPoint(const QPoint &pt);
    void changeClass(int classIndex);

private:
    qreal scaleValue;//pixel / um
    int currentSelectedIdx;
    cv::Mat *m_markersMatrix;
    std::vector<std::vector <cv::Point> > m_contours;
    QStringList m_classes;
    QVector<QList<QStandardItem *> > dataVector;
    QStandardItemModel *contoursModel;
    static const QStringList headerLabels;

    qreal calculatePerimeter(int idx);
    qreal calculateContourAreaByGreenFormula(int idx);
    qreal calculateContourAreaByPixels(int idx);

};
}

#endif // ANALYSER_H
