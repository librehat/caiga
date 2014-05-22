#ifndef ANALYSER_H
#define ANALYSER_H

#include <QObject>
#include <QPoint>
#include <QStandardItemModel>
#include <QStringList>
#include <QStringListModel>
#include <QList>
#include <opencv2/core/core.hpp>
#include "classobject.h"
#include "core_lib.h"

namespace CAIGA
{
class CORE_LIB Analyser : public QObject
{
    Q_OBJECT
public:
    explicit Analyser(QObject *parent = 0);
    ~Analyser() {}
    void setContours(const std::vector<std::vector <cv::Point> > &contours);
    void setMarkers(cv::Mat * const markersMatrix);//setMarkers if the result is obtained by watershed, if markersMatrix is set, then contours won't be used when querying the contour id
    QStandardItemModel *getDataModel();
    QStringList* getClassesList() { return &m_classes; }
    void addClass(const QString &);
    void deleteClass(int classIndex);
    inline void setScaleValue(qreal s) { scaleValue = s; }
    QString getClassValues(int classIdx);
    int count();
    inline int classCount() const { return m_classes.size(); }
    qreal getMaximumDiameter();

    //below are functions that get the value at a specified index in type of QString
    inline QString getClassNameAt(int idx) { return m_classes.at(classIndexOfObject(idx)); }
    inline QString getAreaAt(int idx) { return QString::number(getObjectAt(idx).area()); }
    inline QString getPerimeterAt(int idx) { return QString::number(getObjectAt(idx).perimeter()); }
    inline QString getDiameterAt(int idx) { return QString::number(getObjectAt(idx).diameter()); }
    inline QString getFlatteningAt(int idx) { return QString::number(getObjectAt(idx).flattening()); }
    //below are based on class index
    inline QString getCountOfClass(int i) { return QString::number(classObjMap[i].count()); }
    inline QString getAvgPercentOfClass(int i) { return QString::number(classObjMap[i].percentage() * 100); }
    inline QString getAvgAreaOfClass(int i) { return QString::number(classObjMap[i].averageArea()); }
    inline QString getAvgPerimeterOfClass(int i) { return QString::number(classObjMap[i].averagePerimeter()); }
    inline QString getAvgDiameterOfClass(int i) { return QString::number(classObjMap[i].averageDiameter()); }
    inline QString getAvgInterLengthOfClass(int i) { return QString::number(classObjMap[i].averageIntercept()); }
    inline QString getAvgFlatteningOfClass(int i) { return QString::number(classObjMap[i].averageFlattening()); }
    inline QString getSizeLevelOfClass(int i) { return QString::number(classObjMap[i].sizeLevel()); }

    QMap<int, ClassObject> classObjMap;

    //this static member function "find" the locations whose value equals to the given key
    //the Mat must have value which is of integer type
    static std::vector<cv::Point> findValuePoints(int key, const cv::Mat &m);

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
    QMap<int, bool> boundaryMap;//store all boundary objects' indice

    int classIndexOfObject(int idx);//get the class index of the idx-th object
    Object getObjectAt(int idx);
    QStringList m_classes;
    qreal averageInterceptsLength;

    QStandardItemModel *contoursModel;
    static const QStringList headerLabels;

    qreal calculatePerimeter(int idx);
    qreal calculateContourAreaByGreenFormula(int idx);
    qreal calculateContourAreaByPixels(int idx);
    qreal calculateFlattening(int idx);
    bool determineIsBoundary(int idx);
    void updateBoundaryMap();
    int getBoundaryJointNeighbours(int row, int col);//leave at least 1 position from Mat edge

    /*
     * calculate all sorts of information class by class
     * these functions would clear the previous values
     */
    void calculateClassValues();
};
}

#endif // ANALYSER_H
