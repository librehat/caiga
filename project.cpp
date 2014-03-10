#include "project.h"
using namespace CAIGA;

Project::Project()
{}

Project::Project(QList<Image> imglist)
{
    imageList = imglist;
}

Project::~Project()
{
    //NOTHING
}

QString Project::getProperties()
{
    return properties;
}

void Project::setProperties(QString p)
{
    properties = p;
}
