#include <QFile>
#include <QDebug>
#include "macro.h"
using namespace CAIGA;

Macro::Macro(QObject *parent) :
    QObject(parent)
{
    m_space = NULL;
    m_ccSpace = NULL;
}

void Macro::doCropAndCalibreMacroFromFile(const QString &f)
{
    if (m_ccSpace == NULL) {
        qWarning() << "Error. CCSpace pointer is NULL.";
        return;
    }

    /*
     * file content shoud be
     *
     * c;100,100;50;5.90
     *
     * or
     *
     * r;0,0;300,200;5.90
     *
     * the first character means the crop opeartion, (c)ircle or (r)ectangle
     * the second value is the circle centre or the top left point's coordinate
     * the third one is the radius of circle, or the bottom right point coordinate
     * finally the last value has a type of qreal (aka double), which is the scale
     *
     * blank lines or those start with # will be ignored.
     *
     * any error in the macro file would cause this operation fail.
     */

    QFile macro(f);
    macro.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!macro.isReadable()) {
        qWarning() << "Error. Could not read from macro file.";
        return;
    }

    bool ongoing = true;
    do {
        QString line(macro.readLine());
        QStringList lineList = line.split(';');
        if (lineList.count() != 4) {
            continue;
        }
        else {
            if (lineList[0] == "c") {
                m_ccSpace->setCircle(true);
                QStringList centre = lineList[1].split(',');
                if (centre.count() != 2) {
                    continue;
                }
                else {
                    m_ccSpace->setCircleCentre(QPoint(centre[0].toInt(), centre[1].toInt()));
                }
                m_ccSpace->setCircleRadius(lineList[2].toInt());
                m_ccSpace->setScaleValue(lineList[3].toDouble());
                m_ccSpace->cropImage();
                ongoing = false;
            }
            else if (lineList[0] == "r") {
                m_ccSpace->setCircle(false);
                QStringList topleft = lineList[1].split(',');
                QStringList bottomright = lineList[2].split(',');
                if (topleft.count() != 2 || bottomright.count() != 2) {
                    continue;
                }
                else {
                    QRect r(QPoint(topleft[0].toInt(), topleft[1].toInt()), QPoint(bottomright[0].toInt(), bottomright[1].toInt()));
                    m_ccSpace->setRectangle(r);
                }
                m_ccSpace->setScaleValue(lineList[3].toDouble());
                m_ccSpace->cropImage();
                ongoing = false;
            }
            else {
                continue;
            }
        }
    } while (ongoing);
}

void Macro::doWorkMacroFromFile(const QString &)
{
    //TODO
}
