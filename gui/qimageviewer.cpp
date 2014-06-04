#include "qimageviewer.h"
#include <QScreen>
#include <QStyleOption>

QImageViewer::QImageViewer(QWidget *parent) :
    QWidget(parent)
{
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

    painter.setRenderHint(QPainter::Antialiasing);
    QSizeF pixSize = m_pixmap.size().scaled(this->width(), this->height(), Qt::KeepAspectRatio);
    painter.translate((this->width() - pixSize.width()) / 2, (this->height() - pixSize.height()) / 2);
    painter.scale(pixSize.width() / m_pixmap.width(), pixSize.height() / m_pixmap.height());
    painter.drawPixmap(0, 0, m_pixmap);
}
