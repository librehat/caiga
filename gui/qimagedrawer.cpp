#include "qimagedrawer.h"
#include <QDebug>
#include <qmath.h>
#include <QInputDialog>

QImageDrawer::QImageDrawer(QWidget *parent) :
    QWidget(parent)
{
    m_penColour = QColor(255, 0, 0);//Red
    m_drawMode = -2;
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
    if (ccSpace->circleRadius < 2 && ccSpace->qrect.width() < 2) {
        return;
    }

    if (m_drawMode == -4) {
        QPoint p1(m_calibreLine.p1().x() + (this->width() - m_image.width()) / 2,
                  m_calibreLine.p1().y() + (this->height() - m_image.height()) / 2);
        QPoint p2(m_calibreLine.p2().x() + (this->width() - m_image.width()) / 2,
                  m_calibreLine.p2().y() + (this->height() - m_image.height()) / 2);
        painter.drawLine(p1, p2);
        return;
    }
    else if (m_drawMode == -5)
    {
        QPoint p1(m_gaugeLine.p1().x() + (this->width() - m_image.width()) / 2,
                  m_gaugeLine.p1().y() + (this->height() - m_image.height()) / 2);
        QPoint p2(m_gaugeLine.p2().x() + (this->width() - m_image.width()) / 2,
                  m_gaugeLine.p2().y() + (this->height() - m_image.height()) / 2);
        painter.drawLine(p1, p2);
        return;
    }

    if (ccSpace->getIsCircle()) {
        QPoint pCentre(ccSpace->circleCentre.x() + (this->width() - m_image.width()) / 2,
                            ccSpace->circleCentre.y() + (this->height() - m_image.height()) / 2);
        QLine vCross(pCentre.x(), pCentre.y() - 5, pCentre.x(), pCentre.y() + 5);
        QLine hCross(pCentre.x() - 5, pCentre.y(), pCentre.x() + 5, pCentre.y());
        painter.drawEllipse(pCentre, ccSpace->circleRadius, ccSpace->circleRadius);
        painter.drawLine(vCross);
        painter.drawLine(hCross);
    }
    else {
        int tlx = ccSpace->qrect.topLeft().x() + (this->width() - m_image.width()) / 2;
        int tly = ccSpace->qrect.topLeft().y() + (this->height() - m_image.height()) / 2;
        QRect pRect(tlx, tly, ccSpace->qrect.width(), ccSpace->qrect.height());
        painter.drawRect(pRect);
    }

}

void QImageDrawer::mousePressEvent(QMouseEvent *m)
{
    QWidget::mousePressEvent(m);
    if (m_image.isNull()) {
        return;
    }

    QPoint margin((this->width() - m_image.width()) / 2, (this->height() - m_image.height()) / 2);
    QPoint margin_r = margin + QPoint(m_image.width(), m_image.height());

    m_mousePressed.setX(m->pos().x() - margin.x());
    m_mousePressed.setY(m->pos().y() - margin.y());

    if (m->pos().x() < margin.x()) {
        m_mousePressed.setX(0);
    }
    if (m->pos().y() < margin.y()) {
        m_mousePressed.setY(0);
    }
    if (m->pos().x() > margin_r.x()) {
        m_mousePressed.setX(m_image.width());
    }
    if (m->pos().y() > margin_r.y()) {
        m_mousePressed.setY(m_image.height());
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
    QPoint margin_r = margin + QPoint(m_image.width(), m_image.height());

    m_mouseReleased.setX(m->pos().x() - margin.x());
    m_mouseReleased.setY(m->pos().y() - margin.y());

    if (m_drawMode == -2) {
        QPoint delta = m_mousePressed - m_mouseReleased;
        int radius = static_cast<int>(qSqrt(delta.x() * delta.x() + delta.y() * delta.y()));
        int maxRadius = std::min(std::min(m_mousePressed.x(), m_mousePressed.y()),  std::min(m_image.width() - m_mousePressed.x(), m_image.height() - m_mousePressed.y()));
        if (radius > maxRadius) {
            m_mouseReleased.setX(m_mousePressed.x() + maxRadius);
            m_mouseReleased.setY(m_mousePressed.y());
        }
        delta = m_mousePressed - m_mouseReleased;
        ccSpace->circleRadius = static_cast<int>(qSqrt(delta.x() * delta.x() + delta.y() * delta.y()));
        ccSpace->circleCentre = m_mousePressed;
    }
    else {
        if (m->pos().x() < margin.x()) {
            m_mouseReleased.setX(0);
        }
        if (m->pos().y() < margin.y()) {
            m_mouseReleased.setY(0);
        }
        if (m->pos().x() > margin_r.x()) {
            m_mouseReleased.setX(m_image.width());
        }
        if (m->pos().y() > margin_r.y()) {
            m_mouseReleased.setY(m_image.height());
        }
        if (m_drawMode == -5) {
            m_gaugeLine.setP2(m_mouseReleased);
        }
    }

    if (m_drawMode == -3) {//rectangle
        ccSpace->qrect = QRect(m_mousePressed, m_mouseReleased);
    }
    else if (m_drawMode == -4) {//calibre
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
    if (m_drawMode == -4) {//calibre, we should pop up a dialog and get the real size
        bool ok;
        qreal r = static_cast<qreal>(QInputDialog::getDouble(this, "Input the real size", "Unit: μm", 0, 0, 9999, 4, &ok));
        if (ok) {
            qreal scale = std::max(std::abs(m_calibreLine.dx()), std::abs(m_calibreLine.dy())) / r;
            ccSpace->setScaleValue(scale);
        }
    }
    else if (m_drawMode == -5) {//gauge
        qreal gaugeResult = qSqrt(m_gaugeLine.dx() * m_gaugeLine.dx() + m_gaugeLine.dy() * m_gaugeLine.dy()) / ccSpace->getScaleValue();
        emit gaugeLineResult(gaugeResult);
    }
}
