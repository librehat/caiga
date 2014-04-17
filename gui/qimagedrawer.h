#ifndef QIMAGEDRAWER_H
#define QIMAGEDRAWER_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include "datastructure.h"
using namespace CAIGA;

class QImageDrawer : public QWidget
{
    Q_OBJECT
public:
    explicit QImageDrawer(QWidget *parent = 0);
    const QImage *image() const;
    //QImage getCroppedImage();
    ccStruct getCCStruct();
    void setDrawMode(int);//-2: circle; -3: rect; -4: calibre (QButtonGroup id start with -2)
    void setPenColour(const QString &);
    bool isCircle();
    void reset();
    void restoreState(ccStruct);

public slots:
    void setImage(const QImage &);

signals:
    void calibreFinished(qreal);

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

private:
    QColor m_penColour;
    QImage m_image;
    QImage m_scaledImage;//this ensures user always crop correct region
    qreal m_scale;//m_scaledImage.size() / m_image.size()

    ccStruct m_value;
    /*
     * replaced with ccStruct
     * all these values are calculated based on original image (m_image)
    QPoint m_mousePressed;
    QPoint m_mouseReleased;
    QPoint m_drawedCircleCentre;
    int m_drawedCircleRadius;
    QRect m_drawedRect;
    QLine m_drawedCalibre;
    qreal m_calibreRealSize;
    */
};

#endif // QIMAGEDRAWER_H
