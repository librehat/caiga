#include "zoomer.h"

const QList<qreal> Zoomer::positiveZoomList = QList<qreal>() << 1.0 << 1.25 << 2 << 2.5 << 4 << 5 << 8 << 10 << 20 << 32 << 50 << 100 << 500;

const QList<qreal> Zoomer::negativeZoomList = QList<qreal>() << 0.8 << 0.5 << 0.4 << 0.25 << 0.2 << 0.125 << 0.1 << 0.05 << 0.03125 << 0.02 << 0.01 << 0.002;

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
                m_level = i;//considerint the index starting from 0. we don't need to minus 1 here.
                return;
            }
        }
    }
}
