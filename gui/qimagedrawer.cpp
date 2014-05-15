#include "qimagedrawer.h"
#include <QDebug>
#include <cmath>
#include <QInputDialog>

QImageDrawer::QImageDrawer(QWidget *parent) :
    QWidget(parent)
{
    m_penColour = QColor(255, 0, 0);//Red
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
    if ((ccSpace.mousePressed - ccSpace.mouseReleased).manhattanLength() < 1) {
        return;
    }

    /*
     * use if statement because switch statement forbids initialising variables inside case clauses
     */
    if (ccSpace.drawMode == -2) {//circle
        QPoint delta = ccSpace.mousePressed - ccSpace.mouseReleased;
        ccSpace.circleRadius = static_cast<int>(std::sqrt(std::pow(delta.x(), 2) + std::pow(delta.y(), 2)));
        ccSpace.circleCentre = ccSpace.mousePressed;
    }
    else if (ccSpace.drawMode == -3) {//rectangle
        ccSpace.qrect = QRect(ccSpace.mousePressed, ccSpace.mouseReleased);
    }
    else if (ccSpace.drawMode == -4) {//calibre
        ccSpace.calibreLine = QLine(ccSpace.mousePressed, ccSpace.mouseReleased);
        if (std::abs(ccSpace.calibreLine.dx()) > std::abs(ccSpace.calibreLine.dy())) {
            ccSpace.calibreLine.setP2(QPoint(ccSpace.mouseReleased.x(), ccSpace.mousePressed.y()));
        }
        else {
            ccSpace.calibreLine.setP2(QPoint(ccSpace.mousePressed.x(), ccSpace.mouseReleased.y()));
        }
        QPoint p1(ccSpace.calibreLine.p1().x() + (this->width() - m_image.width()) / 2,
                  ccSpace.calibreLine.p1().y() + (this->height() - m_image.height()) / 2);
        QPoint p2(ccSpace.calibreLine.p2().x() + (this->width() - m_image.width()) / 2,
                  ccSpace.calibreLine.p2().y() + (this->height() - m_image.height()) / 2);
        painter.drawLine(p1, p2);
    }
    else if (ccSpace.drawMode == -5) {//gauge
        QPoint p1(m_gaugeLine.p1().x() + (this->width() - m_image.width()) / 2,
                  m_gaugeLine.p1().y() + (this->height() - m_image.height()) / 2);
        QPoint p2(m_gaugeLine.p2().x() + (this->width() - m_image.width()) / 2,
                  m_gaugeLine.p2().y() + (this->height() - m_image.height()) / 2);
        painter.drawLine(p1, p2);
        return;
    }

    //keep circle or rectangle painted
    if (ccSpace.isCircle) {
        QPoint pCentre(ccSpace.circleCentre.x() + (this->width() - m_image.width()) / 2,
                            ccSpace.circleCentre.y() + (this->height() - m_image.height()) / 2);
        QLine vCross(pCentre.x(), pCentre.y() - 5, pCentre.x(), pCentre.y() + 5);
        QLine hCross(pCentre.x() - 5, pCentre.y(), pCentre.x() + 5, pCentre.y());
        painter.drawEllipse(pCentre, ccSpace.circleRadius, ccSpace.circleRadius);
        painter.drawLine(vCross);
        painter.drawLine(hCross);
    }
    else {
        int tlx = ccSpace.qrect.topLeft().x() + (this->width() - m_image.width()) / 2;
        int tly = ccSpace.qrect.topLeft().y() + (this->height() - m_image.height()) / 2;
        QRect pRect(tlx, tly, ccSpace.qrect.width(), ccSpace.qrect.height());
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

    ccSpace.mousePressed.setX(m->pos().x() - margin.x());
    ccSpace.mousePressed.setY(m->pos().y() - margin.y());

    if (m->pos().x() < margin.x()) {
        ccSpace.mousePressed.setX(0);
    }
    if (m->pos().y() < margin.y()) {
        ccSpace.mousePressed.setY(0);
    }
    if (m->pos().x() > margin_r.x()) {
        ccSpace.mousePressed.setX(m_image.width());
    }
    if (m->pos().y() > margin_r.y()) {
        ccSpace.mousePressed.setY(m_image.height());
    }

    if (ccSpace.drawMode == -5) {
        m_gaugeLine.setP1(ccSpace.mousePressed);
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

    ccSpace.mouseReleased.setX(m->pos().x() - margin.x());
    ccSpace.mouseReleased.setY(m->pos().y() - margin.y());

    if (ccSpace.drawMode == -2) {
        int radius = static_cast<int>(std::sqrt(std::pow((ccSpace.mouseReleased - ccSpace.mousePressed).x(), 2) + std::pow((ccSpace.mouseReleased - ccSpace.mousePressed).y(), 2)));
        int maxRadius = std::min(std::min(ccSpace.mousePressed.x(), ccSpace.mousePressed.y()),  std::min(m_image.width() - ccSpace.mousePressed.x(), m_image.height() - ccSpace.mousePressed.y()));
        if (radius > maxRadius) {
            ccSpace.mouseReleased.setX(ccSpace.mousePressed.x() + maxRadius);
            ccSpace.mouseReleased.setY(ccSpace.mousePressed.y());
        }
    }
    else {
        if (m->pos().x() < margin.x()) {
            ccSpace.mouseReleased.setX(0);
        }
        if (m->pos().y() < margin.y()) {
            ccSpace.mouseReleased.setY(0);
        }
        if (m->pos().x() > margin_r.x()) {
            ccSpace.mouseReleased.setX(m_image.width());
        }
        if (m->pos().y() > margin_r.y()) {
            ccSpace.mouseReleased.setY(m_image.height());
        }
        if (ccSpace.drawMode == -5) {
            m_gaugeLine.setP2(ccSpace.mouseReleased);
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
    if (ccSpace.drawMode == -4) {//calibre, we should pop up a dialog and get the real size
        bool ok;
        qreal r = static_cast<qreal>(QInputDialog::getDouble(this, "Input the real size", "Unit: μm", 0, 0, 9999, 4, &ok));
        if (ok) {
            qreal scale = std::max(std::abs(ccSpace.calibreLine.dx()), std::abs(ccSpace.calibreLine.dy())) / r;
            setScaleValue(scale);
            emit calibreFinished(ccSpace.scaleValue);
        }
    }
    else if (ccSpace.drawMode == -5) {//gauge
        qreal gaugeResult = std::sqrt(static_cast<qreal>(std::pow(m_gaugeLine.dx(), 2) + std::pow(m_gaugeLine.dy(), 2))) / ccSpace.scaleValue;
        emit gaugeLineResult(gaugeResult);
    }
}
