#include "qimagedrawer.h"
#include <QDebug>
#include <qmath.h>
#include <QInputDialog>

QImageDrawer::QImageDrawer(QWidget *parent) :
    QWidget(parent)
{
    m_penColour = QColor(255, 0, 0);//Red
    m_drawMode = -2;
    m_mousePressed = QPoint(0, 0);
}

void QImageDrawer::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    if (m_image.isNull())
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    //using QTransformer to ease the work
    ccSpace->transformer.setMatrix(1.0, 0, 0, 0, 1.0, 0, (this->width() - m_image.width()) / 2, (this->height() - m_image.height()) / 2, 1.0);
    painter.setTransform(ccSpace->transformer);
    painter.drawImage(0, 0, m_image);
    painter.setTransform(QTransform());

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

    m_mousePressed = m->localPos();
    QPointF tl = ccSpace->transformer.map(QPointF(0, 0));
    QPointF br = ccSpace->transformer.map(QPointF(m_image.width() - 1, m_image.height() - 1));

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

    if (m_drawMode == -5) {
        m_gaugeLine.setP1(m_mousePressed);
    }
}

void QImageDrawer::mouseMoveEvent(QMouseEvent *m)
{
    QWidget::mouseMoveEvent(m);
    if (m_image.isNull()) {
        return;
    }

    m_mouseReleased = m->localPos();
    QPointF tl = ccSpace->transformer.map(QPointF(0, 0));
    QPointF br = ccSpace->transformer.map(QPointF(m_image.width() - 1, m_image.height() - 1));

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
            qreal scale = qMax(qFabs(m_calibreLine.dx()), qFabs(m_calibreLine.dy())) / r;
            ccSpace->setScaleValue(scale);
        }
    }
    else if (m_drawMode == -4) {//gauge
        qreal gaugeResult = qSqrt(m_gaugeLine.dx() * m_gaugeLine.dx() + m_gaugeLine.dy() * m_gaugeLine.dy()) / ccSpace->getScaleValue();
        emit gaugeLineResult(gaugeResult);
    }
}
