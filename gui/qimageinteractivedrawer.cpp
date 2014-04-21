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
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    if (m_image.isNull())
        return;

    QPoint topleft;
    topleft.setX((this->width() - m_image.width()) / 2);
    topleft.setY((this->height() - m_image.height()) / 2);

    painter.drawImage(topleft, m_image);
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
    QPoint margin((this->width() - m_image.width()) / 2, (this->height() - m_image.height()) / 2 );
    m_mousePressed.setX(m->pos().x() - margin.x());
    m_mousePressed.setY(m->pos().y() - margin.y());
    emit mouseClickedFinished(m_mousePressed);
}
