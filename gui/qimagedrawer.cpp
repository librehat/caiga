#include "qimagedrawer.h"
#include <cmath>

QImageDrawer::QImageDrawer(QWidget *parent) :
    QWidget(parent)
{
    m_circle = true;
}

void QImageDrawer::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    if (m_origPixmap.isNull())
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    QSize pixSize = m_origPixmap.size();
    pixSize.scale(event->rect().size(), Qt::KeepAspectRatio);

    QPoint topleft;
    topleft.setX((this->width() - pixSize.width()) / 2);
    topleft.setY((this->height() - pixSize.height()) / 2);

    painter.drawPixmap(topleft, m_origPixmap.scaled(pixSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    //Draw
    QPen pen(Qt::red, 2, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
    painter.setPen(pen);

    if (m_circle) {
        QPoint delta = m_mousePressed - m_mouseReleased;
        int radius = static_cast<int>(std::sqrt(std::pow(delta.x(), 2) + std::pow(delta.y(), 2)));
        painter.drawEllipse(m_mousePressed, radius, radius);
    }
    else {
        QRect rect(m_mousePressed, m_mouseReleased);
        painter.drawRect(rect);
    }
}

void QImageDrawer::mousePressEvent(QMouseEvent *m)
{
    QWidget::mousePressEvent(m);
    m_mousePressed = m->pos();
}

void QImageDrawer::mouseReleaseEvent(QMouseEvent *m)
{
    QWidget::mouseReleaseEvent(m);
    m_mouseReleased = m->pos();
    this->update();
}

void QImageDrawer::mouseMoveEvent(QMouseEvent *m)
{
    QWidget::mouseMoveEvent(m);
    m_mouseReleased = m->pos();
    this->update();
}

void QImageDrawer::setDrawCircle(bool circle)
{
    m_circle = circle;
}

const QPixmap* QImageDrawer::getOrigPixmap() const
{
    return &m_origPixmap;
}

const QPixmap* QImageDrawer::getCroppedPixmap() const
{
    return &m_croppedPixmap;
}

void QImageDrawer::setPixmap(const QPixmap &pix)
{
    m_origPixmap = pix;
    this->update();
}
