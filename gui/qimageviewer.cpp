#include "qimageviewer.h"
#include <QtDebug>
#include <QStyleOption>

QImageViewer::QImageViewer(QWidget *parent) :
    QWidget(parent)
{
    m_notLarger = false;
}

void QImageViewer::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    //Make StyleSheet available
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    if (m_pixmap.isNull())
        return;

    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    QSize pixSize = m_pixmap.size();
    if (pixSize.height() > event->rect().size().height() || pixSize.width() > event->rect().size().width() || !m_notLarger) {
        //scale down anyway if it's too large for event
        //or scale up if m_notLarger is false
        pixSize.scale(event->rect().size(), Qt::KeepAspectRatio);
    }

    QPoint topleft;
    topleft.setX((this->width() - pixSize.width()) / 2);
    topleft.setY((this->height() - pixSize.height()) / 2);

    painter.drawPixmap(topleft, m_pixmap.scaled(pixSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

const QPixmap* QImageViewer::pixmap() const
{
    return &m_pixmap;
}

void QImageViewer::setPixmap(const QPixmap &pix)
{
    m_pixmap = pix;
    this->update();
}

void QImageViewer::setPixmap(const QImage &qimg)
{
    m_pixmap = QPixmap::fromImage(qimg);
    this->update();
}

void QImageViewer::setNotLarger(bool n)
{
    m_notLarger = n;
    this->update();
}
