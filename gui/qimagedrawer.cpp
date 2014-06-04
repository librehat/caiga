#include <QWheelEvent>
#include <QDebug>
#include <qmath.h>
#include <QInputDialog>
#include "qimagedrawer.h"

QImageDrawer::QImageDrawer(QWidget *parent) :
    QWidget(parent)
{
    m_penColour = QColor(255, 0, 0);//Red
    m_drawMode = -2;
    m_mousePressed = QPoint(0, 0);
    firstTimeShow = true;
}

void QImageDrawer::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    if (m_image.isNull())
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    if (firstTimeShow) {
        resetZoom();
        firstTimeShow = false;
    }
    qreal zoom = m_zoomer.getZoom();
    QSizeF pixSize = m_image.size() * zoom;
    /*
     * use QTransform instead of window-viewport conversion
     * beacause we can't retrieve a transform with scale from QPainter after setWindow and setViewport
     * don't know why would that happen. maybe that's a Qt bug?
     * anyway, just stick to QTransform for now.
     */
    ccSpace->setTransform(QTransform(zoom, 0, 0, 0, zoom, 0, (this->width() - pixSize.width()) / 2.0, (this->height() - pixSize.height()) / 2.0, 1.0));
    painter.setWorldTransform(ccSpace->getTransform());
    painter.drawImage(0, 0, m_image);

    //Draw
    QPen pen(m_penColour, 2, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
    painter.setPen(pen);

    //don't paint if user didn't draw
    if (ccSpace->qrect.width() < 2 && m_mousePressed.manhattanLength() < 2) {
        return;
    }

    if (m_drawMode == -3) {
        painter.drawLine(m_calibreLine);
        return;
    }
    else if (m_drawMode == -4)
    {
        painter.drawLine(m_gaugeLine);
        return;
    }

    painter.drawRect(ccSpace->qrect);
}

void QImageDrawer::mousePressEvent(QMouseEvent *m)
{
    QWidget::mousePressEvent(m);
    if (m_image.isNull()) {
        return;
    }

    m_mousePressed = ccSpace->getInvertedTransform().map(m->localPos());

    QPointF tl(0, 0);
    QPointF br(m_image.width() - 1, m_image.height() - 1);

    if (m_mousePressed.x() < tl.x()) {
        m_mousePressed.setX(tl.x());
    }
    else if (m_mousePressed.x() > br.x()) {
        m_mousePressed.setX(br.x());
    }

    if (m_mousePressed.y() < tl.y()) {
        m_mousePressed.setY(tl.y());
    }
    else if (m_mousePressed.y() > br.y()) {
        m_mousePressed.setY(br.y());
    }

    if (m_drawMode == -4) {
        m_gaugeLine.setP1(m_mousePressed);
    }
}

void QImageDrawer::mouseMoveEvent(QMouseEvent *m)
{
    QWidget::mouseMoveEvent(m);
    if (m_image.isNull()) {
        return;
    }

    m_mouseReleased = ccSpace->getInvertedTransform().map(m->localPos());
    QPointF tl(0, 0);
    QPointF br(m_image.width() - 1, m_image.height() - 1);

    if (m_mouseReleased.x() < tl.x()) {
        m_mouseReleased.setX(tl.x());
    }
    else if (m_mouseReleased.x() > br.x()) {
        m_mouseReleased.setX(br.x());
    }
    if (m_mouseReleased.y() < tl.y()) {
        m_mouseReleased.setY(tl.y());
    }
    else if (m_mouseReleased.y() > br.y()) {
        m_mouseReleased.setY(br.y());
    }

    if (m_drawMode == -4) {
        m_gaugeLine.setP2(m_mouseReleased);
    }else if (m_drawMode == -2) {//rectangle
        ccSpace->setRectangle(QRectF(m_mousePressed, m_mouseReleased));
    }
    else if (m_drawMode == -3) {//calibre
        m_calibreLine = QLineF(m_mousePressed, m_mouseReleased);
        if (qFabs(m_calibreLine.dx()) > qFabs(m_calibreLine.dy())) {
            m_calibreLine.setP2(QPointF(m_mouseReleased.x(), m_mousePressed.y()));
        }
        else {
            m_calibreLine.setP2(QPointF(m_mousePressed.x(), m_mouseReleased.y()));
        }
    }

    this->update();
}

void QImageDrawer::mouseReleaseEvent(QMouseEvent *m)
{
    QWidget::mouseReleaseEvent(m);
    if (m_image.isNull()) {
        return;
    }
    if (m_drawMode == -3) {//calibre, we should pop up a dialog and get the real size
        bool ok;
        qreal r = static_cast<qreal>(QInputDialog::getDouble(this, tr("Input the real size"), tr("Unit: μm"), 0, 0, 9999, 4, &ok));
        if (ok) {
            ccSpace->setScaleValue(qMax(qFabs(m_calibreLine.dx()), qFabs(m_calibreLine.dy())), r);
        }
    }
    else if (m_drawMode == -4) {//gauge
        qreal gaugeResult = qSqrt(m_gaugeLine.dx() * m_gaugeLine.dx() + m_gaugeLine.dy() * m_gaugeLine.dy()) / ccSpace->getScaleValue();
        emit gaugeLineResult(gaugeResult);
    }
}

void QImageDrawer::wheelEvent(QWheelEvent *we)
{
    QWidget::wheelEvent(we);
    if(we->angleDelta().y() > 0) {
        m_zoomer.zoomIn();
    }
    else {
        m_zoomer.zoomOut();
    }
    handleSizeChanged(m_zoomer.getZoom());
    update();
}

void QImageDrawer::resetZoom()
{
    m_zoomer.setZoom(1.0);
    handleSizeChanged(m_zoomer.getZoom());
}

void QImageDrawer::handleSizeChanged(qreal zoom)
{
    emit zoomUpdated(zoom);
    setMinimumSize(m_image.size() * zoom);
}
