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
    QPoint topleft;
    topleft.setX((this->width() - m_image.width()) / 2);
    topleft.setY((this->height() - m_image.height()) / 2);
    painter.drawImage(topleft, m_image);

    //Draw
    QPen pen(m_penColour, 2, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
    painter.setPen(pen);

    //don't paint if user didn't draw
    if (ccSpace->qrect.width() < 2 && m_mousePressed.manhattanLength() < 2) {
        return;
    }

    if (m_drawMode == -3) {
        QPoint p1(m_calibreLine.p1().x() + (this->width() - m_image.width()) / 2,
                  m_calibreLine.p1().y() + (this->height() - m_image.height()) / 2);
        QPoint p2(m_calibreLine.p2().x() + (this->width() - m_image.width()) / 2,
                  m_calibreLine.p2().y() + (this->height() - m_image.height()) / 2);
        painter.drawLine(p1, p2);
        return;
    }
    else if (m_drawMode == -4)
    {
        QPoint p1(m_gaugeLine.p1().x() + (this->width() - m_image.width()) / 2,
                  m_gaugeLine.p1().y() + (this->height() - m_image.height()) / 2);
        QPoint p2(m_gaugeLine.p2().x() + (this->width() - m_image.width()) / 2,
                  m_gaugeLine.p2().y() + (this->height() - m_image.height()) / 2);
        painter.drawLine(p1, p2);
        return;
    }

    int tlx = ccSpace->qrect.topLeft().x() + (this->width() - m_image.width()) / 2;
    int tly = ccSpace->qrect.topLeft().y() + (this->height() - m_image.height()) / 2;
    QRect pRect(tlx, tly, ccSpace->qrect.width(), ccSpace->qrect.height());
    painter.drawRect(pRect);
}

void QImageDrawer::mousePressEvent(QMouseEvent *m)
{
    QWidget::mousePressEvent(m);
    if (m_image.isNull()) {
        return;
    }

    QPoint margin((this->width() - m_image.width()) / 2, (this->height() - m_image.height()) / 2);

    m_mousePressed = m->pos() - margin;

    if (m_mousePressed.x() < 0) {
        m_mousePressed.setX(0);
    }
    else if (m_mousePressed.x() > m_image.width() - 1) {
        m_mousePressed.setX(m_image.width() - 1);
    }
    if (m_mousePressed.y() < 0) {
        m_mousePressed.setY(0);
    }
    else if (m_mousePressed.y() > m_image.height() - 1) {
        m_mousePressed.setY(m_image.height() - 1);
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

    QPoint margin((this->width() - m_image.width()) / 2, (this->height() - m_image.height()) / 2);
    m_mouseReleased = m->pos() - margin;

    if (m_mouseReleased.x() < 0) {
        m_mouseReleased.setX(0);
    }
    else if (m_mouseReleased.x() > m_image.width() - 1) {
        m_mouseReleased.setX(m_image.width() - 1);
    }
    if (m_mouseReleased.y() < 0) {
        m_mouseReleased.setY(0);
    }
    else if (m_mouseReleased.y() > m_image.height() - 1) {
        m_mouseReleased.setY(m_image.height() - 1);
    }

    if (m_drawMode == -4) {
        m_gaugeLine.setP2(m_mouseReleased);
    }else if (m_drawMode == -2) {//rectangle
        ccSpace->qrect = QRect(m_mousePressed, m_mouseReleased);
    }
    else if (m_drawMode == -3) {//calibre
        m_calibreLine = QLine(m_mousePressed, m_mouseReleased);
        if (std::abs(m_calibreLine.dx()) > std::abs(m_calibreLine.dy())) {
            m_calibreLine.setP2(QPoint(m_mouseReleased.x(), m_mousePressed.y()));
        }
        else {
            m_calibreLine.setP2(QPoint(m_mousePressed.x(), m_mouseReleased.y()));
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
        qreal r = static_cast<qreal>(QInputDialog::getDouble(this, "Input the real size", "Unit: μm", 0, 0, 9999, 4, &ok));
        if (ok) {
            qreal scale = std::max(std::abs(m_calibreLine.dx()), std::abs(m_calibreLine.dy())) / r;
            ccSpace->setScaleValue(scale);
        }
    }
    else if (m_drawMode == -4) {//gauge
        qreal gaugeResult = qSqrt(m_gaugeLine.dx() * m_gaugeLine.dx() + m_gaugeLine.dy() * m_gaugeLine.dy()) / ccSpace->getScaleValue();
        emit gaugeLineResult(gaugeResult);
    }
}
