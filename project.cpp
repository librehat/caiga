#include <QSqlError>
#include <QMessageBox>
#include "project.h"
using namespace CAIGA;

Project::Project()
{
    m_isSaved = false;
}
/*
Project::Project(QList<Image> imglist)
{
    m_isSaved = false;
    imageList = imglist;
}
*/
Project::Project(const QString &dbFile)
{
    setDataBase(dbFile);
}

Project::~Project()
{}

QString Project::getProperties()
{
    return properties;
}

void Project::setProperties(QString p)
{
    properties = p;
}

bool Project::setDataBase(const QString &dbFile)
{
    m_isSaved = true;
    if (db.isOpen()) {
        db.close();
    }
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbFile);
    if(!db.open()) {
        QMessageBox sqlE;
        sqlE.setWindowTitle("SQL Error");
        sqlE.setIcon(QMessageBox::Critical);
        sqlE.setText(db.lastError().text());
        sqlE.setStandardButtons(QMessageBox::Ok);
        sqlE.exec();
        return false;
    }
    return true;
}

bool Project::save()
{
    //TODO
    m_isSaved = true;
    return true;
}

bool Project::isSaved()
{
    return m_isSaved;
}

void Project::close()
{
    db.close();
}
