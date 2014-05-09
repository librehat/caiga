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
    void setOrignialMat(cv::Mat *src);

public slots:
    void updateMarkers();
    void onPreviewStarted();
    void onPreviewFinished();

signals:
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
    void onMarkFinished(const QVector<QPoint> &);
    void onResetButtonClicked();
    void onUndoButtonClicked();
    void onRedoButtonClicked();
    void onPreviewButtonClicked();
    void onWorkStarted();
    void onWorkFinished();
    void handleAccepted();
};

#endif // WATERSHEDMARKERDIALOG_H
