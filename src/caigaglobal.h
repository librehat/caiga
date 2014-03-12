/*
 * This is the global header file for CAIGA project.
 * Please implement it carefully!
 * Please don't define classes in this file.
 */

#ifndef CAIGAGLOBAL_H
#define CAIGAGLOBAL_H

#include <QString>
#include <QDir>
/*
 * Include OpenCV stuffs
 */
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;

namespace CAIGA {
/*
 * Define global variables here
 */

const QString tempImg = QDir::tempPath() + QString("/CAIGA_Cam.jpeg");

/*
 * Define inline global functions here
 */

inline void setCurrentDir(const QString &filePath)
{
    int trimIndex = filePath.lastIndexOf('/');
    if (trimIndex != -1)
    {
        QDir::setCurrent(filePath.left(trimIndex));
    }
}

}//END OF CAIGA NAMESPACE
#endif // CAIGAGLOBAL_H
