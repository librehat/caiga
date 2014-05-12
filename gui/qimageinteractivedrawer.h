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
    enum DRAW_MODE{ NONE, PENCIL, ERASER };
    explicit QImageInteractiveDrawer(QWidget *parent = 0);
    inline const QImage *image() const { return &m_image; }
    inline void setWhite(bool w = true) { m_white = w; m_penColour = w ? QColor(255, 255, 255) : QColor(0, 0, 0); }
    inline void setPenColour(const QColor &pc) { m_penColour = pc; }
    inline bool isWhite() { return m_penColour == QColor(255, 255, 255); }
    void setDrawMode(DRAW_MODE);

signals:
    void mousePressed(QPoint);
    void mouseMoved(QPoint);
    void mouseReleased(const QVector<QPoint> &);

public slots:
    inline void setImage(const QImage &img) { m_image = img; update(); }

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
