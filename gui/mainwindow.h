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

    //process
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
    inline void onFloodFillParametersChanged(int, double h, double l, bool c8) { previewSpace.setFloodFillWorkParameters(h, l, c8); }
    inline void onFloodFillMouseClicked(QPoint p) { floodfillPts.append(p); previewSpace.newFloodFillWork(floodfillPts); }
    void onFloodFillAccepted();
    void onFloodFillRejected();
    inline void onScharrButtonClicked() { preWorkSpace.newScharrWork(); }
    void onCannyButtonClicked();
    inline void onCannyParametersChanged(int aSize, double high, double low, bool l2) { previewSpace.newCannyWork(aSize, high, low, l2); }
    void onWatershedButtonClicked();
    inline void onWatershedPreviewed(const Mat *input) { previewSpace.newWatershedWork(input); }
    void onWatershedAccepted();
    void onParametersAccepted();
    void onParametersRejected();
    inline void onContoursButtonClicked() { preWorkSpace.newContoursWork(); }
    void onPreviewWorkFinished();
    void onProcessWorkFinished();
    void onProcessButtonBoxClicked(QAbstractButton *);

    //analysis
    void onContourIndexFound(const QModelIndex &);
    void onCurrentClassChanged(int);
    void onAnalysisButtonBoxClicked(QAbstractButton *);

    //information
    void onSplitSpinBoxValueChanged(int);
    void onSplitButtonClicked();
    void onReportAvailable(bool);

    //menu and toolbar
    void onCurrentTabChanged(int);
    void addDiskFileDialog();
    void addCameraImageDialog();
    void onCameraImageAccepted(const QImage &);
    void onResetActionTriggered();
    void onOptionsActionTriggered();
    void onQuickExportTriggered();
    void onInformationExportTriggered();
    inline void aboutQtDialog() { QMessageBox::aboutQt(this); }
    inline void aboutCAIGADialog() { QMessageBox::about(this, "About CAIGA", aboutText); }
    void onMessagesArrived(const QString &);
    void updateOptions(int, int, int, const QString &);

private:
    Ui::MainWindow *ui;
    QMenu *mouseBehaviourMenu;
    CameraDialog *cameraDlg;
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
