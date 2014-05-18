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
    QStringList* getClassesList() { return &m_classes; }
    inline QVector<qreal> *getEquivalentRadii() { return &equivalentRadiusVector; }
    inline QVector<int> *getClassIndice() { return &classIdxVector; }
    void addClass(const QString &);
    void deleteClass(int classIndex);
    inline void setScaleValue(qreal s) { scaleValue = s; }
    QString getClassValues(int classIdx) const;
    inline int count() const { return classIdxVector.size(); }
    inline int classCount() const { return m_classes.size(); }

    //below are functions that get the value at a specified index in type of QString
    inline QString getClassNameAt(int idx) { return m_classes.at(classIdxVector.at(idx)); }
    inline QString getAreaAt(int idx) { return QString::number(areaVector.at(idx)); }
    inline QString getPerimeterAt(int idx) { return QString::number(perimeterVector.at(idx)); }
    inline QString getRadiusAt(int idx) { return QString::number(equivalentRadiusVector.at(idx)); }

signals:
    void foundContourIndex(const QModelIndex &);
    void currentClassChanged(int classIdx);

public slots:
    void findContourHasPoint(const QPoint &pt);
    void onModelIndexChanged(const QModelIndex &mIdx);
    void onClassChanged(const QModelIndex &mIndex, const QString classText);

private:
    qreal scaleValue;//pixel / um
    int currentSelectedIdx;
    int previousClassIdx;
    cv::Mat *m_markerMatrix;
    std::vector<std::vector <cv::Point> > m_contours;

    QVector<qreal> areaVector;
    QVector<qreal> perimeterVector;
    QVector<qreal> equivalentRadiusVector;//the radius of the circle with the same area
    QVector<int> classIdxVector;//classIdx has the same index of m_classes

    /*
     * calculated information are stored by classIdx
     */
    QStringList m_classes;
    QVector<int> classNumber;
    QVector<qreal> grainAreaPercentageVector;
    QVector<qreal> grainAverageAreaVector;
    QVector<qreal> grainAveragePerimeterVector;
    QVector<qreal> grainAverageEquivalentRadiusVector;
    QVector<qreal> grainAverageInterceptVector;// = Total Length / Total Intercept
    QVector<qreal> grainSizeLevelVector;//G, calculated by intercept method

    qreal averageInterceptsLength;

    QStandardItemModel *contoursModel;
    static const QStringList headerLabels;

    qreal calculatePerimeter(int idx);
    qreal calculateContourAreaByGreenFormula(int idx);
    qreal calculateContourAreaByPixels(int idx);

    int getBoundaryJointNeighbours(int row, int col);//leave at least 1 position from Mat edge

    /*
     * calculate all sorts of information class by class
     * these functions would clear the previous values
     */
    void calculateClassValues();

};
}

#endif // ANALYSER_H
