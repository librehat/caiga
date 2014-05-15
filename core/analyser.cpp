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
    dataVector.clear();
    contoursModel->setHorizontalHeaderLabels(headerLabels);
    //calculate data
    int size = static_cast<int>(m_contours.size());
    for (int id = 0; id < size; ++id) {
        QList<QStandardItem *> items;
        double area = cv::contourArea(m_contours[id], false);
        double perimeter = cv::arcLength(m_contours[id], true);//get perimeter of closed contour
        QStandardItem *idItem = new QStandardItem();
        idItem->setData(QVariant(id + 1), Qt::DisplayRole);
        QStandardItem *areaItem = new QStandardItem();
        areaItem->setData(QVariant(area), Qt::DisplayRole);
        QStandardItem *periItem = new QStandardItem();
        periItem->setData(QVariant(perimeter), Qt::DisplayRole);
        items << idItem << areaItem << periItem << new QStandardItem(m_classes[0]);
        dataVector << items;
        contoursModel->appendRow(items);
    }
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
    dataVector[currentSelectedIdx][3]->setData(QVariant(m_classes[classIndex]), Qt::DisplayRole);
}

void Analyser::findContourHasPoint(const QPoint &pt)
{
    cv::Point cvPt(pt.x(), pt.y());
    int size = static_cast<int>(m_contours.size());
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
        currentSelectedIdx = idx;
        emit foundContourIndex(contoursModel->index(currentSelectedIdx, 0));
        emit foundContourClass(dataVector[currentSelectedIdx][3]->data(Qt::DisplayRole).toString());
        emit statusString("Contour Found");
    }
    else {
        emit statusString("Error. No contour contains the point.");
    }
}
