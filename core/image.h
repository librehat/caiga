/*
 * Every CAIGA::Image object contains:
 * - Name (should be unique)
 * - Remarks (optional, maybe empty)
 * - Original Image
 * - Pre-Processed Image
 * - Processed Image (after analysis)
 * - Original analysis data
 * - Analysis result used to display on screen
 *
 * Images are in type of QImage since it's easy to convert to Mat and QPixmap
 *
 */
#ifndef IMAGE_H
#define IMAGE_H

#include "core_lib.h"
#include <QStringList>
#include <QImage>
#include <QPixmap>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;

/*
 * define inside CAIGA namespace
 * in case of global conflict
 */
namespace CAIGA {
// Mat is implemented internally instead of relevant Qt Classes
class CORE_LIB Image
{
public:
    Image();
    Image(QImage img);
    Image(Mat matImg);
    Image(const QString &imgfile);
    ~Image();
    Mat getRawMatrix();
    QPixmap getRawPixmap();
    QImage getRawImage();
    void setRawImage(Mat img);
    void setRawImage(QImage qimg);
    void setRawImage(const QString &imgfile);
    Mat getCroppedMatrix();
    QImage getCroppedImage();
    QPixmap getCroppedPixmap();
    QImage getPreProcessedImage();
    QPixmap getPreProcessedPixmap();
    QImage getProcessedImage();
    QPixmap getProcessedPixmap();
    void setProcessedImage(Mat img);
    inline bool isCircle() { return m_isCircle; }
    bool isCropped();
    bool isPreProcessed();
    bool hasEdges();
    bool isProcessed();
    bool isAnalysed();

    QStringList getInfoList();

    static Mat makeInCircle(const cv::Mat &rect);
    static QImage convertMat2QImage(const cv::Mat &src);
    static QPixmap convertMat2QPixmap(const cv::Mat &src);
    static Mat convertQImage2Mat(const QImage &qimg, bool indexed = false);

    friend class WorkSpace;
    friend class CCSpace;

private:
    bool m_isCircle;
    Mat rawImage;//original image
    Mat croppedImage;//preProcess later on take in this image. when isCircle, we use mask to make a circular image and store it in croppedCircularImage
    Mat croppedCircularImage;//when isCircle, return this
    Mat preprocessedImage;//pre-processed image. same thing here
    Mat preprocessedCircularImage;//return this when isCircle
    Mat edgesImage;//same thing here.
    Mat edgesCircularImage;//return this when isCircle
    Mat processedImage;//image ready to be analysed (shed colour already)
    Mat processedCircularImage;

    /*
     * analysis data are defined below
     */
    QStringList infoList;//information displayed on the info text browser

};

}

#endif // IMAGE_H
