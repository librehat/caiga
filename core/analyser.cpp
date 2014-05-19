#include "analyser.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>
#include <qmath.h>
#include <QDebug>
using namespace CAIGA;

const QStringList Analyser::headerLabels = QStringList() << "ID" << "Class" << "Area" << "Perimeter" << "Radius" << "Flattening";

Analyser::Analyser(QObject *parent) :
    QObject(parent)
{
    addClass(QString("Base"));
    contoursModel = new QStandardItemModel(0, 4, this);
    m_markerMatrix = NULL;
    scaleValue = 0;
    currentSelectedIdx = 0;
    previousClassIdx = -1;
}

Analyser::~Analyser()
{
    /* Qt takes care of its child QObject
    if (contoursModel != NULL) {
        delete contoursModel;//this will destroy all items
    }
    */
}

void Analyser::setContours(const std::vector<std::vector<cv::Point> > &contours)
{
    m_contours = contours;
    contoursModel->clear();
    areaVector.clear();
    perimeterVector.clear();
    equivalentRadiusVector.clear();
    classIdxVector.clear();
    flatteningVector.clear();
    contoursModel->setHorizontalHeaderLabels(headerLabels);
    //calculate data
    int size = static_cast<int>(m_contours.size());
    for (int id = 0; id < size; ++id) {
        QList<QStandardItem *> items;
        qreal area = calculateContourAreaByPixels(id);
        areaVector.push_back(area);
        qreal perimeter = calculatePerimeter(id);
        perimeterVector.push_back(perimeter);
        qreal radius = qSqrt((area / M_PI));//M_PI, which is accurate pi, is defined in <cmath>
        equivalentRadiusVector.push_back(radius);
        qreal flatng = calculateFlattening(id);
        flatteningVector.push_back(flatng);

        /*
         * insert number instead of QString by using setData()
         * otherwise, the sort function would become odd.
         */
        QStandardItem *idItem = new QStandardItem();
        idItem->setData(QVariant(id + 1), Qt::DisplayRole);
        QStandardItem *areaItem = new QStandardItem();
        areaItem->setData(QVariant(area), Qt::DisplayRole);
        QStandardItem *periItem = new QStandardItem();
        periItem->setData(QVariant(perimeter), Qt::DisplayRole);
        QStandardItem *radiusItem = new QStandardItem();
        radiusItem->setData(QVariant(radius), Qt::DisplayRole);
        QStandardItem *flatngItem = new QStandardItem();
        flatngItem->setData(QVariant(flatng), Qt::DisplayRole);

        items << idItem << new QStandardItem(m_classes[0]) << areaItem << periItem << radiusItem << flatngItem;
        classIdxVector.push_back(0);
        contoursModel->appendRow(items);
    }
    calculateClassValues();
}

void Analyser::setMarkers(cv::Mat * const markersMatrix)
{
    if (markersMatrix == NULL) {
        qWarning() << "Error. markersMatrix pointer is NULL!";
        return;
    }
    m_markerMatrix = markersMatrix;
}

QStandardItemModel *Analyser::getDataModel()
{
    return contoursModel;
}

void Analyser::addClass(const QString &c)
{
    m_classes.append(c);
}

void Analyser::deleteClass(int classIndex)
{
    m_classes.removeAt(classIndex);
}

void Analyser::findContourHasPoint(const QPoint &pt)
{
    int size = static_cast<int>(m_contours.size());
    if (m_markerMatrix == NULL) {
        cv::Point cvPt(pt.x(), pt.y());
        int idx = 0;
        bool found = false;
        for (; idx < size; ++idx) {
            double dist = cv::pointPolygonTest(m_contours[idx], cvPt, false);
            if (dist > 0) {
                found = true;
                break;
            }
        }
        if (found) {
            emit foundContourIndex(contoursModel->index(idx, 0));
        }
    }
    else {
        int indexVal = m_markerMatrix->at<int>(pt.y(), pt.x());//(row, col) == (y, x)
        if(indexVal > 0 && indexVal <= size) {
            emit foundContourIndex(contoursModel->index(indexVal - 1, 0));
        }
    }
}

void Analyser::onModelIndexChanged(const QModelIndex &mIdx)
{
    currentSelectedIdx = mIdx.row();
    if (classIdxVector.at(currentSelectedIdx) != previousClassIdx) {
        emit currentClassChanged(classIdxVector.at(currentSelectedIdx));
    }
    previousClassIdx = classIdxVector.at(currentSelectedIdx);
}

void Analyser::onClassChanged(const QModelIndex &mIndex, const QString classText)
{
    int idx = mIndex.row();
    int classIdx = m_classes.indexOf(classText);
    if (classIdx >= 0) {
        classIdxVector.replace(idx, classIdx);
        calculateClassValues();
    }
    else {
        m_classes << classText;
        classIdx = m_classes.indexOf(classText);
        if (classIdx >= 0) {
            classIdxVector.replace(idx, classIdx);
            calculateClassValues();
        }
        else {
            qWarning() << "Critical Error. Change class failed. Please report a bug.";
        }
    }
    onModelIndexChanged(mIndex);
}

