/*
 * The file type of CAIGA Project is SQLite.
 * Read/Write CAIGA Project file by using QtSQL.
 *
 * CAIGA::Project contains:
 * - Project Name
 * - Project Properties (name, size, created date, modified date, etc)
 * - Analysis Type (Grain Size Rate, Ductile Iron Spheroidal Rate, Pearlite Nodulizing Rate, etc)
 * - Relevant Analyser Object
 * - A List of CAIGA::Image
 */

#ifndef PROJECT_H
#define PROJECT_H

#include "core_lib.h"
#include <QSqlDatabase>
#include "image.h"

/*
 * define inside CAIGA namespace
 * in case of global conflict
 */
namespace CAIGA
{
class CORE_LIB Project
{
public:
    Project();
    //Project(QList<Image> imglist);//Image equals CAIGA::Image here.
    Project(const QString &dbFile);
    ~Project();
    //QList<Image> imageList;//for the sake of convenience
    void setProperties(QString p);
    QString getProperties();//"Project Properties"
    bool setDataBase(const QString &dbFile);
    bool save();//Save all data into sqlite file.
    bool isSaved();
    void close();//Free allocated memory and resources. Close opened database connection.
private:
    QString properties;
    QSqlDatabase db;
    bool m_isSaved;
};

}

#endif // PROJECT_H
