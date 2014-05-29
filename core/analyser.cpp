#include "analyser.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <qmath.h>
#include <QDebug>
#include <QtConcurrent>
using namespace CAIGA;

const QStringList Analyser::headerLabels = QStringList() << "ID" << "Class" << "Area" << "Perimeter" << "Diameter" << "Flattening";

Analyser::Analyser(qreal scale, cv::Mat *markers, std::vector<std::vector<cv::Point> > contours, QObject *parent) :
    QObject(parent)
{
    addClass(QString("Base"));
    contoursModel = new QStandardItemModel(0, 4, this);
    contoursModel->setHorizontalHeaderLabels(headerLabels);
    m_markerMatrix = markers;
    scaleValue = scale;
    m_contours = contours;
    currentSelectedIdx = 0;
    previousClassIdx = -1;
    interceptsNumber = 5;
    calculateByContours();
    calculatePercentage();
    calculateIntercepts();
}

void Analyser::calculateByContours()
{
    //calculate data
    updateBoundarySet();
    ClassObject base;

    //implement multi-threading
    QList<int> itemIndice;
    for (int id = 0; id < static_cast<int>(m_contours.size()); ++id) {
        itemIndice.append(id);
    }
    //lambda expression (part of C++11). please upgrade your compiler if compiler complains.
    QtConcurrent::blockingMap(itemIndice, [&base, this] (const int &id) {
        qreal area = calculateContourAreaByPixels(id);
        qreal perimeter = calculatePerimeter(id);
        qreal diameter = qSqrt(area);
        qreal flatng = calculateFlattening(id);
        bool boundary = determineIsBoundary(id);

        Object obj(boundary, area, perimeter, diameter, flatng);
        base.insert(id, obj);
    });

    for (int id = 0; id < static_cast<int>(m_contours.size()); ++id) {
        /*
         * insert number instead of QString by using setData()
         * otherwise, the sort function would become odd.
         */
        Object obj = base.ObjectAt(id);
        QList<QStandardItem *> items;
        QStandardItem *idItem = new QStandardItem();
        idItem->setData(QVariant(id + 1), Qt::DisplayRole);
        QStandardItem *areaItem = new QStandardItem();
        areaItem->setData(QVariant(obj.area()), Qt::DisplayRole);
        QStandardItem *periItem = new QStandardItem();
        periItem->setData(QVariant(obj.perimeter()), Qt::DisplayRole);
        QStandardItem *diameterItem = new QStandardItem();
        diameterItem->setData(QVariant(obj.diameter()), Qt::DisplayRole);
        QStandardItem *flatteningItem = new QStandardItem();
        flatteningItem->setData(QVariant(obj.flattening()), Qt::DisplayRole);

        items << idItem << new QStandardItem(m_classes[0]) << areaItem << periItem << diameterItem << flatteningItem;
        contoursModel->appendRow(items);
    }

    classObjMap[0] = base;
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
    //validate the point
    if (pt.x() < 0 || pt.x() >= m_markerMatrix->cols || pt.y() < 0 || pt.y() >= m_markerMatrix->rows) {
        return;
    }

    int size = static_cast<int>(m_contours.size());
    int indexVal = m_markerMatrix->at<int>(pt.y(), pt.x());//(row, col) == (y, x)
    if(indexVal > 0 && indexVal <= size) {
        emit foundContourIndex(contoursModel->index(indexVal - 1, 0));
    }
}

void Analyser::onModelIndexChanged(const QModelIndex &mIdx)
{
    currentSelectedIdx = mIdx.row();
    if (classIndexOfObject(currentSelectedIdx) != previousClassIdx) {
        emit currentClassChanged(classIndexOfObject(currentSelectedIdx));
    }
    previousClassIdx = classIndexOfObject(currentSelectedIdx);
}