QString Analyser::getClassValues(int classIdx) const
{
    if (classIdx < 0 || classIdx >= m_classes.size()) {
        return QString("Error. Class index is out of classes's range.");
    }
    QString info = QString("Count: %1<br />Percentage: %2%<br />Average Area: %3 μm<sup>2</sup><br />Average Perimeter: %4 μm<br />Average Radius: %5 μm<br />Average Flattening: %6<br />Average Intercept: %7 μm<br />Grain Size Level: %8").arg(classNumber.at(classIdx)).arg(grainAreaPercentageVector.at(classIdx) * 100).arg(grainAverageAreaVector.at(classIdx)).arg(grainAveragePerimeterVector.at(classIdx)).arg(grainAverageEquivalentRadiusVector.at(classIdx)).arg(grainAverageFlatteningVector.at(classIdx)).arg(grainAverageInterceptVector.at(classIdx)).arg(grainSizeLevelVector.at(classIdx));
    return info;
}

qreal Analyser::calculatePerimeter(int idx)
{
    return (cv::arcLength(m_contours[idx], true) / scaleValue);//get perimeter of closed contour
}

qreal Analyser::calculateContourAreaByGreenFormula(int idx)
{
    return (cv::contourArea(m_contours[idx], false) / scaleValue / scaleValue);
}

qreal Analyser::calculateContourAreaByPixels(int idx)
{
    if (m_markerMatrix == NULL) {
        qWarning() << "Warning. marker matrix is NULL. Using green formula to calculate the area instead.";
        return calculateContourAreaByGreenFormula(idx);
    }
    qreal pixels = 0;
    //using STL itreator to get rid of tedious double-for-loop
    for (cv::MatConstIterator_<int> it = m_markerMatrix->begin<int>(); it != m_markerMatrix->end<int>(); ++it) {
        if (((*it) - 1) == idx) {
            ++pixels;//treat every pixel as 1*1 rectangle, then pixels equals to the area
        }
    }
    return (pixels / scaleValue / scaleValue);
}

qreal Analyser::calculateFlattening(int idx)
{
    /*
     * fit a minimum ellipse
     * use the ellipse's axes to finish calculation
     * Flattening: http://en.wikipedia.org/wiki/Flattening
     */
    std::vector<cv::Point> pts = findValuePoints(idx + 1, *m_markerMatrix);
    cv::RotatedRect ellipse = cv::fitEllipse(pts);
    float h = ellipse.size.height;
    float w =ellipse.size.width;
    qreal delta = qFabs(static_cast<qreal>(h - w));
    qreal flattening = static_cast<qreal>(h > w ? delta / h : delta / w);//has to be divided by longer axis
    return flattening;
}

int Analyser::getBoundaryJointNeighbours(int row, int col)
{
    //8 neighbours
    QSet<int> neighbours;//because QSet doesn't allow duplicates, the size of neighbours is exactly the grain neighbours number
    neighbours << m_markerMatrix->at<int>(row, col - 1);//west
    neighbours << m_markerMatrix->at<int>(row, col + 1);//east
    neighbours << m_markerMatrix->at<int>(row - 1, col);//north
    neighbours << m_markerMatrix->at<int>(row + 1, col);//south
    neighbours << m_markerMatrix->at<int>(row - 1, col + 1);//northeast
    neighbours << m_markerMatrix->at<int>(row - 1, col - 1);//northwest
    neighbours << m_markerMatrix->at<int>(row + 1, col - 1);//southwest
    neighbours << m_markerMatrix->at<int>(row + 1, col + 1);//southeast
    neighbours.remove(-1);//-1 means boundary which doesn't count

    return neighbours.size();
}

