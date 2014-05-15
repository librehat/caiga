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
    if ((m_value.pressed - m_value.released).manhattanLength() < 1) {
        return;
    }

    /*
     * use if statement because switch statement forbids initialising variables inside case clauses
     */
    if (m_value.drawMode == -2) {//circle
        QPoint delta = m_value.pressed - m_value.released;
        m_value.radius = static_cast<int>(std::sqrt(std::pow(delta.x(), 2) + std::pow(delta.y(), 2)));
        m_value.centre = m_value.pressed;
    }
    else if (m_value.drawMode == -3) {//rectangle
        m_value.rect = QRect(m_value.pressed, m_value.released);
    }
    else if (m_value.drawMode == -4) {//calibre
        m_value.calibreLine = QLine(m_value.pressed, m_value.released);
        if (std::abs(m_value.calibreLine.dx()) > std::abs(m_value.calibreLine.dy())) {
            m_value.calibreLine.setP2(QPoint(m_value.released.x(), m_value.pressed.y()));
        }
        else {
            m_value.calibreLine.setP2(QPoint(m_value.pressed.x(), m_value.released.y()));
        }
        QPoint p1(m_value.calibreLine.p1().x() + (this->width() - m_image.width()) / 2,
                  m_value.calibreLine.p1().y() + (this->height() - m_image.height()) / 2);
        QPoint p2(m_value.calibreLine.p2().x() + (this->width() - m_image.width()) / 2,
                  m_value.calibreLine.p2().y() + (this->height() - m_image.height()) / 2);
        painter.drawLine(p1, p2);
    }
    else if (m_value.drawMode == -5) {//gauge
        QPoint p1(m_gaugeLine.p1().x() + (this->width() - m_image.width()) / 2,
                  m_gaugeLine.p1().y() + (this->height() - m_image.height()) / 2);
        QPoint p2(m_gaugeLine.p2().x() + (this->width() - m_image.width()) / 2,
                  m_gaugeLine.p2().y() + (this->height() - m_image.height()) / 2);
        painter.drawLine(p1, p2);
        return;
    }

    //keep circle or rectangle painted
    if (m_value.isCircle) {
        QPoint pCentre(m_value.centre.x() + (this->width() - m_image.width()) / 2,
                            m_value.centre.y() + (this->height() - m_image.height()) / 2);
        QLine vCross(pCentre.x(), pCentre.y() - 5, pCentre.x(), pCentre.y() + 5);
        QLine hCross(pCentre.x() - 5, pCentre.y(), pCentre.x() + 5, pCentre.y());
        painter.drawEllipse(pCentre, m_value.radius, m_value.radius);
        painter.drawLine(vCross);
        painter.drawLine(hCross);
    }
    else {
        int tlx = m_value.rect.topLeft().x() + (this->width() - m_image.width()) / 2;
        int tly = m_value.rect.topLeft().y() + (this->height() - m_image.height()) / 2;
        QRect pRect(tlx, tly, m_value.rect.width(), m_value.rect.height());
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

    m_value.pressed.setX(m->pos().x() - margin.x());
    m_value.pressed.setY(m->pos().y() - margin.y());

    if (m->pos().x() < margin.x()) {
        m_value.pressed.setX(0);
    }
    if (m->pos().y() < margin.y()) {
        m_value.pressed.setY(0);
    }
    if (m->pos().x() > margin_r.x()) {
        m_value.pressed.setX(m_image.width());
    }
    if (m->pos().y() > margin_r.y()) {
        m_value.pressed.setY(m_image.height());
    }

    if (m_value.drawMode == -5) {
        m_gaugeLine.setP1(m_value.pressed);
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

    m_value.released.setX(m->pos().x() - margin.x());
    m_value.released.setY(m->pos().y() - margin.y());

    if (m_value.drawMode == -2) {
        int radius = static_cast<int>(std::sqrt(std::pow((m_value.released - m_value.pressed).x(), 2) + std::pow((m_value.released - m_value.pressed).y(), 2)));
        int maxRadius = std::min(std::min(m_value.pressed.x(), m_value.pressed.y()),  std::min(m_image.width() - m_value.pressed.x(), m_image.height() - m_value.pressed.y()));
        if (radius > maxRadius) {
            m_value.released.setX(m_value.pressed.x() + maxRadius);
            m_value.released.setY(m_value.pressed.y());
        }
    }
    else {
        if (m->pos().x() < margin.x()) {
            m_value.released.setX(0);
        }
        if (m->pos().y() < margin.y()) {
            m_value.released.setY(0);
        }
        if (m->pos().x() > margin_r.x()) {
            m_value.released.setX(m_image.width());
        }
        if (m->pos().y() > margin_r.y()) {
            m_value.released.setY(m_image.height());
        }
        if (m_value.drawMode == -5) {
            m_gaugeLine.setP2(m_value.released);
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
    if (m_value.drawMode == -4) {//calibre, we should pop up a dialog and get the real size
        bool ok;
        qreal r = static_cast<qreal>(QInputDialog::getDouble(this, "Input the real size", "Unit: μm", 0, 0, 9999, 4, &ok));
        if (ok) {
            qreal scale = std::max(std::abs(m_value.calibreLine.dx()), std::abs(m_value.calibreLine.dy())) / r;
            setScaleValue(scale);
            emit calibreFinished(m_value.calibre);
        }
    }
    else if (m_value.drawMode == -5) {//gauge
        qreal gaugeResult = std::sqrt(static_cast<qreal>(std::pow(m_gaugeLine.dx(), 2) + std::pow(m_gaugeLine.dy(), 2))) / m_value.calibre;
        emit gaugeLineResult(gaugeResult);
    }
}