void Analyser::onClassChanged(const QModelIndex &mIndex, const QString classText)
{
    int idx = mIndex.row();
    int classIdx = m_classes.indexOf(classText);
    if (classIdx >= 0) {
        int oldCIdx = classIndexOfObject(idx);
        classObjMap[classIdx].insert(idx, classObjMap[oldCIdx].takeAt(idx));
        calculatePercentage();
        calculateIntercepts();
    }
    else {
        m_classes << classText;
        classIdx = m_classes.indexOf(classText);
        if (classIdx >= 0) {
            int oldCIdx = classIndexOfObject(idx);
            classObjMap[classIdx].insert(idx, classObjMap[oldCIdx].takeAt(idx));
            calculatePercentage();
            calculateIntercepts();
        }
        else {
            qCritical() << "Critical Error. Change class failed. Please report a bug.";
        }
    }
    onModelIndexChanged(mIndex);
}

QString Analyser::getClassValues(int classIdx)
{
    if (classIdx < 0 || classIdx >= m_classes.size()) {
        return QString("Error. Class index is out of classes's range.");
    }
    QString info = QString("Count: %1<br />Percentage: %2%<br />Average Grain Area: %3 μm<sup>2</sup><br />Average Perimeter: %4 μm<br />Average Diameter: %5 μm<br />Average Flattening: %6<br />Mean Intercept: %7 μm<br />Grain Size Number (Planimetric Procedure): %8<br />Grain Size Number (Intercept Procedure): %9").arg(classObjMap[classIdx].count()).arg(classObjMap[classIdx].percentage() * 100).arg(classObjMap[classIdx].averageArea()).arg(classObjMap[classIdx].averagePerimeter()).arg(classObjMap[classIdx].averageDiameter()).arg(classObjMap[classIdx].averageFlattening()).arg(classObjMap[classIdx].averageIntercept()).arg(classObjMap[classIdx].sizeNumberByPlanimetric()).arg(classObjMap[classIdx].sizeNumberByIntercept());
    return info;
}

int Analyser::count()
{
    int c = 0;
    for (QMap<int, ClassObject>::iterator it = classObjMap.begin(); it != classObjMap.end(); ++it) {
        c += it->count();
    }
    return c;
}

qreal Analyser::getMaximumDiameter()
{
    qreal d = 0;
    for (QMap<int, ClassObject>::iterator it = classObjMap.begin(); it != classObjMap.end(); ++it) {
        for (QMap<int, Object>::const_iterator oit = it->rObjects().begin(); oit != it->rObjects().end(); ++oit) {
            d = std::max(oit->diameter(), d);
        }
    }
    return d;
}

void Analyser::onInterceptsNumberChanged(const int &num)
{
    interceptsNumber = num;
    calculateIntercepts();
    emit currentClassChanged(classIndexOfObject(currentSelectedIdx));
}

int Analyser::classIndexOfObject(int idx)
{
    for (int i = 0; i < classObjMap.size(); ++i) {
        if (classObjMap[i].contains(idx)) {
            return i;
        }
    }
    qWarning() << "Cannot find the class index of the object " << idx;
    return 0;//should not get here
}

