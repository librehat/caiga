#ifndef QIMAGEDRAWER_H
#define QIMAGEDRAWER_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include "ccspace.h"
#include "zoomer.h"
using namespace CAIGA;

class QImageDrawer : public QWidget
{
    Q_OBJECT
public:
    explicit QImageDrawer(QWidget *parent = 0);
    inline const QImage *image() const { return &m_image; }
    inline void setSpace(CCSpace *s) { ccSpace = s; connect(s, &CCSpace::scaleValueChanged, this, &QImageDrawer::calibreFinished); }
    inline void setDrawMode(int m) { m_drawMode = m; update(); }
    inline void setPenColour(const QString &c)
    {
        QColor colour(c);
        if (colour.isValid()) {
            m_penColour = colour;
        }
    }
    inline QColor getPenColour() { return m_penColour; }
    inline void reset() { ccSpace->reset(); update(); }

public slots:
    inline void setImage(QImage img) { m_image = img; findGoodEnoughZoom(); update(); }

signals:
    void calibreFinished(qreal);
    void gaugeLineResult(qreal);

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);

private:
    QColor m_penColour;
    QImage m_image;

    CCSpace *ccSpace;

    int m_drawMode;
    QPointF m_mousePressed;
    QPointF m_mouseReleased;
    QLineF m_calibreLine;
    QLineF m_gaugeLine;

    Zoomer m_zoomer;

    void findGoodEnoughZoom();

private slots:
    void onZoomChanged(bool in);//zoom in (larger)
};

#endif // QIMAGEDRAWER_H
