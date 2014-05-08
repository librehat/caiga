#ifndef QIMAGEINTERACTIVEDRAWER_H
#define QIMAGEINTERACTIVEDRAWER_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>

class QImageInteractiveDrawer : public QWidget
{
    Q_OBJECT
public:
    enum DRAW_MODE{ NONE, POLY_LINE, CIRCLE_LINE };
    explicit QImageInteractiveDrawer(QWidget *parent = 0);
    const QImage *image() const;
    void setWhite(bool = true);
    bool isWhite();
    void setDrawMode(DRAW_MODE);

signals:
    void mousePressed(QPoint);
    void mouseMoved(QPoint);
    void mouseReleased(QVector<QPoint>);

public slots:
    void setImage(const QImage &);

private:
    QImage m_image;
    QImage m_scaledImage;
    qreal m_scale;
    QPoint m_pressed;
    QVector<QPoint> m_movePoints;
    QPoint m_released;
    QPolygon m_poly;
    bool m_white;
    QColor m_penColour;
    DRAW_MODE m_drawMode;

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
};

#endif // QIMAGEINTERACTIVEDRAWER_H
