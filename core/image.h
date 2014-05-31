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
    QImage getProcessedImage();
    QPixmap getProcessedPixmap();
    void setProcessedImage(Mat img);
    inline qreal getScaleValue() { return m_scale; }
    bool isCropped();
    bool isProcessed();

    inline QString getFileName() { return m_filename; }

    static Mat makeInCircle(const cv::Mat &rect);
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
    Mat processedImage;//image ready to be analysed (segmented and painted already)
};

}

#endif // IMAGE_H
