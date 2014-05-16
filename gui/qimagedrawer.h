#ifndef QIMAGEDRAWER_H
#define QIMAGEDRAWER_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include "ccspace.h"
using namespace CAIGA;

class QImageDrawer : public QWidget
{
    Q_OBJECT
public:
    explicit QImageDrawer(QWidget *parent = 0);
    inline const QImage *image() const { return &m_image; }
    inline void setSpace(CCSpace *s) { ccSpace = s; }
    inline void setDrawMode(int m)//QButtonGroup id start with -2
    {
        m_drawMode = m;
        switch (m) {
        case -2://circle
            ccSpace->setCircle(true);
            break;
        case -3://rect
            ccSpace->setCircle(false);
            break;
        case -4://calibre
        case -5://gauge
            break;
        }
    }
    inline void setPenColour(const QString &c)
    {
        QColor colour(c);
        if (colour.isValid()) {
            m_penColour = colour;
        }
    }
    inline QColor getPenColour() { return m_penColour; }
    inline bool isCircle() { return ccSpace->getIsCircle(); }
    inline void reset() { ccSpace->reset(); update(); }

public slots:
    inline void setImage(QImage img) { m_image = img; setMinimumSize(m_image.size()); update(); }

signals:
    void calibreFinished(qreal);
    void gaugeLineResult(qreal);

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

private:
    QColor m_penColour;
    QImage m_image;

    CCSpace *ccSpace;

    int m_drawMode;
    QPoint m_mousePressed;
    QPoint m_mouseReleased;
    QLine m_calibreLine;
    QLine m_gaugeLine;
};

#endif // QIMAGEDRAWER_H
