#include "analyser.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <QDebug>
using namespace CAIGA;

const QStringList Analyser::headerLabels = QStringList() << "ID" << "Area" << "Perimeter" << "Class";

Analyser::Analyser(QObject *parent) :
    QObject(parent)
{
    addClass(QString("Base"));
    contoursModel = new QStandardItemModel(0, 4, this);
    m_markersMatrix = NULL;
    scaleValue = 0;
}

Analyser::~Analyser()
{
    if (contoursModel != NULL) {
        delete contoursModel;//this will destroy all items
    }
}

void Analyser::setContours(const std::vector<std::vector<cv::Point> > &contours)
{
    m_contours = contours;
    contoursModel->clear();
    areaVector.clear();
    perimeterVector.clear();
    classIdxVector.clear();
    contoursModel->setHorizontalHeaderLabels(headerLabels);
    //calculate data
    int size = static_cast<int>(m_contours.size());
    for (int id = 0; id < size; ++id) {
        QList<QStandardItem *> items;
        qreal area = calculateContourAreaByPixels(id);
        areaVector.push_back(area);
        qreal perimeter = calculatePerimeter(id);
        perimeterVector.push_back(perimeter);
        QStandardItem *idItem = new QStandardItem();
        idItem->setData(QVariant(id + 1), Qt::DisplayRole);
        QStandardItem *areaItem = new QStandardItem();
        areaItem->setData(QVariant(area), Qt::DisplayRole);
        QStandardItem *periItem = new QStandardItem();
        periItem->setData(QVariant(perimeter), Qt::DisplayRole);
        items << idItem << areaItem << periItem << new QStandardItem(m_classes[0]);
        classIdxVector.push_back(0);
        contoursModel->appendRow(items);
    }
}

void Analyser::setMarkers(cv::Mat * const markersMatrix)
{
    if (markersMatrix == NULL) {
        qWarning() << "Error. markersMatrix pointer is NULL!";
        return;
    }
    m_markersMatrix = markersMatrix;
}

QStandardItemModel *Analyser::getDataModel()
{
    return contoursModel;
}

QStringList Analyser::getClassesList() const
{
    return m_classes;
}

void Analyser::addClass(const QString &c)
{
    m_classes.append(c);
}

void Analyser::deleteClass(int classIndex)
{
    m_classes.removeAt(classIndex);
}

void Analyser::changeClass(int classIndex)
{
    if (classIndex >= m_classes.size() || classIndex < 0) {
        qWarning() << "Error. New class index is out of classes's range.";
        return;
    }
    classIdxVector.replace(currentSelectedIdx, classIndex);
    contoursModel->setData(contoursModel->index(currentSelectedIdx, 3), QVariant(m_classes[classIndex]));
}

void Analyser::findContourHasPoint(const QPoint &pt)
{
    int size = static_cast<int>(m_contours.size());
    if (m_markersMatrix == NULL) {
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
            setCurrentSelectedIdx(idx);
            emit foundContourIndex(contoursModel->index(currentSelectedIdx, 0));
        }
        else {
            emit statusString("Error. No contour contains the point.");
        }
    }
    else {
        int indexVal = m_markersMatrix->at<int>(pt.y(), pt.x());//(row, col) == (y, x)
        if(indexVal > 0 && indexVal <= size) {
            setCurrentSelectedIdx(indexVal - 1);
            emit foundContourIndex(contoursModel->index(currentSelectedIdx, 0));
        }
        else {
            emit statusString("Error. No contour contains the point.");
        }
    }
}

void Analyser::setCurrentSelectedIdx(int i)
{
    currentSelectedIdx = i;
    emit foundContourClass(m_classes.at(classIdxVector[currentSelectedIdx]));
    emit statusString("Contour Found");
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
    if (m_markersMatrix == NULL) {
        qWarning() << "Warning. markers matrix is NULL. Using green formula to calculate the area instead.";
        return calculateContourAreaByGreenFormula(idx);
    }
    qreal pixels = 0;
    for (int i = 0; i < m_markersMatrix->rows; ++i) {
        for (int j = 0; j < m_markersMatrix->cols; ++j) {
            int idxVal = m_markersMatrix->at<int>(i, j) - 1;
            if (idxVal == idx) {
                ++pixels;//treat every pixel as 1*1 rectangle, then pixels equals to the area
            }
        }
    }
    return (pixels / scaleValue / scaleValue);
}
