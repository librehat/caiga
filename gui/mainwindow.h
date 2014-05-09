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
#include <QImage>
#include <QPixmap>
#include <QSettings>
#include <QDir>
#include "image.h"
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
    void configReadFinished(int, int, int, const QString &);
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
    void onInvertGrayscaleButtonClicked();
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
    void addDiskFileDialog();
    void createCameraDialog();
    void createOptionsDialog();
    void exportImgDialog();
    void aboutQtDialog();
    void aboutCAIGADialog();
    void onMessagesArrived(const QString &);
    void updateOptions(int, int, int, const QString &);

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
    void setCurrentDirbyFile(QString &);
    CAIGA::Image cgimg;
    CAIGA::WorkSpace preWorkSpace;
    CAIGA::WorkSpace previewSpace;

    //Text in About Dialog
    const static QString aboutText;

protected:
     void closeEvent(QCloseEvent *event);
};
#endif // MAINWINDOW_H
