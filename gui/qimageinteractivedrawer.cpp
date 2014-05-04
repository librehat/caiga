#include "qimageinteractivedrawer.h"
#include <QStyleOption>

QImageInteractiveDrawer::QImageInteractiveDrawer(QWidget *parent) :
    QWidget(parent)
{
}

void QImageInteractiveDrawer::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    //Make StyleSheet available
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    if (m_image.isNull())
        return;

    QSize pixSize = m_image.size();
    if (pixSize.height() > event->rect().size().height() || pixSize.width() > event->rect().size().width()) {
        pixSize.scale(event->rect().size(), Qt::KeepAspectRatio);
    }
    m_scale = static_cast<qreal>(pixSize.width()) / static_cast<qreal>(m_image.width());//width / width is okay because the ratio was kept

    QPoint topleft;
    topleft.setX((this->width() - pixSize.width()) / 2);
    topleft.setY((this->height() - pixSize.height()) / 2);

    m_scaledImage = m_image.scaled(pixSize, Qt::KeepAspectRatio, Qt::FastTransformation);
    painter.drawImage(topleft, m_scaledImage);
}

const QImage *QImageInteractiveDrawer::image() const
{
    return &m_image;
}

void QImageInteractiveDrawer::setImage(const QImage &img)
{
    m_image = img;
    this->update();
}

void QImageInteractiveDrawer::mousePressEvent(QMouseEvent *m)
{
    QWidget::mousePressEvent(m);
    if (m_image.isNull()) {
        return;
    }
    QPoint margin((this->width() - m_scaledImage.width()) / 2, (this->height() - m_scaledImage.height()) / 2);
    emit mousePressed((m->pos().x() - margin.x()) / m_scale, (m->pos().y() - margin.y()) / m_scale);
}

void QImageInteractiveDrawer::mouseMoveEvent(QMouseEvent *m)
{
    QWidget::mouseMoveEvent(m);
    if (m_image.isNull()) {
        return;
    }
    QPoint margin((this->width() - m_scaledImage.width()) / 2, (this->height() - m_scaledImage.height()) / 2);
    emit mouseMoved((m->pos().x() - margin.x()) / m_scale, (m->pos().y() - margin.y()) / m_scale);
}
