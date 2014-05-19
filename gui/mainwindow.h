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
#include "ccspace.h"
#include "workspace.h"
#include "parametersdialog.h"
#include "watershedmarkerdialog.h"
#include "analyser.h"
#include "analysisitemdelegate.h"
#include "reporter.h"

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
    //crop and calibre
    void ccModeChanged(int);
    void onGaugeLineFinished(qreal);
    void onCCLoadMacroButtonClicked();
    void onCCSaveMacroButtonClicked();
    void onCCButtonBoxClicked(QAbstractButton *);

    //preProcess
    void onMouseNormalArrow();
    void onMouseWhitePencil();
    void onMouseBlackPencil();
    void onPencilDrawFinished(const QVector<QPoint> &);
    void onMouseWhiteEraser();
    void onMouseBlackEraser();
    void onEraserDrawFinished(const QVector<QPoint> &);
    void onInvertGrayscaleButtonClicked();
    void onHistEqualiseButtonClicked();
    void onGradientButtonClicked();
    void onGradientParamatersChanged(int, double, double, bool);
    void onBoxFilterButtonClicked();
    void onBoxFilterParametersChanged(int, double, double, bool);
    void onAdaptiveBilateralFilterButtonClicked();
    void onAdaptiveBilateralFilterParametersChanged(int, double, double, bool);
    void onMedianBlurButtonClicked();
    void onMedianBlurParametersChanged(int, double, double, bool);
    void onGaussianBinaryzationButtonClicked();
    void onGaussianBinaryzationParametersChanged(int, double, double, bool);
    void onMedianBinaryzationButtonClicked();
    void onMedianBinaryzationParametersChanged(int, double, double, bool);
    void onFloodFillButtonClicked();
    void onFloodFillParametersChanged(int, double, double, bool);
    void onFloodFillMouseClicked(QPoint);
    void onFloodFillAccepted();
    void onFloodFillRejected();
    void onScharrButtonClicked();
    void onCannyButtonClicked();
    void onCannyParametersChanged(int, double, double, bool);
    void onWatershedButtonClicked();
    void onWatershedPreviewed(const QVector<QVector<QPoint> > &);
    void onWatershedAccepted(const QVector<QVector<QPoint> > &);
    void onPreParametersAccepted();
    void onPreParametersRejected();
    void onContoursButtonClicked();
    void onPreviewWorkFinished();
    void onPreProcessWorkFinished();
    void onPreProcessButtonBoxClicked(QAbstractButton *);

    //analysis
    void onContourIndexFound(const QModelIndex &);
    void onCurrentClassChanged(int);
    void onAnalysisButtonBoxClicked(QAbstractButton *);

    //information
    void onSplitSpinBoxValueChanged(int);
    void onSplitButtonClicked();

    //menu and toolbar
    void onCurrentTabChanged(int);
    void addDiskFileDialog();
    void addCameraImageDialog();
    void onResetActionTriggered();
    void onOptionsActionTriggered();
    void onQuickExportTriggered();
    void onInformationExportTriggered();
    void aboutQtDialog();
    void aboutCAIGADialog();
    void onMessagesArrived(const QString &);
    void updateOptions(int, int, int, const QString &);

private:
    Ui::MainWindow *ui;
    QMenu *mouseBehaviourMenu;
    ParametersDialog *parametersDlg;
    WatershedMarkerDialog *watershedDlg;
    CAIGA::Analyser *analyser;
    AnalysisItemDelegate *analysisDelegate;
    Reporter *reporter;

    void handleParametersDialogue(void (MainWindow::*) (int, double, double, bool));
    void updateInformationReport(int);

    QSettings settings;
    void readConfig();
    void setCurrentDirbyFile(QString &);
    CAIGA::Image cgimg;
    CAIGA::CCSpace ccSpace;
    CAIGA::WorkSpace preWorkSpace;
    CAIGA::WorkSpace previewSpace;
    QVector<QPoint> floodfillPts;

    //Text in About Dialog
    const static QString aboutText;

protected:
     void closeEvent(QCloseEvent *event);
};
#endif // MAINWINDOW_H
