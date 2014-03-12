#ifndef QIMAGEVIEWER_H
#define QIMAGEVIEWER_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>

class QImageViewer : public QWidget
{
    Q_OBJECT
public:
    explicit QImageViewer(QWidget *parent = 0);
    const QPixmap *pixmap() const;

public slots:
    void setPixmap(const QPixmap &pix);

protected:
    void paintEvent(QPaintEvent *);

private:
    QPixmap m_pixmap;
};

#endif // QIMAGEVIEWER_H
