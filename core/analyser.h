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
    Analyser(std::vector<std::vector <cv::Point> > contours, QObject *parent = 0);
    ~Analyser();
    QStandardItemModel *getDataModel();
    QStringList getClassesList() const;
    void addClass(const QString &);
    void deleteClass(int classIndex);

signals:
    void foundContourIndex(const QModelIndex &);
    void foundContourClass(const QString &);
    void statusString(const QString &);
    void classesChanged(const QStringList &);

public slots:
    void findContourHasPoint(const QPoint &pt);
    void changeClass(int classIndex);

private:
    int currentSelectedIdx;
    std::vector<std::vector <cv::Point> > m_contours;
    QStringList m_classes;
    QVector<QList<QStandardItem *> > dataVector;
    QStandardItemModel *contoursModel;
    static const QStringList headerLabels;

};
}

#endif // ANALYSER_H
