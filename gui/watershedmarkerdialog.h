#ifndef WATERSHEDMARKERDIALOG_H
#define WATERSHEDMARKERDIALOG_H

#include <QDialog>
#include "workspace.h"

namespace Ui {
class WatershedMarkerDialog;
}

class WatershedMarkerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WatershedMarkerDialog(QWidget *parent = 0);
    ~WatershedMarkerDialog();
    void setPenColour(const QColor &c);
    inline void setOrignialImage(CAIGA::Image *img) { viewSpace.setImage(img); }//invoke this function before setOriginalMat, otherwise it may fail
    void setOrignialMat(cv::Mat *src);

public slots:
    void updateMarkers();
    void onPreviewStarted();
    void onPreviewFinished();

signals:
    void reseted();
    void autoMarked();
    void previewTriggled(const QVector<QVector<QPoint> > &);
    void accepted(const QVector<QVector<QPoint> > &);

private:
    cv::Mat *m_originalMat;
    QColor m_colour;
    Ui::WatershedMarkerDialog *ui;
    QVector<QPoint> pts;
    QVector<QVector<QPoint> > ptsVec;
    QVector<QVector<QPoint> > ptsVecUndone;
    CAIGA::WorkSpace viewSpace;

private slots:
    void onAutoClicked();
    void onMarkFinished(const QVector<QPoint> &);
    void onResetButtonClicked();
    void onUndoButtonClicked();
    void onRedoButtonClicked();
    inline void onPreviewButtonClicked() { emit previewTriggled(ptsVec); }
    void onWorkStarted();
    void onWorkFinished();
    inline void handleAccepted() { emit accepted(ptsVec); }
};

#endif // WATERSHEDMARKERDIALOG_H
