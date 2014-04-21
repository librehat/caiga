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
    void mouseClickedFinished(QPoint);

public slots:
    void setImage(const QImage &);

private:
    QImage m_image;
    QPoint m_mousePressed;//relevant to m_image

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
};

#endif // QIMAGEINTERACTIVEDRAWER_H
