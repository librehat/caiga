#ifndef PROJECT_H
#define PROJECT_H

#include <QSqlDatabase>
#include "caigaglobal.h"
#include "image.h"

/*
 * define inside CAIGA namespace
 * in case of global conflict
 */
namespace CAIGA
{
class Project
{
public:
    Project();
    //Project(QList<Image> imglist);//Image equals CAIGA::Image here.
    Project(const QString &dbFile);
    ~Project();
    //QList<Image> imageList;//for the sake of convenience
    void setProperties(QString p);
    QString getProperties();
    bool setDataBase(const QString &dbFile);
    bool save();
    bool isSaved();
    void close();//Free allocated memory and resources. Close opened database connection.
private:
    QString properties;
    QSqlDatabase db;
    bool m_isSaved;
};

}

#endif // PROJECT_H
