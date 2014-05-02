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
#include <QTimer>
#include <QDir>
#include "image.h"
#include "project.h"
#include "workerthread.h"
#include "workspace.h"
#include "parametersdialog.h"

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
    void configReadFinished(int, int, int, bool, int, const QString &);
    void binaryParametersChanged();
    void messageArrived(const QString &);//to display on statusbar

private slots:
    void ccModeChanged(int);
    void onCCButtonBoxClicked(QAbstractButton *);
    void onHistEqualiseButtonClicked();
    void onAdaptiveBilateralFilterButtonClicked();
    void onAdaptiveBilateralFilterParametersAccepted(int, double, double);
    void onMedianBlurButtonClicked();
    void onPreProcessWorkFinished();
    void onPreProcessButtonBoxClicked(QAbstractButton *);
    void onPreProcessUndoClicked();
    void onPreProcessRedoClicked();
    void onBinaryMethodChanged(int);
    void onBinarySizeChanged(int);
    void onBinaryParametersChanged();
    void onBinaryWorkFinished();
    void onBinaryInterDrawerClicked(QPoint);
    void onBinaryButtonBoxClicked(QAbstractButton *);
    void onContourRefreshButtonPressed();
    void onContourWorkFinished();
    void onCurrentTabChanged(int);
    void newProject();
    void openProjectDialog();
    void saveProject();
    void saveProjectAsDialog();
    void closeProject();
    void addDiskFileDialog();
    void createCameraDialog();
    void createOptionsDialog();
    void exportImgDialog();
    void exportProDialog();
    void projectPropertiesDialog();
    void aboutQtDialog();
    void aboutCAIGADialog();
    void onMessagesArrived(const QString &);

    void setActivateImage(QModelIndex);
    void updateOptions(int, int, int, bool, int, const QString &);

private:
    Ui::MainWindow *ui;
    ParametersDialog *adaptiveBilateralDlg;

    QSettings settings;
    void readConfig();
    QTimer autoSaveTimer;

    CAIGA::Project project;
    bool projectUnsaved;

    int unSavedProject();//2 for cancel, 0 for no, 1 for yes, -1 if there is no unsaved project
    void setWidgetsEnabled(bool);
    void setCurrentDirbyFile(QString &);
    /*
     * Use the abstract interface to the model, which ensures
     * that the code still works despite what the model type is.
     */
    CAIGA::Image cgimg;
    QAbstractItemModel *imgNameModel;
    QStringList imgNameList;
    CAIGA::WorkerThread worker;
    CAIGA::WorkSpace preWorkSpace;

    //Text in About Dialog
    const static QString aboutText;

protected:
     void closeEvent(QCloseEvent *event);
};
#endif // MAINWINDOW_H
