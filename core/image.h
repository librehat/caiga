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
struct CORE_LIB ccStruct
{
    int drawMode;
    bool isCircle;
    QPoint pressed;
    QPoint released;
    QPoint centre;
    int radius;
    QRect rect;
    QLine calibreLine;
    qreal calibre;//pixel/μm
    qreal realSize;
    Mat croppedImage;
};

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
    QImage getCroppedImage();
    QPixmap getCroppedPixmap();
    ccStruct *getCropCalibreStruct();
    void setCroppedImage(const ccStruct &);
    QImage getEdgesImage();
    QPixmap getEdgesPixmap();
    void doEdgesDetection(double, double, int, bool);
    void setEdges(Mat img);
    QImage getPreProcessedImage();
    QPixmap getPreProcessedPixmap();
    void setPreProcessedImage(Mat img);
    QImage getProcessedImage();
    QPixmap getProcessedPixmap();
    void setProcessedImage(Mat img);
    bool isCropped();
    bool isPreProcessed();
    bool hasEdges();
    bool isProcessed();
    bool isAnalysed();

    QStringList getInfoList();

    static Mat ImageToCannyed(const Mat &img, double ht, double lt, int aSize, bool l2);
    static QImage convertMat2QImage(const cv::Mat &src);
    static QPixmap convertMat2QPixmap(const cv::Mat &src);
    static Mat convertQImage2Mat(const QImage &qimg);

private:
    Mat rawImage;//original image
    ccStruct cropCalibre;
    Mat preprocessedImage;//pre-processed image
    Mat edges;
    Mat processedImage;//image ready to be analysed (shed colour already)
    bool m_isCropped;
    bool m_hasEdges;
    bool m_isPreProcessed;//with prefix "m_" to indicate an object instead of a function
    bool m_isProcessed;
    bool m_isAnalysed;

    /*
     * analysis data are defined below
     */
    QStringList infoList;//information displayed on the info text browser

};

}

#endif // IMAGE_H
