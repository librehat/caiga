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
#include "calibrespace.h"
#include "workspace.h"
#include "cameradialog.h"
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
    void configReadFinished(int, int, const QString &, int);
    void binaryParametersChanged();
    void messageArrived(const QString &);//to display on statusbar

private slots:
    //calibre
    void calibreModeChanged(int);
    void onMeasureFinished(qreal);
    void onCalibreLoadMacroButtonClicked();
    void onCalibreSaveMacroButtonClicked();
    void onCalibreButtonBoxClicked(QAbstractButton *);

    //process
    void onMouseNormalArrow();
    void onMouseWhitePencil();
    void onMouseBlackPencil();
    void onPencilDrawFinished(const QVector<QPointF> &);
    void onMouseWhiteEraser();
    void onMouseBlackEraser();
    void onEraserDrawFinished(const QVector<QPointF> &);
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
    inline void onFloodFillParametersChanged(int, double h, double l, bool c8) { previewSpace->setFloodFillWorkParameters(h, l, c8); }
    inline void onFloodFillMouseClicked(QPointF p) { floodfillPts.append(p); previewSpace->newFloodFillWork(floodfillPts); }
    void onFloodFillAccepted();
    void onFloodFillRejected();
    void onCannyButtonClicked();
    inline void onCannyParametersChanged(int aSize, double high, double low, bool l2) { previewSpace->newCannyWork(aSize, high, low, l2); }
    void onWatershedButtonClicked();
    inline void onWatershedPreviewed(const Mat *input) { previewSpace->newWatershedWork(input); }
    void onWatershedAccepted();
    void onParametersAccepted();
    void onParametersRejected();
    inline void onContoursButtonClicked() { processSpace->newContoursWork(); }
    void onPreviewWorkFinished();
    void onProcessWorkFinished();
    void onProcessButtonBoxClicked(QAbstractButton *);

    //analysis
    void onAnalysisFinished();
    void onContourIndexFound(const QModelIndex &);
    void onCurrentClassChanged(int);
    void onAnalysisButtonBoxClicked(QAbstractButton *);

    //report
    void onSplitSpinBoxValueChanged(int);
    void onSplitButtonClicked();
    void onReportAvailable(bool);

    //menu and toolbar
    void onCurrentTabChanged(int);
    void onUndoTriggered();
    void onRedoTriggered();
    void onAddDiskImageTriggered();
    void onAddCameraImageTriggered();
    void onCameraImageAccepted(const QImage &);
    void onResetActionTriggered();
    void onOptionsActionTriggered();
    void onQuickExportTriggered();
    void onReportExportTriggered();
    void onSaveCurrentImageTriggered();
    inline void onAboutQt() { QMessageBox::aboutQt(this); }
    inline void onAboutCAIGA() { QMessageBox::about(this, "About CAIGA", aboutText); }
    void onMessagesArrived(const QString &);
    void onZoomUpdated(qreal);
    void updateOptions(int, int, const QString &, int);

private:
    Ui::MainWindow *ui;
    QMenu *mouseBehaviourMenu;
    QLabel *zoomLabel;
    CameraDialog *cameraDlg;
    ParametersDialog *parametersDlg;
    WatershedMarkerDialog *watershedDlg;
    CAIGA::Analyser *analyser;
    AnalysisItemDelegate *analysisDelegate;
    Reporter *reporter;
    int m_undoSteps;

    void onNewImageOpened();
    void updateRedoUndoStatus();
    void handleParametersDialogue(void (MainWindow::*) (int, double, double, bool));
    void updateReportTextBrowser(int);

    QSettings settings;
    void readConfig();
    void setCurrentDirbyFile(QString &);

    CAIGA::Image cgimg;
    CAIGA::CalibreSpace *calibreSpace;
    CAIGA::WorkSpace *processSpace;
    CAIGA::WorkSpace *previewSpace;
    QVector<QPointF> floodfillPts;

    //Text in About Dialog
    const static QString aboutText;

protected:
     void closeEvent(QCloseEvent *event);
};
#endif // MAINWINDOW_H
