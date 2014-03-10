/*
 * "Load Image from Camera" Dialog
 *
 * Don't define real data classes in this file.
 *
 */
#ifndef CAMERADIALOG_H
#define CAMERADIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QCamera>
#include <QCameraImageCapture>

#include "caigaglobal.h"

namespace Ui
{
class CameraDialog;
}

class CameraDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CameraDialog(QWidget *parent = 0);
    ~CameraDialog();

private slots:
    void updateCameraDevice(int camId);
    void setCamera(const QByteArray &cameraDevice);
    void takePicture();
    void displayCapturedImage(int, const QImage &img);

    void displaycameraError();
    void displaycaptureError(int, QCameraImageCapture::Error, const QString &err);

private:
    Ui::CameraDialog *ui;

    QCamera *camera;
    QCameraImageCapture *imageCapture;
    QImage capturedImage;
};

#endif // CAMERADIALOG_H
