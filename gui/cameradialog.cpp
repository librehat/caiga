#include "cameradialog.h"
#include "ui_cameradialog.h"

CameraDialog::CameraDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CameraDialog),
    camera(NULL),
    imageCapture(NULL)
{
    ui->setupUi(this);

    //Camera devices:
    QByteArray cameraDevice;
    foreach(const QByteArray &deviceName, QCamera::availableDevices()) {
        QString description = camera->deviceDescription(deviceName);
        if (cameraDevice.isEmpty()) {//assign a camera device
            cameraDevice = deviceName;
        }
        ui->cameraBox->addItem(description, QVariant(deviceName));
    }

    setCamera(cameraDevice);

    //SIGNALs and SLOTs
    connect(ui->cameraBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CameraDialog::updateCameraDevice);
    connect(ui->captureButton, &QPushButton::clicked, this, &CameraDialog::takePicture);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &CameraDialog::handleAccepted);
}

CameraDialog::~CameraDialog()
{
    delete ui;
}

void CameraDialog::updateCameraDevice(int camId)
{
    setCamera(ui->cameraBox->itemData(camId).toByteArray());
}

void CameraDialog::setCamera(const QByteArray &cameraDevice)
{
    if (imageCapture != NULL) {
        delete imageCapture;
    }
    if (camera != NULL) {
        delete camera;
    }

    if (cameraDevice.isEmpty()) {
        camera = new QCamera(this);
    }
    else {
        camera = new QCamera(cameraDevice, this);
    }

    camera->setViewfinder(ui->viewfinder);
    camera->setCaptureMode(QCamera::CaptureStillImage);
    connect(camera, static_cast<void (QCamera::*) (QCamera::Error)>(&QCamera::error), this, &CameraDialog::displaycameraError);

    imageCapture = new QCameraImageCapture(camera, this);
    //imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToFile);
    imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);
    imageCapture->setBufferFormat(QVideoFrame::Format_RGB32);
    connect(imageCapture, &QCameraImageCapture::imageCaptured, this, &CameraDialog::onImageCaptured);
    connect(imageCapture, static_cast<void (QCameraImageCapture::*) (int, QCameraImageCapture::Error, const QString&)>(&QCameraImageCapture::error), this, &CameraDialog::displaycaptureError);
    camera->start();
}

void CameraDialog::takePicture()
{
    camera->searchAndLock();
    imageCapture->capture(QDir::tempPath() + QString("/caiga_tmp.jpeg"));
    camera->unlock();
}

void CameraDialog::onImageCaptured(int, const QImage &img)
{
    capturedImage = img;
    capturedImage.detach();
    ui->viewLabel->setPixmap(QPixmap::fromImage(img).scaled(ui->viewLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void CameraDialog::displaycameraError()
{
    QMessageBox::warning(this, "Error", camera->errorString());
}

void CameraDialog::displaycaptureError(int, QCameraImageCapture::Error, const QString &err)
{
    QMessageBox::warning(this, "Error", err);
}
