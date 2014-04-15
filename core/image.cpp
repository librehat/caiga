#include "image.h"
#include <QDebug>
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
    rawImage = convertQImage2Mat(qimg);
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

void Image::setCroppedImage(QImage qimg)
{
    croppedImage = convertQImage2Mat(qimg);
    m_isCropped = true;
    m_isPreProcessed = false;
    m_hasEdges = false;
    m_isProcessed = false;
    m_isAnalysed = false;
}

void Image::setEdges(Mat img)
{
    edges = img;
    m_hasEdges = true;
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

void Image::setCalibre(int p, double c)
{
    m_calibre = static_cast<double>(p) / c;
}

double Image::getCalibre()
{
    return m_calibre;
}

QStringList Image::getInfoList()
{
    return infoList;
}

QPixmap Image::ImageToCannyedPixmap(const Mat &img, double ht, double lt, int aSize, bool l2)
{
    if (img.depth() != CV_8U) {
        qWarning("Error: Canny can't accept a Mat whose depth is not CV_8U!");
        return QPixmap();
    }
    else {
        cv::Mat out;
        cv::Canny(img, out, ht, lt, aSize, l2);
        return convertMat2QPixmap(out);
    }
}

QImage Image::convertMat2QImage(const cv::Mat &src)
{
    switch(src.type()) {
    case CV_8UC3:
    {
        QImage t(src.data, src.cols, src.rows, (int)src.step, QImage::Format_RGB888);
        return t.rgbSwapped();
        break;
    }
    case CV_8UC4:
    {
        QImage t(src.data, src.cols, src.rows, (int)src.step, QImage::Format_RGB32);
        return t;
        break;
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
        break;
    }
    default:
        qWarning("Unsupported Mat type.");
        return QImage();
    }
}

QPixmap Image::convertMat2QPixmap(const cv::Mat &src)
{
    return QPixmap::fromImage(convertMat2QImage(src));
}


Mat Image::convertQImage2Mat(const QImage &qimg)
{
    Mat t(qimg.height(), qimg.width(), qimg.format(), const_cast<uchar *>(qimg.bits()), qimg.bytesPerLine());
    return t;
}
