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
    cameraList = QCameraInfo::availableCameras();
    foreach (const QCameraInfo &cameraInfo, cameraList) {
        ui->cameraBox->addItem(cameraInfo.description(), QVariant(cameraInfo.deviceName()));
    }
    if (!cameraList.isEmpty()) {
        setCamera(cameraList.first());
    }

    //SIGNALs and SLOTs
    connect(ui->cameraBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CameraDialog::updateCameraDevice);
    connect(ui->captureButton, &QPushButton::clicked, this, &CameraDialog::takePicture);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &CameraDialog::handleAccepted);
}

CameraDialog::~CameraDialog()
{
    delete ui;
}

void CameraDialog::setCamera(const QCameraInfo &cam)
{
    if (imageCapture != NULL) {
        delete imageCapture;
    }
    if (camera != NULL) {
        delete camera;
    }

    if (cam.isNull()) {//null or invalid
        camera = new QCamera(this);
    }
    else {
        camera = new QCamera(cam, this);
    }

    camera->setViewfinder(ui->viewfinder);
    camera->setCaptureMode(QCamera::CaptureStillImage);
    connect(camera, static_cast<void (QCamera::*) (QCamera::Error)>(&QCamera::error), this, &CameraDialog::displaycameraError);

    imageCapture = new QCameraImageCapture(camera, this);
    imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);
    imageCapture->setBufferFormat(QVideoFrame::Format_RGB32);
    connect(imageCapture, &QCameraImageCapture::imageCaptured, this, &CameraDialog::onImageCaptured);
    connect(imageCapture, static_cast<void (QCameraImageCapture::*) (int, QCameraImageCapture::Error, const QString&)>(&QCameraImageCapture::error), this, &CameraDialog::displaycaptureError);
    camera->start();
}

void CameraDialog::takePicture()
{
    camera->searchAndLock();
    imageCapture->capture(QDir::tempPath() + QString("/caiga_camera.jpeg"));
    camera->unlock();
}

void CameraDialog::onImageCaptured(int, const QImage &img)
{
    capturedImage = img;
    capturedImage.detach();
    ui->viewLabel->setPixmap(QPixmap::fromImage(img).scaled(ui->viewLabel->size(), Qt::KeepAspectRatio, Qt::FastTransformation));
}

void CameraDialog::displaycameraError()
{
    QMessageBox::warning(this, "Error", camera->errorString());
}

void CameraDialog::displaycaptureError(int, QCameraImageCapture::Error, const QString &err)
{
    QMessageBox::warning(this, "Error", err);
}
