#include "qimageinteractivedrawer.h"
#include <QStyleOption>

QImageInteractiveDrawer::QImageInteractiveDrawer(QWidget *parent) :
    QWidget(parent)
{
    m_white = false;
    m_penColour = QColor(0, 0, 0);//black by default
    m_drawMode = QImageInteractiveDrawer::NONE;
    firstTimeShow = true;
}

void QImageInteractiveDrawer::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    //Make StyleSheet available
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    if (m_image.isNull())
        return;

    if (firstTimeShow) {
        resetZoom();
        firstTimeShow = false;
    }

    qreal zoom = m_zoomer.getZoom();
    QSizeF pixSize = m_image.size() * zoom;

    transformer.setMatrix(zoom, 0, 0, 0, zoom, 0, (this->width() - pixSize.width()) / 2.0, (this->height() - pixSize.height()) / 2.0, 1.0);
    painter.setWorldTransform(transformer);
    painter.drawImage(0, 0, m_image);

    //draw points
    QPen pen(m_penColour, 3, Qt::SolidLine, Qt::RoundCap, Qt::BevelJoin);
    switch (m_drawMode) {
    case NONE:
        return;
    case PENCIL:
        painter.setPen(pen);
        painter.drawPolyline(m_poly);
        break;
    case ERASER:
        pen.setWidth(8);
        painter.setPen(pen);
        painter.drawPolyline(m_poly);
        break;
    }
}

void QImageInteractiveDrawer::setDrawMode(DRAW_MODE m)
{
    m_drawMode = m;
    switch (m_drawMode) {
    case NONE:
        this->setCursor(QCursor(Qt::ArrowCursor));
        break;
    case PENCIL:
    case ERASER:
        this->setCursor(QCursor(Qt::CrossCursor));
        break;
    }
}

void QImageInteractiveDrawer::mousePressEvent(QMouseEvent *m)
{
    QWidget::mousePressEvent(m);
    if (m_image.isNull()) {
        return;
    }
    m_pressed = transformer.inverted().map(m->localPos());
    if (m_pressed.x() < 0) {
        m_pressed.setX(0);
    }
    else if (m_pressed.x() > m_image.width() - 1) {
        m_pressed.setX(m_image.width() - 1);
    }
    if (m_pressed.y() < 0) {
        m_pressed.setY(0);
    }
    else if (m_pressed.y() > m_image.height() - 1) {
        m_pressed.setY(m_image.height() - 1);
    }
    m_movePoints.append(m_pressed);
    emit mousePressed(m_pressed);
}

void QImageInteractiveDrawer::mouseMoveEvent(QMouseEvent *m)
{
    QWidget::mouseMoveEvent(m);
    if (m_image.isNull()) {
        return;
    }
    m_released = transformer.inverted().map(m->localPos());
    if (m_released.x() < 0) {
        m_released.setX(0);
    }
    else if (m_released.x() > m_image.width() - 1) {
        m_released.setX(m_image.width() - 1);
    }
    if (m_released.y() < 0) {
        m_released.setY(0);
    }
    else if (m_released.y() > m_image.height() - 1) {
        m_released.setY(m_image.height() - 1);
    }
    m_movePoints.append(m_released);
    m_poly << m_released;
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

void QImageInteractiveDrawer::wheelEvent(QWheelEvent *we)
{
    QWidget::wheelEvent(we);
    if(we->angleDelta().y() > 0) {
        m_zoomer.zoomIn();
    }
    else {
        m_zoomer.zoomOut();
    }
    handleSizeChanged(m_zoomer.getZoom());
    update();
}

void QImageInteractiveDrawer::resetZoom()
{
    m_zoomer.setZoom(1.0);
    handleSizeChanged(m_zoomer.getZoom());
}

void QImageInteractiveDrawer::handleSizeChanged(qreal zoom)
{
    emit zoomUpdated(zoom);
    setMinimumSize(m_image.size() * zoom);
}
