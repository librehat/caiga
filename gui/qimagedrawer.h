#ifndef QIMAGEDRAWER_H
#define QIMAGEDRAWER_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include "calibrespace.h"
#include "zoomer.h"
using namespace CAIGA;

class QImageDrawer : public QWidget
{
    Q_OBJECT
public:
    explicit QImageDrawer(QWidget *parent = 0);
    inline const QImage *image() const { return &m_image; }
    inline void setSpace(CalibreSpace *s) { ccSpace = s; connect(s, &CalibreSpace::scaleValueChanged, this, &QImageDrawer::calibreFinished); }
    inline void setDrawMode(int m) { m_drawMode = m; update(); }
    inline void setPenColour(const QString &c)
    {
        QColor colour(c);
        if (colour.isValid()) {
            m_penColour = colour;
        }
    }
    inline QColor getPenColour() { return m_penColour; }
    inline void reset() { ccSpace->reset(); update(); }
    inline qreal getZoom() { return m_zoomer.getZoom(); }

public slots:
    inline void setImage(QImage img, bool needRefreshZoom = false) { m_image = img; firstTimeShow = needRefreshZoom || firstTimeShow; update(); }

signals:
    void calibreFinished(qreal);
    void gaugeLineResult(qreal);
    void zoomUpdated(qreal);

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);

private:
    QColor m_penColour;
    QImage m_image;

    CalibreSpace *ccSpace;

    int m_drawMode;
    QPointF m_mousePressed;
    QPointF m_mouseReleased;
    QLineF m_calibreLine;
    QLineF m_gaugeLine;

    bool firstTimeShow;
    Zoomer m_zoomer;

    void resetZoom();
    void handleSizeChanged(qreal zoom);
};

#endif // QIMAGEDRAWER_H
