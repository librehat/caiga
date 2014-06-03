#include "zoomer.h"

const QList<qreal> Zoomer::positiveZoomList = QList<qreal>() << 1.0 << 1.25 << 2 << 2.5 << 4 << 5 << 8 << 10 << 20 << 32 << 50 << 100 << 500;

const QList<qreal> Zoomer::negativeZoomList = QList<qreal>() << 0.8 << 0.5 << 0.4 << 0.25 << 0.2 << 0.125 << 0.1 << 0.05 << 0.03125 << 0.02 << 0.01 << 0.002;

Zoomer::Zoomer()
{
    m_level = 0;
}

void Zoomer::zoomIn()
{
    if (m_level < positiveZoomList.count() - 1) {
        ++m_level;
    }
}

void Zoomer::zoomOut()
{
    if (abs(m_level) < negativeZoomList.count() || m_level >= 0) {
        --m_level;
    }
}

qreal Zoomer::getZoom()
{
    if (m_level < 0) {
        int nI = abs(m_level);
        return negativeZoomList.at(nI - 1);
    }
    else {
        return positiveZoomList.at(m_level);
    }
}
