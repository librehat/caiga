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
#include <QCameraInfo>
#include <QCameraImageCapture>
#include <QDir>

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

signals:
    void imageAccepted(const QImage &);

private slots:
    inline void updateCameraDevice(int camId) { setCamera(cameraList.at(camId)); }
    void setCamera(const QCameraInfo &cam);
    void takePicture();
    void onImageCaptured(int, const QImage &img);

    void displaycameraError();
    void displaycaptureError(int, QCameraImageCapture::Error, const QString &err);

    inline void handleAccepted() { emit imageAccepted(capturedImage); }

private:
    Ui::CameraDialog *ui;

    QCamera *camera;
    QList<QCameraInfo> cameraList;
    QCameraImageCapture *imageCapture;
    QImage capturedImage;
};

#endif // CAMERADIALOG_H
