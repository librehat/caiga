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
    inline const QImage *image() const { return &m_image; }
    inline ccStruct getCCStruct() { return m_value; }
    inline void setDrawMode(int m)//QButtonGroup id start with -2
    {
        m_value.drawMode = m;
        switch (m) {
        case -2://circle
            m_value.isCircle = true;
            break;
        case -3://rect
            m_value.isCircle = false;
            break;
        case -4://calibre
        case -5://gauge
            break;
        }
    }
    inline void setPenColour(const QString &c)
    {
        QColor colour(c);
        if (colour.isValid()) {
            m_penColour = colour;
        }
    }
    inline QColor getPenColour() { return m_penColour; }
    inline bool isCircle() { return m_value.isCircle; }
    inline void reset() { m_value = ccStruct(); update(); }
    inline void restoreState(ccStruct c) { m_value = c; update(); }

public slots:
    inline void setImage(const QImage &img) { m_image = img; setMinimumSize(m_image.size()); update(); }
    inline void setScaleValue(qreal s) { m_value.calibre = s ;}

signals:
    void calibreFinished(qreal);
    void gaugeLineResult(qreal);

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

private:
    QColor m_penColour;
    QImage m_image;

    ccStruct m_value;

    QLine m_gaugeLine;
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