Object Analyser::getObjectAt(int idx)
{
    int ci = classIndexOfObject(idx);
    return classObjMap[ci].ObjectAt(idx);
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
            ++pixels;//treat every pixel as 1 * 1 rectangle, then pixels equals to the area
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

bool Analyser::determineIsBoundary(int idx)
{
    return boundarySet.contains(idx);
}

void Analyser::updateBoundarySet()
{
    boundarySet.clear();
    int size = static_cast<int>(m_contours.size());
    int i = 1, j = 0;//margin 1 because the image boundary is boundary, too. :)
    for (; j < m_markerMatrix->cols; ++j) {
        int index = m_markerMatrix->at<int>(i, j) - 1;
        if (index >= 0 && index < size) {
            boundarySet << index;
        }
    }
    i = m_markerMatrix->rows - 2;//same margin here
    j = 0;
    for (; j < m_markerMatrix->cols; ++j) {
        int index = m_markerMatrix->at<int>(i, j) - 1;
        if (index >= 0 && index < size) {
            boundarySet << index;
        }
    }
    i = 0;
    j = m_markerMatrix->cols - 2;//same margin here
    for (; i < m_markerMatrix->rows; ++i) {
        int index = m_markerMatrix->at<int>(i, j) - 1;
        if (index >= 0 && index < size) {
            boundarySet << index;
        }
    }
    i = 0;
    j = 1;//same margin here
    for (; i < m_markerMatrix->rows; ++i) {
        int index = m_markerMatrix->at<int>(i, j) - 1;
        if (index >= 0 && index < size) {
            boundarySet << index;
        }
    }
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

void Analyser::calculatePercentage()
{
    qreal imageArea = m_markerMatrix->cols * m_markerMatrix->rows / scaleValue / scaleValue;
    for (QMap<int, ClassObject>::iterator it = classObjMap.begin(); it != classObjMap.end(); ++it) {
        it->setPercentage(it->totalArea() / imageArea);
    }
}

void Analyser::calculateIntercepts()
{
    //calculate intercepts
    int perWidth = m_markerMatrix->cols / (interceptsNumber + 2);
    int perHeight = m_markerMatrix->rows / (interceptsNumber + 2);
    int bottom = m_markerMatrix->rows - 2;//margin 2 pos
    int right = m_markerMatrix->cols - 2;
    qreal totalIntercepts = 0;

    for (int s = 1; s < (interceptsNumber + 1); ++s) {//at(row, col)
        int sliceV = perHeight * s;
        int sliceH = perWidth * s;
        cv::Mat vertical = m_markerMatrix->col(sliceH);//remember it's not x-y but row-col in cv::Mat
        cv::Mat horizontal = m_markerMatrix->row(sliceV);
        qreal verticalInterception = 0;
        qreal horizontalInterception = 0;

        /*
         * ASTM E112-12 13.3
         * When counting intercepts, segments at the end of a test line
         * which penetrate into a grain are scored as half intercepts.
         */

        //calculate the intercepts on vertical line (column)
        bool removeStartMargin = false;
        int lastIntercept = 0;
        qreal verticalLineLength = bottom - 4;
        int previousIdx = -99;//initialise it using a ridiculous number
        for (int i = 2; i < bottom - 2; ++i) {//margin 2 pos
            int idx = vertical.at<int>(i, 0) - 1;
            if (idx != previousIdx && idx == -2) {//boundary should be counted only once to avoid tangency
                if (i == 2 || i == bottom - 3) {//end points
                    verticalInterception += 0.5;//count the end 0.5
                }
                else if (getBoundaryJointNeighbours(i, sliceH) > 2) {//the joint of (more than) three grains should count 1.5
                    verticalInterception += 1.5;
                }
                else {
                    verticalInterception += 1;
                }
                if (!removeStartMargin) {
                    verticalLineLength -= (i - 2);
                    removeStartMargin = true;
                }
                lastIntercept = i;
            }
            previousIdx = idx;
        }
        verticalLineLength -= (bottom - 2 - lastIntercept);
        totalIntercepts += verticalLineLength / scaleValue / verticalInterception;


        //calculate the intercepts on horizontal line (row)
        qreal horizontalLineLength = right - 4;
        removeStartMargin = false;
        previousIdx = -99;
        for (int i = 2; i < right - 2; ++i) {
            int idx = horizontal.at<int>(0, i) - 1;
            if (idx != previousIdx && idx == -2) {
                if (i == 2 || i == right - 3) {
                    horizontalInterception += 0.5;
                }
                else if (getBoundaryJointNeighbours(sliceV, i) > 2) {
                    horizontalInterception += 1.5;
                }
                else {
                    horizontalInterception += 1;
                }
                if (!removeStartMargin) {
                    horizontalLineLength -= (i - 2);
                    removeStartMargin = true;
                }
                lastIntercept = i;
            }
            previousIdx = idx;
        }
        horizontalLineLength -= (right - 2 - lastIntercept);
        totalIntercepts += horizontalLineLength / scaleValue / horizontalInterception;
    }

    averageIntercept = totalIntercepts / (interceptsNumber * 2) ;

    /*
     * calculate the averageInterceptLength by class.
     * la = l * Va. divide averageInterceptsLength by volume percentage (using area percentage here)
     */
    for (QMap<int, ClassObject>::iterator it = classObjMap.begin(); it != classObjMap.end(); ++it) {
        it->setAverageIntercept(averageIntercept * it->percentage());
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
