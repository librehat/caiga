#ifndef QIMAGEINTERACTIVEDRAWER_H
#define QIMAGEINTERACTIVEDRAWER_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include "zoomer.h"

class QImageInteractiveDrawer : public QWidget
{
    Q_OBJECT
public:
    enum DRAW_MODE{ NONE, PENCIL, ERASER };
    explicit QImageInteractiveDrawer(QWidget *parent = 0);
    inline const QImage *image() const { return &m_image; }
    inline void setWhite(bool w = true) { m_white = w; m_penColour = w ? QColor(255, 255, 255) : QColor(0, 0, 0); }
    inline void setPenColour(const QColor &pc) { m_penColour = pc; }
    inline bool isWhite() { return m_penColour == QColor(255, 255, 255); }
    void setDrawMode(DRAW_MODE);
    inline DRAW_MODE getDrawMode() const { return m_drawMode; }

signals:
    void mousePressed(QPointF);
    void mouseMoved(QPointF);
    void mouseReleased(const QVector<QPointF> &);
    void zoomUpdated(qreal);

public slots:
    inline void setImage(const QImage &img) { m_image = img; firstTimeShow = true; update(); }

private:
    QImage m_image;
    QPointF m_pressed;
    QVector<QPointF> m_movePoints;
    QPointF m_released;
    QPolygonF m_poly;
    bool m_white;
    QColor m_penColour;
    DRAW_MODE m_drawMode;
    QTransform transformer;
    Zoomer m_zoomer;
    bool firstTimeShow;

    void findGoodEnoughZoom();

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);
};

#endif // QIMAGEINTERACTIVEDRAWER_H
