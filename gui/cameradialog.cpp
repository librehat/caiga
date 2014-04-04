#include "cameradialog.h"
#include "ui_cameradialog.h"

CameraDialog::CameraDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CameraDialog),
    camera(0),
    imageCapture(0)
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
}

CameraDialog::~CameraDialog()
{
    delete ui;
    delete camera;
    delete imageCapture;
}

void CameraDialog::updateCameraDevice(int camId)
{
    setCamera(ui->cameraBox->itemData(camId).toByteArray());
}

void CameraDialog::setCamera(const QByteArray &cameraDevice)
{

    delete imageCapture;
    delete camera;

    if (cameraDevice.isEmpty())
        camera = new QCamera;
    else
        camera = new QCamera(cameraDevice);

    camera->setViewfinder(ui->viewfinder);
    camera->setCaptureMode(QCamera::CaptureStillImage);
    connect(camera, SIGNAL(error(QCamera::Error)), this, SLOT(displaycameraError()));

    imageCapture = new QCameraImageCapture(camera);
    imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToFile);
    connect(imageCapture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(displayCapturedImage(int,QImage)));

    connect(imageCapture, SIGNAL(error(int,QCameraImageCapture::Error,QString)), this, SLOT(displaycaptureError(int,QCameraImageCapture::Error,QString)));

    camera->start();
}

void CameraDialog::takePicture()
{
    camera->searchAndLock();
    imageCapture->capture(QDir::tempPath() + QString("/caiga_tmp.jpeg"));
    camera->unlock();
}

void CameraDialog::displayCapturedImage(int, const QImage &img)
{
    capturedImage = img;
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
