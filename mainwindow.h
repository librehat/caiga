/*
 * MainWindow
 *
 * Don't define real data classes in this file.
 *
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QStringList>
#include <QStringListModel>
#include <QAbstractItemModel>
#include <QImage>
#include <QPixmap>
#include <QSettings>

#include "caigaglobal.h"
#include "project.h"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void configReadFinished(int,int,int,bool,int);

private slots:
    void openProjectDialog();
    void saveProjectDialog();
    void addDiskFileDialog();
    void createCameraDialog();
    void createOptionsDialog();
    void exportImgDialog();
    void exportProDialog();
    void projectPropertiesDialog();
    void aboutQtDialog();
    void aboutCAIGADialog();

    void setActivateImage(QModelIndex);
    void updateOptions(int,int,int,bool,int);

private:
    Ui::MainWindow *ui;

    QSettings settings;
    void readConfig();

    //CAIGA::Project caiga_Project;
    //Pre Alpha Dev: Focus on Single Image!
    CAIGA::Image proImg;
    /*
     * Use the abstract interface to the model, which ensures
     * that the code still works despite what the model type is.
     */
    //QAbstractItemModel *imgNameModel;
    QStringListModel *imgNameModel;
    QStringList imgNameList;


    //Text in About Dialog
    static QString aboutText;
};
#endif // MAINWINDOW_H
