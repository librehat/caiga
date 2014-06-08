#include "zoomer.h"

const QList<qreal> Zoomer::positiveZoomList = QList<qreal>() << 1.0 << 1.25 << 2 << 2.5 << 4 << 5 << 8 << 10 << 16 << 20 << 32 << 40 << 50 << 64 << 80 << 100;

const QList<qreal> Zoomer::negativeZoomList = QList<qreal>() << 0.8 << 0.5 << 0.4 << 0.25 << 0.2 << 0.125 << 0.1 << 0.0625 << 0.05 << 0.03125 << 0.025 << 0.02 << 0.015625 << 0.0125 << 0.01;

Zoomer::Zoomer()
{
    m_level = 0;
}

bool Zoomer::zoomIn()
{
    if (m_level < positiveZoomList.count() - 1) {
        ++m_level;
        return true;
    }
    else {
        return false;
    }
}

bool Zoomer::zoomOut()
{
    if (abs(m_level) < negativeZoomList.count() || m_level >= 0) {
        --m_level;
        return true;
    }
    else {
        return false;
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

void Zoomer::setZoom(qreal z)
{
    int p = -1;
    if (z >= 0) {
        p = positiveZoomList.indexOf(z);
        m_level = p >= 0 ? p : 0;
    }
    else {
        p = negativeZoomList.indexOf(z);
        m_level = p >= 0 ? -(p + 1) : 0;
    }
}

void Zoomer::adjustToNear(qreal scale)
{
    if (scale < 1.0) {
        for (int i = 0; i < negativeZoomList.count(); ++i) {
            if (negativeZoomList.at(i) < scale) {
                m_level = -(i + 1);//minus 1 (so the abs(m_level) plus 1)
                return;
            }
        }
    }
    else {
        for (int i = 0; i < positiveZoomList.count(); ++i) {
            if (positiveZoomList.at(i) > scale) {
                m_level = i;
                return;
            }
        }
    }
}
