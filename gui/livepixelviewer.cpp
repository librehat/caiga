#include <QTimerEvent>
#include <QPainter>
#include <QScreen>
#include <QApplication>
#include <QDesktopWidget>
#include "livepixelviewer.h"

LivePixelViewer::LivePixelViewer(QWidget *parent) :
    QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setMouseTracking(true);
    m_updateId = startTimer(50);
    m_live = true;
}

void LivePixelViewer::setLivePreviewEnabled(bool l)
{
    m_live = l;
}

void LivePixelViewer::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_updateId) {
        grabScreen();
    }
}

void LivePixelViewer::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    const int w = width();
    const int h = height();

    if (m_bufferPix.isNull()) {
        return;
    }
    p.scale(4, 4);
    p.drawPixmap(0, 0, m_bufferPix);
    p.scale(0.25, 0.25);

    p.setPen(Qt::black);
    for (int x = 0; x < w; x += 4) {
        p.drawLine(x, 0, x, h);
    }
    for (int y = 0; y < h; y += 4) {
        p.drawLine(0, y, w, y);
    }
}

void LivePixelViewer::grabScreen()
{
    QPoint mousePos = QCursor::pos();
    if (mousePos == m_lastMousePos || !m_live) {
        return;
    }

    int w = width() / 4;
    int h = height() / 4;

    if (width() % 4 > 0) {
        ++w;
    }
    if (height() % 4 > 0) {
        ++h;
    }

    int x = mousePos.x() - w / 2;
    int y = mousePos.y() - h / 2;

    QScreen *screen = qApp->screens().at(qApp->desktop()->screenNumber());
    m_bufferPix = screen->grabWindow(qApp->desktop()->winId(), x, y, w, h);

    update();

    m_lastMousePos = mousePos;
}
