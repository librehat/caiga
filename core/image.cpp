#include <QFuture>
#include "image.h"
using namespace CAIGA;

Image::Image()
{
    m_filename = QString("CAIGA_Image");
    m_isCircle = true;
    m_scale = 0;
}

Image::Image(QImage img)
{
    Image();
    setRawImage(img);
}

Image::Image(Mat matImg)
{
    Image();
    setRawImage(matImg);
}

Image::Image(const QString &imgfile)
{
    Image();
    setRawImageByFile(imgfile);
}

Image::~Image()
{
    //NOTHING
}

Mat Image::getRawMatrix()
{
    return rawImage;
}

Mat Image::getCroppedMatrix()
{
    return croppedImage;
}

QImage Image::getRawImage()
{
    return convertMat2QImage(rawImage);
}

QImage Image::getCroppedImage()
{
    if (isCircle()) {
        return convertMat2QImage(croppedCircularImage);
    }
    else
        return convertMat2QImage(croppedImage);
}

QImage Image::getPreProcessedImage()
{
    if (isCircle()) {
        preprocessedCircularImage = makeInCircle(preprocessedImage);
        return convertMat2QImage(preprocessedCircularImage);
    }
    else
        return convertMat2QImage(preprocessedImage);
}

QImage Image::getProcessedImage()
{
    if (isCircle()) {
        processedCircularImage = makeInCircle(processedImage);
        return convertMat2QImage(processedCircularImage);
    }
    else
        return convertMat2QImage(processedImage);
}

QPixmap Image::getRawPixmap()
{
    return convertMat2QPixmap(rawImage);
}

QPixmap Image::getCroppedPixmap()
{
    if (isCircle()) {
        return convertMat2QPixmap(croppedCircularImage);
    }
    else {
        return convertMat2QPixmap(croppedImage);
    }
}

QPixmap Image::getPreProcessedPixmap()
{
    if (isCircle()) {
        preprocessedCircularImage = makeInCircle(preprocessedImage);
        return convertMat2QPixmap(preprocessedCircularImage);
    }
    else
        return convertMat2QPixmap(preprocessedImage);
}

QPixmap Image::getProcessedPixmap()
{
    if (isCircle()) {
        processedCircularImage = makeInCircle(processedImage);
        return convertMat2QPixmap(processedCircularImage);
    }
    else
        return convertMat2QPixmap(processedImage);
}

void Image::setRawImage(Mat img)
{
    rawImage = img;
}

void Image::setRawImage(QImage qimg)
{
    rawImage = convertQImage2Mat(qimg, true);
}

void Image::setRawImageByFile(const QString &imgfile)
{
    rawImage = cv::imread(imgfile.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
    m_filename = imgfile;
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
    return !edgesImage.empty();
}

bool Image::isPreProcessed()
{
    return !preprocessedImage.empty();
}

bool Image::isProcessed()
{
    return !processedImage.empty();
}

Mat Image::makeInCircle(const Mat &rect)
{
    if (rect.empty()) {
        qWarning("Abort. Input rect image is empty!");
        return cv::Mat();
    }

    //notice that this centre is relevant to croppedImage instead of rawImage
    cv::Point centre(rect.rows / 2, rect.cols / 2);
    cv::Mat circle = cv::Mat::zeros(rect.rows, rect.cols, CV_8UC1);
    cv::Mat out = cv::Mat::zeros(rect.rows, rect.cols, CV_8UC1);
    cv::circle(circle, centre, centre.x, 255, -1, 8, 0);
    cv::bitwise_and(rect, rect, out, circle);//bitwise_and is the C++ version of "cvAnd"
    return out.clone();
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
