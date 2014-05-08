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
    void onGaugeLineFinished(qreal);
    void onCCButtonBoxClicked(QAbstractButton *);
    void onMouseNormalArrow();
    void onMouseWhitePencil();
    void onMouseBlackPencil();
    void onPencilDrawFinished(QVector<QPoint>);
    void onMouseWhiteEraser();
    void onMouseBlackEraser();
    void onEraserDrawFinished(QVector<QPoint>);
    void onHistEqualiseButtonClicked();
    void onBoxFilterButtonClicked();
    void onBoxFilterParametersChanged(int, double, double, bool);
    void onAdaptiveBilateralFilterButtonClicked();
    void onAdaptiveBilateralFilterParametersChanged(int, double, double, bool);
    void onGaussianBinaryzationButtonClicked();
    void onGaussianBinaryzationParametersChanged(int, double, double, bool);
    void onMedianBinaryzationButtonClicked();
    void onMedianBinaryzationParametersChanged(int, double, double, bool);
    void onFloodFillButtonClicked();
    void onFloodFillParametersChanged(int, double, double, bool);
    void onFloodFillMouseClicked(QPoint);
    void onFloodFillAccepted();
    void onFloodFillRejected();
    void onCannyButtonClicked();
    void onCannyParametersChanged(int, double, double, bool);
    void onPreParametersAccepted();
    void onPreParametersRejected();
    void onContoursButtonClicked();
    void onPreviewWorkFinished();
    void onPreProcessWorkFinished();
    void onPreProcessButtonBoxClicked(QAbstractButton *);
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
    QMenu *mouseBehaviourMenu;
    ParametersDialog boxFilterDlg;
    ParametersDialog adaptiveBilateralDlg;
    ParametersDialog gaussianBinaryDlg;
    ParametersDialog medianBinaryDlg;
    ParametersDialog cannyDlg;
    ParametersDialog floodFillDlg;

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
    CAIGA::WorkSpace preWorkSpace;
    CAIGA::WorkSpace previewSpace;

    //Text in About Dialog
    const static QString aboutText;

protected:
     void closeEvent(QCloseEvent *event);
};
#endif // MAINWINDOW_H
