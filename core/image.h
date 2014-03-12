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
// store QImage instead of Mat or QPixmap
class CORE_LIB Image
{
public:
    Image();
    Image(QImage img);
    Image(Mat &matImg);
    Image(const QString &imgfile);
    Image(const QByteArray &orig, QByteArray *pre, QByteArray *pro, QStringList *info);
    ~Image();
    QImage getOrigImage();
    void setOrigImage(QImage img);
    QImage getProcessedImage();
    void setPreProcessedImage(QImage img);
    void setProcessedImage(QImage img);
    bool isPreProcessed();
    bool isProcessed();
    bool isAnalysed();
    QStringList getInfoList();

    static QImage convertMat2QImage(const cv::Mat &src);
    static QPixmap convertMat2QPixmap(const cv::Mat &src);
    static Mat preProcess(Mat &src);
    static Mat process(Mat &src);

private:
    QImage origImage;//original image
    QImage preprocessedImage;//pre-processed image
    QImage processedImage;//image ready to be analysed
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
