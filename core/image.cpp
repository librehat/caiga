#include <QFuture>
#include "image.h"
using namespace CAIGA;

Image::Image()
{}

Image::Image(QImage img)
{
    setRawImage(img);
}

Image::Image(Mat matImg)
{
    setRawImage(matImg);
}

Image::Image(const QString &imgfile)
{
    setRawImage(imgfile);
}

Image::~Image()
{
    //NOTHING
}

Mat Image::getRawMatrix()
{
    return rawImage;
}

QImage Image::getRawImage()
{
    return convertMat2QImage(rawImage);
}

QImage Image::getCroppedImage()
{
    return convertMat2QImage(croppedImage);
}

QImage Image::getEdgesImage()
{
    return convertMat2QImage(edges);
}

QImage Image::getPreProcessedImage()
{
    return convertMat2QImage(preprocessedImage);
}

QImage Image::getProcessedImage()
{
    return convertMat2QImage(processedImage);
}

QPixmap Image::getRawPixmap()
{
    return convertMat2QPixmap(rawImage);
}

QPixmap Image::getCroppedPixmap()
{
    return convertMat2QPixmap(croppedImage);
}

QPixmap Image::getPreProcessedPixmap()
{
    return convertMat2QPixmap(preprocessedImage);
}

QPixmap Image::getEdgesPixmap()
{
    return convertMat2QPixmap(edges);
}

QPixmap Image::getProcessedPixmap()
{
    return convertMat2QPixmap(processedImage);
}

ccStruct Image::getCropCalibreStruct()
{
    return cropCalibre;
}

void Image::setRawImage(Mat img)
{
    rawImage = img;
}

void Image::setRawImage(QImage qimg)
{
    rawImage = convertQImage2Mat(qimg, true);
}

void Image::setRawImage(const QString &imgfile)
{
    rawImage = cv::imread(imgfile.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
}

void Image::setCropCalibreStruct(const ccStruct &c)
{
    cropCalibre = c;
    if(cropCalibre.isCircle) {
        cv::Point tl(c.centre.x() - c.radius, c.centre.y() - c.radius);
        cv::Point br(c.centre.x() + c.radius, c.centre.y() + c.radius);
        cv::Rect enclosingRect(tl, br);
        croppedEnclosingRectImage = rawImage(enclosingRect).clone();

        //notice that this centre is relevant to croppedImage instead of rawImage
        cv::Point centre(enclosingRect.width / 2, enclosingRect.height / 2);
        cv::Mat circle = cv::Mat::zeros(croppedEnclosingRectImage.rows, croppedEnclosingRectImage.cols, CV_8UC1);
        croppedImage = cv::Mat::zeros(croppedEnclosingRectImage.rows, croppedEnclosingRectImage.cols, CV_8UC1);
        cv::circle(circle, centre, c.radius, 255, -1, 8, 0);
        cv::bitwise_and(croppedEnclosingRectImage, croppedEnclosingRectImage, croppedImage, circle);//bitwise_and is the C++ version of "cvAnd"
    }
    else {
        cv::Point tl(c.rect.topLeft().x(), c.rect.topLeft().y());
        cv::Point br(c.rect.bottomRight().x(), c.rect.bottomRight().y());
        cv::Rect rect(tl, br);
        croppedImage = rawImage(rect).clone();
    }
}

void Image::setPreProcessedImage(Mat img)
{
    preprocessedImage = img;
}

bool Image::validateHistogramEqualise()
{
    if (croppedImage.empty() || croppedImage.type() != CV_8UC1) {
        qWarning("Abort. CroppedImage is invalid. Its Mat type is: %d", croppedImage.type());
        return false;
    }
    else
        return true;
}

bool Image::validateGaussianMedianBlur()
{
    if (croppedImage.empty()) {
        qWarning("Abort. Cropped Image is invalid.");
        return false;
    }
    else
        return true;
}

bool Image::validateAdaptiveBilateralFilter()
{
    if ((croppedImage.type() != CV_8UC1 && croppedImage.type() != CV_8UC3) || croppedImage.empty()) {
        qWarning("Abort. CroppedImage is invalid. Its Mat type is: %d", croppedImage.type());
        return false;
    }
    else
        return true;
}

bool Image::validateBinaryzation()
{
    if (croppedImage.empty()) {
        qWarning("Abort. Cropped Image is invalid.");
        return false;
    }
    else
        return true;
}

bool Image::validateEdgesDetection()
{
    if (preprocessedImage.empty()) {
        qWarning("Abort. Image has not been preProcessed.");
        return false;
    }
    else
        return true;
}

void Image::setProcessedImage(Mat img)
{
    processedImage = img;
}

bool Image::isCropped()
{
    return !croppedImage.empty();
}

bool Image::hasEdges()
{
    return !edges.empty();
}

bool Image::isPreProcessed()
{
    return !preprocessedImage.empty();
}

bool Image::isProcessed()
{
    return !processedImage.empty();
}

bool Image::isAnalysed()
{
    return !infoList.empty();
}

QStringList Image::getInfoList()
{
    return infoList;
}

//cv::Mat and QImage conversion code is based on http://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap/
QImage Image::convertMat2QImage(const cv::Mat &src)
{
    switch(src.type()) {
    case CV_8UC3:
    {
        QImage t(src.data, src.cols, src.rows, static_cast<int>(src.step), QImage::Format_RGB888);
        return t.rgbSwapped();
    }
    case CV_8UC4:
    {
        QImage t(src.data, src.cols, src.rows, static_cast<int>(src.step), QImage::Format_RGB32);
        return t;
    }
    case CV_8UC1:
    {
        static QVector<QRgb> colorTable;
        if (colorTable.isEmpty()) {
            for (int i = 0; i < 256; i++) {
                colorTable.push_back(qRgb(i, i, i));
            }
        }
        QImage t(src.data, src.cols, src.rows, static_cast<int>(src.step), QImage::Format_Indexed8);
        t.setColorTable(colorTable);
        return t;
    }
    default:
        qWarning("Unsupported Mat type: %d", src.type());
        return QImage();
    }
}

QPixmap Image::convertMat2QPixmap(const cv::Mat &src)
{
    return QPixmap::fromImage(convertMat2QImage(src));
}

Mat Image::convertQImage2Mat(const QImage &qimg, bool indexed)
{
    QImage swapped = qimg.rgbSwapped();
    cv::Mat to;
    cv::Mat temp;
    switch (qimg.format()) {
    case QImage::Format_RGB32:
        to = Mat(qimg.height(), qimg.width(), CV_8UC4, const_cast<uchar *>(qimg.bits()), qimg.bytesPerLine()).clone();
        if (indexed) {
            cvtColor(to, temp, CV_RGB2GRAY);
            temp.convertTo(to, CV_8UC1);
        }
        return to.clone();
    case QImage::Format_RGB888:
        to = Mat(swapped.height(), swapped.width(), CV_8UC3, const_cast<uchar *>(swapped.bits()), swapped.bytesPerLine()).clone();
        if (indexed) {
            cvtColor(to, temp, CV_RGB2GRAY);//FIXME: not sure about this conversion
            temp.convertTo(to, CV_8UC1);
        }
        return to.clone();
    case QImage::Format_Indexed8:
        //it's indexed anyway
        return Mat(qimg.height(), qimg.width(), CV_8UC1, const_cast<uchar *>(qimg.bits()), qimg.bytesPerLine()).clone();
    default:
        qWarning("Unsupported QImage format: %d", qimg.format());
        return cv::Mat();
    }
}
