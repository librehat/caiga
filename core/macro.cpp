#include <QFile>
#include <QDebug>
#include "macro.h"
using namespace CAIGA;

Macro::Macro(CalibreSpace *c, QObject *parent) :
    QObject(parent)
{
    m_calibreSpace = c;
}

void Macro::doCropAndCalibreMacroFromFile(const QString &f)
{
    if (m_calibreSpace == NULL) {
        qWarning() << "Error. CCSpace pointer is NULL.";
        return;
    }

    /*
     * file content shoud be
     *
     * r;0,0;300,200;5.90
     *
     * the first character means the opeartion, (r)ectangle
     * the second value is the top left point's coordinate
     * the third one is the bottom right point coordinate
     * finally the last value has a type of qreal (aka double), which is the scale
     *
     * blank lines or those start with # will be ignored.
     * only the first valid line will make effects.
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
            if (lineList[0] == "r") {
                QStringList topleft = lineList[1].split(',');
                QStringList bottomright = lineList[2].split(',');
                if (topleft.count() != 2 || bottomright.count() != 2) {
                    continue;
                }
                else {
                    QRect r(QPoint(topleft[0].toInt(), topleft[1].toInt()), QPoint(bottomright[0].toInt(), bottomright[1].toInt()));
                    m_calibreSpace->setRectangle(r);
                }
                m_calibreSpace->setScaleValue(lineList[3].toDouble());
                m_calibreSpace->cropImage();
                ongoing = false;
            }
            else {
                continue;
            }
        }
    } while (ongoing);
}

void Macro::saveCropAndCalibreAsMacroFile(const QString &f)
{
    if (m_calibreSpace == NULL) {
        qWarning() << "Error. CCSpace pointer is NULL.";
        return;
    }

    QFile macroFile(f);
    macroFile.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!macroFile.isWritable()) {
        qWarning() << "Error. Cannot write to macro file. It may not be saved.";
        return;
    }

    QByteArray arrayToBeWritten;
    arrayToBeWritten.append("r;");
    QString topleft = QString::number(m_calibreSpace->qrect.topLeft().x()) + "," + QString::number(m_calibreSpace->qrect.topLeft().y()) + ";";
    QString bottomright = QString::number(m_calibreSpace->qrect.bottomRight().x()) + "," + QString::number(m_calibreSpace->qrect.bottomRight().y()) + QString(";");
    arrayToBeWritten.append(topleft);
    arrayToBeWritten.append(bottomright);
    arrayToBeWritten.append(QString::number(m_calibreSpace->getScaleValue()));
    arrayToBeWritten.append('\n');
    macroFile.write(arrayToBeWritten);
}
