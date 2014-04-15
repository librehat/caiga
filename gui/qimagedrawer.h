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
    void setDrawMode(int);//-2: circle; -3: rect; -4: calibre (QButtonGroup id start with -2)
    void setPenColour(const QString &);

public slots:
    void setPixmap(const QPixmap &pix);

signals:
    void calibreFinished(int, double);

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

private:
    int m_drawMode;
    QColor m_penColour;
    QPixmap m_origPixmap;
    QPixmap m_croppedPixmap;
    QPoint m_mousePressed;
    QPoint m_mouseReleased;

    QPoint m_drawedCircleCentre;
    int m_drawedCircleRadius;
    QRect m_drawedRect;
    QLine m_drawedCalibre;
    double m_calibreRealSize;
};

#endif // QIMAGEDRAWER_H
