#include <QFuture>
#include "image.h"
using namespace CAIGA;

Image::Image()
{
    m_hasEdges = false;
    m_isPreProcessed = false;
    m_isProcessed = false;
    m_isAnalysed = false;
}

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
    m_isCropped = false;
    m_isPreProcessed = false;
    m_hasEdges = false;
    m_isProcessed = false;
    m_isAnalysed = false;
}

void Image::setRawImage(QImage qimg)
{
    rawImage = convertQImage2Mat(qimg, true);
    m_isCropped = false;
    m_isPreProcessed = false;
    m_hasEdges = false;
    m_isProcessed = false;
    m_isAnalysed = false;
}

void Image::setRawImage(const QString &imgfile)
{
    rawImage = cv::imread(imgfile.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
    m_isCropped = false;
    m_isPreProcessed = false;
    m_hasEdges = false;
    m_isProcessed = false;
    m_isAnalysed = false;
}

void Image::setCropCalibreStruct(const ccStruct &c)
{
    cropCalibre = c;
    if(cropCalibre.isCircle) {
        //TODO
    }
    else {
        cv::Point tl(c.rect.topLeft().x(), c.rect.topLeft().y());
        cv::Point br(c.rect.bottomRight().x(), c.rect.bottomRight().y());
        cv::Rect rect(tl, br);
        croppedImage = rawImage(rect).clone();
    }
    m_isCropped = true;
    m_isPreProcessed = false;
    m_hasEdges = false;
    m_isProcessed = false;
    m_isAnalysed = false;
}

void Image::setPreProcessedImage(Mat img)
{
    preprocessedImage = img;
    m_isPreProcessed = true;
    m_hasEdges = false;
    m_isProcessed = false;
    m_isAnalysed = false;
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
    m_isProcessed = true;
    m_isAnalysed = false;
}

bool Image::isCropped()
{
    return m_isCropped;
}

bool Image::hasEdges()
{
    return m_hasEdges;
}

bool Image::isPreProcessed()
{
    return m_isPreProcessed;
}

bool Image::isProcessed()
{
    return m_isProcessed;
}

bool Image::isAnalysed()
{
    return m_isAnalysed;
}

QStringList Image::getInfoList()
{
    return infoList;
}

Mat Image::ImageToCannyed(const Mat &img, double ht, double lt, int aSize, bool l2)
{
    if (img.depth() != CV_8U) {
        qWarning("Error: Canny can't accept a Mat whose depth is not CV_8U!");
        return cv::Mat();
    }
    else {
        cv::Mat out;
        cv::Canny(img, out, ht, lt, aSize, l2);
        return out;
    }
}
//cv::Mat and QImage conversion code is based on http://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap/
QImage Image::convertMat2QImage(const cv::Mat &src)
{
    switch(src.type()) {
    case CV_8UC3:
    {
        QImage t(src.data, src.cols, src.rows, (int)src.step, QImage::Format_RGB888);
        return t.rgbSwapped();
    }
    case CV_8UC4:
    {
        QImage t(src.data, src.cols, src.rows, (int)src.step, QImage::Format_RGB32);
        return t;
    }
    case CV_8UC1:
    {
        static QVector<QRgb> colorTable;
        if (colorTable.isEmpty()) {
            for (int i=0; i<256; i++) {
                colorTable.push_back(qRgb(i,i,i));
            }
        }
        QImage t(src.data, src.cols, src.rows, (int)src.step, QImage::Format_Indexed8);
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
