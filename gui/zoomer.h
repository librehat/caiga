#ifndef ZOOMER_H
#define ZOOMER_H

#include <QtCore>

class Zoomer
{
public:
    Zoomer();
    void zoomIn();
    void zoomOut();
    qreal getZoom();

private:
    int m_level;
    const static QList<qreal> positiveZoomList;
    const static QList<qreal> negativeZoomList;
};

#endif // ZOOMER_H
