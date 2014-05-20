#ifndef WATERSHEDMARKERDIALOG_H
#define WATERSHEDMARKERDIALOG_H

#include <QDialog>
#include <QMenu>
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
    void setOriginalMat(cv::Mat *src);

public slots:
    void updateMarkers();
    void onPreviewStarted();
    void onPreviewFinished();

signals:
    void reseted();
    void previewTriggered(const cv::Mat *);
    void accepted();

private:
    QMenu *brushMenu;
    cv::Mat rawMat;
    QColor m_colour;
    Ui::WatershedMarkerDialog *ui;
    CAIGA::WorkSpace markSpace;

private slots:
    void onAutoClicked();
    void onMarkFinished(const QVector<QPoint> &);
    void onResetButtonClicked();
    void onUndoButtonClicked();
    void onRedoButtonClicked();
    inline void onPreviewButtonClicked() { emit previewTriggered(markSpace.getLastMatrix()); }
    void onWorkStarted();
    void onWorkFinished();
    inline void handleAccepted() { emit accepted(); }
    void onBrushPencil();
    void onBrushEraser();
};

#endif // WATERSHEDMARKERDIALOG_H
