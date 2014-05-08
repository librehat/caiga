#include "qimageinteractivedrawer.h"
#include <QStyleOption>

QImageInteractiveDrawer::QImageInteractiveDrawer(QWidget *parent) :
    QWidget(parent)
{
    m_white = false;
    m_penColour = QColor(0, 0, 0);//black by default
    m_drawMode = QImageInteractiveDrawer::NONE;
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

    //draw points
    QPen pen(m_penColour, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    switch (m_drawMode) {
    case NONE:
        return;
    case POLY_LINE:
        painter.setPen(pen);
        painter.drawPolyline(m_poly);
        break;
    case CIRCLE_LINE:
        pen.setWidth(8);
        painter.setPen(pen);
        painter.drawPolyline(m_poly);
        break;
    }
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

void QImageInteractiveDrawer::setWhite(bool w)
{
    m_white = w;
    if (w) {
        m_penColour = QColor(255, 255, 255);
    }
    else {
        m_penColour = QColor(0, 0, 0);
    }
}

bool QImageInteractiveDrawer::isWhite()
{
    return m_white;
}

void QImageInteractiveDrawer::setDrawMode(DRAW_MODE m)
{
    m_drawMode = m;
}

void QImageInteractiveDrawer::mousePressEvent(QMouseEvent *m)
{
    QWidget::mousePressEvent(m);
    if (m_image.isNull()) {
        return;
    }
    QPoint margin((this->width() - m_scaledImage.width()) / 2, (this->height() - m_scaledImage.height()) / 2);
    m_pressed.setX((m->pos().x() - margin.x()) / m_scale);
    m_pressed.setY((m->pos().y() - margin.y()) / m_scale);
    m_movePoints.append(m_pressed);
    emit mousePressed(m_pressed);
}

void QImageInteractiveDrawer::mouseMoveEvent(QMouseEvent *m)
{
    QWidget::mouseMoveEvent(m);
    if (m_image.isNull()) {
        return;
    }
    QPoint margin((this->width() - m_scaledImage.width()) / 2, (this->height() - m_scaledImage.height()) / 2);
    m_released.setX((m->pos().x() - margin.x()) / m_scale);
    m_released.setY((m->pos().y() - margin.y()) / m_scale);
    m_movePoints.append(m_released);
    QPoint p(m_released.x() * m_scale + (this->width() - m_scaledImage.width()) / 2, m_released.y() * m_scale + (this->height() - m_scaledImage.height()) / 2);
    m_poly << p;
    emit mouseMoved(m_released);
    this->update();
}

void QImageInteractiveDrawer::mouseReleaseEvent(QMouseEvent *m)
{
    QWidget::mouseReleaseEvent(m);
    if (m_image.isNull()) {
        return;
    }
    emit mouseReleased(m_movePoints);
    m_movePoints.clear();
    m_poly.clear();
    this->update();
}
