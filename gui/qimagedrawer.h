#ifndef QIMAGEDRAWER_H
#define QIMAGEDRAWER_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>

class QImageDrawer : public QWidget
{
    Q_OBJECT
public:
    explicit QImageDrawer(QWidget *parent = 0);
    const QPixmap *getOrigPixmap() const;
    const QPixmap *getCroppedPixmap() const;
    void setDrawCircle(bool circle = true);

public slots:
    void setPixmap(const QPixmap &pix);

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

private:
    QPixmap m_origPixmap;
    QPixmap m_croppedPixmap;
    QPoint m_mousePressed;
    QPoint m_mouseReleased;
    bool m_circle;
};

#endif // QIMAGEDRAWER_H
