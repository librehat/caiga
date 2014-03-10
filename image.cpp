#include "image.h"
using namespace CAIGA;

Image::Image()
{
    m_isPreProcessed = false;
    m_isProcessed = false;
    m_isAnalysed = false;
}

Image::Image(QImage img)
{
    origImage = img;
    origImage.detach();
    m_isPreProcessed = false;
    m_isProcessed = false;
    m_isAnalysed = false;
}

Image::Image(Mat &matImg)
{
    origImage = convertMat2QImage(matImg);
    origImage.detach();
    m_isPreProcessed = false;
    m_isProcessed = false;
    m_isAnalysed = false;
}

Image::Image(const QString &imgfile)
{
    origImage = QImage(imgfile);
    m_isPreProcessed = false;
    m_isProcessed = false;
    m_isAnalysed = false;
}

Image::Image(const QByteArray &orig, QByteArray *pre = 0, QByteArray *pro = 0, QStringList *info = 0)
{
    origImage = QImage::fromData(orig);
    if (pre != 0) {
        preprocessedImage = QImage::fromData(*pre);
        m_isPreProcessed = true;
    }
    else {
        m_isPreProcessed = false;
    }
    if (pro != 0) {
        processedImage = QImage::fromData(*pro);
        m_isProcessed = true;
    }
    else {
        m_isProcessed = false;
    }
    if (info != 0) {
        infoList = QStringList(*info);
        m_isAnalysed = true;
    }
    else {
        m_isAnalysed = false;
    }
}

Image::~Image()
{
    //NOTHING
}

QImage Image::getOrigImage()
{
    return origImage;
}

QImage Image::getProcessedImage()
{
    return processedImage;
}

void Image::setOrigImage(QImage img)
{
    origImage = img;
    m_isPreProcessed = false;
    m_isProcessed = false;
    m_isAnalysed = false;
}

void Image::setPreProcessedImage(QImage img)
{
    preprocessedImage = img;
    m_isPreProcessed = true;
    m_isProcessed = false;
    m_isAnalysed = false;
}

void Image::setProcessedImage(QImage img)
{
    processedImage = img;
    m_isProcessed = true;
    m_isAnalysed = false;
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

QImage Image::convertMat2QImage(const cv::Mat &src)
{
    switch(src.type()) {
    case CV_8UC3:
    {
        QImage t(src.data, src.cols, src.rows, src.step, QImage::Format_RGB888);
        return t.rgbSwapped();
        break;
    }
    case CV_8UC4:
    {
        QImage t(src.data, src.cols, src.rows, src.step, QImage::Format_RGB32);
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
        QImage t(src.data, src.cols, src.rows, src.step, QImage::Format_Indexed8);
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
