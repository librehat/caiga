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
    void setNotLarger(bool n = false);

public slots:
    void setPixmap(const QPixmap &pix);
    void setImage(const QImage &qimg);

protected:
    void paintEvent(QPaintEvent *);

private:
    QPixmap m_pixmap;
    bool m_notLarger;//not larger than original one. the scale ratio will be always <= 1.0
};

#endif // QIMAGEVIEWER_H