void Analyser::calculateClassValues()
{
    //initialise the vector
    classNumber.clear();
    grainAreaPercentageVector.clear();
    grainAverageAreaVector.clear();
    grainAveragePerimeterVector.clear();
    grainAverageEquivalentRadiusVector.clear();
    grainAverageInterceptVector.clear();
    grainAverageFlatteningVector.clear();
    grainSizeLevelVector.clear();

    classNumber.fill(0, m_classes.size());
    grainAreaPercentageVector.fill(0, m_classes.size());
    grainAverageAreaVector.fill(0, m_classes.size());
    grainAveragePerimeterVector.fill(0, m_classes.size());
    grainAverageEquivalentRadiusVector.fill(0, m_classes.size());
    grainAverageInterceptVector.fill(0, m_classes.size());
    grainSizeLevelVector.fill(0, m_classes.size());
    grainAverageFlatteningVector.fill(0, m_classes.size());

    qreal totalArea = 0;
    //sum up area and perimeter first
    for (int idx = 0; idx < classIdxVector.size(); ++idx) {//global contour index
        int classIdx = classIdxVector.at(idx);
        classNumber[classIdx] += 1;
        grainAverageAreaVector[classIdx] += areaVector.at(idx);
        totalArea += areaVector.at(idx);
        grainAveragePerimeterVector[classIdx] += perimeterVector.at(idx);
        grainAverageEquivalentRadiusVector[classIdx] += equivalentRadiusVector.at(idx);
        grainAverageFlatteningVector[classIdx] += flatteningVector.at(idx);
    }

    //calculate the percentage and average value at last
    for (int ci = 0; ci < classNumber.size(); ++ci) {//class Index
        grainAreaPercentageVector[ci] = grainAverageAreaVector.at(ci) / totalArea;
        grainAverageAreaVector[ci] /= classNumber.at(ci);
        grainAveragePerimeterVector[ci] /= classNumber.at(ci);
        grainAverageEquivalentRadiusVector[ci] /= classNumber.at(ci);
        grainAverageFlatteningVector[ci] /= classNumber.at(ci);
    }

    //calculate the grain size using intercept method, which is noted as a standard way in GB/T 6394-2002
    int perWidth = m_markerMatrix->cols / 6; //need 5 slices vertically. this is the gap. so does the below
    int perHeight = m_markerMatrix->rows / 6;//need 5 slices horizontally.
    int bottom = m_markerMatrix->rows - 2;//margin 2 pos
    int right = m_markerMatrix->cols - 2;
    qreal totalInterceptsVertical = 0;
    qreal totalInterceptsHorizontal = 0;

    for (int s = 1; s < 6; ++s) {//at(row, col)
        int sliceV = perHeight * s;
        int sliceH = perWidth * s;
        cv::Mat vertical = m_markerMatrix->col(sliceH);//remember it's not x-y but row-col in cv::Mat
        cv::Mat horizontal = m_markerMatrix->row(sliceV);

        //calculate the intercepts on vertical line (column)
        int previousIdx = -99;//initialise it using a ridiculous number
        for (int i = 2; i < bottom; ++i) {//margin 2 pos
            int idx = vertical.at<int>(i, 0) - 1;
            if (idx != previousIdx && idx == -2) {//boundary should be counted only once to avoid tangency
                if (i == 2 || i == bottom - 1) {
                    totalInterceptsVertical += 0.5;//count the end 0.5
                }
                else if (getBoundaryJointNeighbours(i, sliceH) > 2) {//the joint of (more than) three grains should count 1.5
                    totalInterceptsVertical += 1.5;
                }
                else {
                    totalInterceptsVertical += 1;
                }
            }
            previousIdx = idx;
        }

        //calculate the intercepts on horizontal line (row)
        previousIdx = -99;
        for (int i = 2; i < right; ++i) {
            int idx = horizontal.at<int>(0, i) - 1;
            if (idx != previousIdx && idx == -2) {
                if (i == 2 || i == right - 1) {
                    totalInterceptsHorizontal += 0.5;
                }
                else if (getBoundaryJointNeighbours(sliceV, i) > 2) {
                    totalInterceptsHorizontal += 1.5;
                }
                else {
                    totalInterceptsHorizontal += 1;
                }
            }
            previousIdx = idx;
        }
    }
    totalInterceptsVertical /= 5.0;//that makes them average intercepts now
    totalInterceptsHorizontal /= 5.0;

    qreal horizontalLineLength = (right - 2) / scaleValue;//um
    qreal verticalLineLength = (bottom - 2) / scaleValue;//um
    averageInterceptsLength = ((horizontalLineLength / totalInterceptsHorizontal) + (verticalLineLength / totalInterceptsVertical)) / 2.0;

    /*
     * calculate the averageInterceptLength and grain size level by class.
     * la = l * Va. divide averageInterceptsLength by volume percentage (using area percentage here)
     */
    for (int ci = 0; ci < classNumber.size(); ++ci) {
        grainAverageInterceptVector[ci] = averageInterceptsLength * grainAreaPercentageVector.at(ci);
        //intercepts length need to be converted to minimeter (divided by 1000)
        grainSizeLevelVector[ci] = (-6.643856 * log10(grainAverageInterceptVector.at(ci) / 1000)) - 3.288 ;
    }
}

std::vector<cv::Point> Analyser::findValuePoints(int key, const cv::Mat &m)
{
    int pos = 0;
    std::vector<cv::Point> pts;
    for (cv::MatConstIterator_<int> it = m.begin<int>(); it != m.end<int>(); ++it) {
        if ((*it) == key) {
            cv::Point p(pos % m.cols, pos / m.rows);
            pts.push_back(p);
        }
        ++pos;
    }
    return pts;
}
