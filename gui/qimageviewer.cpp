#include "qimageviewer.h"
#include <QScreen>
#include <QStyleOption>

QImageViewer::QImageViewer(QWidget *parent) :
    QWidget(parent)
{
    m_noScale = false;
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
    QPoint topleft;
    if (m_noScale) {
        topleft.setX((this->width() - m_pixmap.width()) / 2);
        topleft.setY((this->height() - m_pixmap.height()) / 2);
        painter.drawPixmap(topleft, m_pixmap);
    }
    else {
        QSize pixSize = m_pixmap.size();
        pixSize.scale(event->rect().size(), Qt::KeepAspectRatio);
        topleft.setX((this->width() - pixSize.width()) / 2);
        topleft.setY((this->height() - pixSize.height()) / 2);
        painter.drawPixmap(topleft, m_pixmap.scaled(pixSize, Qt::KeepAspectRatio, Qt::FastTransformation));
    }
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

void QImageViewer::setImage(const QImage &qimg)
{
    m_pixmap = QPixmap::fromImage(qimg);
    this->update();
}

void QImageViewer::setNoScale(bool n)
{
    m_noScale = n;
    this->update();
}
