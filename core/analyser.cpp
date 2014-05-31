#include "analyser.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <qmath.h>
#include <functional>
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
    QtConcurrent::blockingMap(itemIndice, [&] (const int &id) {
        qreal area = calculateContourAreaByPixels(id);
        qreal perimeter = calculatePerimeter(id);
        qreal diameter = qSqrt(area);
        qreal flatng = calculateFlattening(id);
        Object::POSITION boundary = determineIsBoundary(id);

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
    QString info = QString("Count: %1<br />Total Area: %2μm<sup>2</sup><br />Percentage: %3%<br />Average Grain Area: %4 μm<sup>2</sup><br />Average Perimeter: %5 μm<br />Average Diameter: %6 μm<br />Average Flattening: %7<br />Mean Intercept: %8 μm<br />Grain Size Number (Planimetric Procedure): %9<br />Grain Size Number (Intercept Procedure): %10").arg(classObjMap[classIdx].count()).arg(classObjMap[classIdx].totalArea()).arg(classObjMap[classIdx].percentage() * 100).arg(classObjMap[classIdx].averageArea()).arg(classObjMap[classIdx].averagePerimeter()).arg(classObjMap[classIdx].averageDiameter()).arg(classObjMap[classIdx].averageFlattening()).arg(classObjMap[classIdx].averageIntercept()).arg(classObjMap[classIdx].sizeNumberByPlanimetric()).arg(classObjMap[classIdx].sizeNumberByIntercept());
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

qreal Analyser::calculateContourAreaByPixels(int idx)
{
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
    qreal h = ellipse.size.height / 2;
    qreal w = ellipse.size.width / 2;
    qreal flattening = qFabs(h - w) / std::max(h, w);//has to be divided by longer axis
    return flattening;
}

Object::POSITION Analyser::determineIsBoundary(int idx)
{
    if (boundarySet.contains(idx)) {
        return Object::INTERCEPTED;
    }
    else if (cornerSet.contains(idx)) {
        return Object::CORNER;
    }
    else {
        return Object::INSIDE;
    }
}

void Analyser::updateBoundarySet()
{
    boundarySet.clear();
    cornerSet.clear();
   cv::Point topLeft(1, 1), topRight (m_markerMatrix->cols - 2, 1), bottomLeft(1, m_markerMatrix->rows - 2), bottomRight(m_markerMatrix->cols - 2, m_markerMatrix->rows - 2);
    QVector<QVector<cv::Point> > boundaryArray;
    QVector<cv::Point> top, left, bottom, right;
    top << topLeft << topRight;
    left << topLeft <<bottomLeft;
    bottom << bottomLeft << bottomRight;
    right << topRight << bottomRight;
    boundaryArray << top << left << bottom << right;

    QtConcurrent::blockingMap(boundaryArray.begin(), boundaryArray.end(), [this] (QVector<cv::Point> line) {
        cv::LineIterator it(*m_markerMatrix, line[0], line[1]);
        for (int i = 0; i < it.count; ++i, ++it) {
            boundarySet << m_markerMatrix->at<int>(it.pos()) - 1;
        }
    });

    cornerSet << m_markerMatrix->at<int>(topLeft) - 1
              << m_markerMatrix->at<int>(topRight) - 1
              << m_markerMatrix->at<int>(bottomLeft) - 1
              << m_markerMatrix->at<int>(bottomRight) - 1;
    for (QSet<int>::iterator it = cornerSet.begin(); it != cornerSet.end(); ++it) {
        boundarySet.remove(*it);
    }
}

int Analyser::getBoundaryJointNeighbours(const cv::Point &pos)
{
    //8 neighbours
    QSet<int> neighbours;//because QSet doesn't allow duplicates, the size of neighbours is exactly the grain neighbours number
    neighbours << m_markerMatrix->at<int>(pos - cv::Point(1, 0));//west
    neighbours << m_markerMatrix->at<int>(pos + cv::Point(1, 0));//east
    neighbours << m_markerMatrix->at<int>(pos - cv::Point(0, 1));//north
    neighbours << m_markerMatrix->at<int>(pos + cv::Point(0, 1));//south
    neighbours << m_markerMatrix->at<int>(pos + cv::Point(1, -1));//northeast
    neighbours << m_markerMatrix->at<int>(pos - cv::Point(1, 1));//northwest
    neighbours << m_markerMatrix->at<int>(pos - cv::Point(1, -1));//southwest
    neighbours << m_markerMatrix->at<int>(pos + cv::Point(1, 1));//southeast
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
    qreal totalInterception = 0, totalTestLineLength = 0;
    cv::Point topLeft(1, 1), topRight (m_markerMatrix->cols - 2, 1), bottomLeft(1, m_markerMatrix->rows - 2), bottomRight(m_markerMatrix->cols - 2, m_markerMatrix->rows - 2);

    QVector<QVector<cv::Point> > testLineArray;
    //left, bottom, slash (/), anti-slash(\)
    QVector<cv::Point> testLineL, testLineB, testLineS, testLineA;
    //ASTM E112-13 13.4 noted that test lines shall not radiate from common points
    testLineL << topLeft + cv::Point(0, 1) << bottomLeft - cv::Point(0, 1);
    testLineB << bottomLeft + cv::Point(1, 0) << bottomRight - cv::Point(1, 0);
    //ASTM E112-13 13.4 suggests four or more test lines of **different** direction for equiaxed structures
    //testLineT << topLeft + cv::Point(1, 0) << topRight - cv::Point (1, 0);
    //testLineR << topRight + cv::Point (0, 1) << bottomRight - cv::Point(0, 1);
    testLineS << topRight + cv::Point(-1, 1) << bottomLeft - cv::Point(-1, 1);
    testLineA << topLeft + cv::Point(1, 1) << bottomRight - cv::Point(1, 1);

    testLineArray << testLineL << testLineB << testLineS << testLineA;

    //implement multi-threading
    QtConcurrent::blockingMap(testLineArray.begin(), testLineArray.end(), [&] (QVector<cv::Point> line) {
        cv::LineIterator it(*m_markerMatrix, line[0], line[1]);
        int previousIndex = -9;
        for (int i = 0; i < it.count; ++i, ++it) {
            int index = m_markerMatrix->at<int>(it.pos());
            if (index == -1 && index != previousIndex) {//boundary
                if (i == 0 || i == it.count - 1) {
                    totalInterception += 0.5;
                }
                else if (getBoundaryJointNeighbours(it.pos()) > 2) {
                    totalInterception += 1.5;
                }
                else {
                    totalInterception += 1;
                }
            }
            previousIndex = index;
        }
        //add this line's length
        totalTestLineLength += distanceBetweenPoints(line[0], line[1]);
    });

    averageIntercept = totalTestLineLength / totalInterception / scaleValue;

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

qreal Analyser::distanceBetweenPoints(const cv::Point &pt1, const cv::Point &pt2)
{
    cv::Point d = pt2 - pt1;
    return qSqrt(static_cast<qreal>(d.x * d.x) + static_cast<qreal>(d.y * d.y));
}
