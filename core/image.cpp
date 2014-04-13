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

QImage Image::getRawImage()
{
    return convertMat2QImage(rawImage);
}

QImage Image::getEdges()
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

void Image::setRawImage(Mat img)
{
    rawImage = img;
    m_isPreProcessed = false;
    m_isProcessed = false;
    m_isAnalysed = false;
}

void Image::setRawImage(QImage qimg)
{
    rawImage = convertQImage2Mat(qimg);
    m_isPreProcessed = false;
    m_isProcessed = false;
    m_isAnalysed = false;
}

void Image::setRawImage(const QString &imgfile)
{
    rawImage = cv::imread(imgfile.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
    m_isPreProcessed = false;
    m_isProcessed = false;
    m_isAnalysed = false;
}

void Image::setEdges(Mat img)
{
    edges = img;
    m_hasEdges = true;
    m_isPreProcessed = false;
    m_isProcessed = false;
    m_isAnalysed = false;
}

void Image::setPreProcessedImage(Mat img)
{
    preprocessedImage = img;
    m_isPreProcessed = true;
    m_isProcessed = false;
    m_isAnalysed = false;
}

void Image::setProcessedImage(Mat img)
{
    processedImage = img;
    m_isProcessed = true;
    m_isAnalysed = false;
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

void Image::toBeCannyed(double ht, double lt, int aSize, bool l2)
{
    cv::Mat out;
    cv::Canny(rawImage, out, ht, lt, aSize, l2);
    setEdges(out);
}

QStringList Image::getInfoList()
{
    return infoList;
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
    Mat t(qimg.height(), qimg.width(), qimg.format(), (uchar *)qimg.bits(), qimg.bytesPerLine());
    return t;
}
