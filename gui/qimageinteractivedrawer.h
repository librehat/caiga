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
    explicit QImageInteractiveDrawer(QWidget *parent = 0);
    const QImage *image() const;

signals:
    void mousePressed(int, int);
    void mouseMoved(int, int);

public slots:
    void setImage(const QImage &);

private:
    QImage m_image;
    QImage m_scaledImage;
    qreal m_scale;

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
};

#endif // QIMAGEINTERACTIVEDRAWER_H
