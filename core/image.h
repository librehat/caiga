#ifndef IMAGE_H
#define IMAGE_H

#include "core_lib.h"
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
class CORE_LIB Image
{
public:
    Image();
    Image(const QImage &img);
    Image(const Mat &matImg);
    Image(const QString &imgfile);
    ~Image() {}
    Mat getRawMatrix();
    QPixmap getRawPixmap();
    QImage getRawImage();
    void setRawImage(Mat img);
    void setRawImageFromCamera(const QImage &qimg);
    void setRawImageByFile(const QString &imgfile);
    Mat getCroppedMatrix();
    inline Mat *croppedMatrix() { return &croppedImage; }
    QImage getCroppedImage();
    QPixmap getCroppedPixmap();
    inline qreal getScaleValue() { return m_scale; }

    inline QString getFileName() { return m_filename; }

    //static Mat makeInCircle(const cv::Mat &rect);//we don't implement circular testing anymore
    static QImage convertMat2QImage(const cv::Mat &src);
    static QPixmap convertMat2QPixmap(const cv::Mat &src);
    static Mat convertQImage2Mat(const QImage &qimg, bool indexed = false);

    friend class WorkSpace;
    friend class CCSpace;

private:
    qreal m_scale;// pixel/um

    QString m_filename;//if it's from camera, then it'll use a default name.
    Mat rawImage;//original image
    Mat croppedImage;//process later on take in this image.
};

}

#endif // IMAGE_H
