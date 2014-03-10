#ifndef PROJECT_H
#define PROJECT_H

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
    Project(QList<Image> imglist);//Image equals CAIGA::Image here.
    ~Project();
    QList<Image> imageList;//for the sake of convenience
    void setProperties(QString p);
    QString getProperties();
private:
    QString properties;
};

}

#endif // PROJECT_H
