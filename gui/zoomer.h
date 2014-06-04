#ifndef ZOOMER_H
#define ZOOMER_H

#include <QtCore>

class Zoomer
{
public:
    Zoomer();
    bool zoomIn();
    bool zoomOut();
    qreal getZoom();
    void setZoom(qreal z);
    void adjustToNear(qreal scale);

private:
    int m_level;
    const static QList<qreal> positiveZoomList;
    const static QList<qreal> negativeZoomList;
};

#endif // ZOOMER_H
