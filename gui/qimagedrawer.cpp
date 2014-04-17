#include "qimagedrawer.h"
#include <cmath>
#include <QInputDialog>

QImageDrawer::QImageDrawer(QWidget *parent) :
    QWidget(parent)
{
    m_value.drawMode = -2;//circle
    m_value.isCircle = true;
    m_penColour = QColor(255, 0, 0);//Red
    m_scale = 1.0;
}

void QImageDrawer::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    if (m_image.isNull())
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    QSize pixSize = m_image.size();
    pixSize.scale(event->rect().size(), Qt::KeepAspectRatio);
    m_scale = static_cast<qreal>(pixSize.width()) / static_cast<qreal>(m_image.width());//width / width is okay because the ratio was kept

    QPoint topleft;
    topleft.setX((this->width() - pixSize.width()) / 2);
    topleft.setY((this->height() - pixSize.height()) / 2);

    m_scaledImage = m_image.scaled(pixSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    painter.drawImage(topleft, m_scaledImage);

    //Draw
    QPen pen(m_penColour, 2, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
    painter.setPen(pen);

    /*
     * use if statement because switch statement forbids initialising variables inside case clauses
     * recalculate points because the user may change m_scaledImage's size by changing window's size.
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
        QPoint p1(m_value.calibreLine.p1().x() * m_scale + (this->width() - m_scaledImage.width()) / 2,
                  m_value.calibreLine.p1().y() * m_scale + (this->height() - m_scaledImage.height()) / 2);
        QPoint p2(m_value.calibreLine.p2().x() * m_scale + (this->width() - m_scaledImage.width()) / 2,
                  m_value.calibreLine.p2().y() * m_scale + (this->height() - m_scaledImage.height()) / 2);
        painter.drawLine(p1, p2);
    }

    //keep circle or rectangle painted
    //resize to m_scaledImage in advance.
    if (m_value.isCircle) {
        QPoint scaledCentre(m_value.centre.x() * m_scale + (this->width() - m_scaledImage.width()) / 2,
                            m_value.centre.y() * m_scale + (this->height() - m_scaledImage.height()) / 2);
        int scaledRadius = m_value.radius * m_scale;
        QLine vCross(scaledCentre.x(), scaledCentre.y() - 5, scaledCentre.x(), scaledCentre.y() + 5);
        QLine hCross(scaledCentre.x() - 5, scaledCentre.y(), scaledCentre.x() + 5, scaledCentre.y());
        painter.drawEllipse(scaledCentre, scaledRadius, scaledRadius);
        painter.drawLine(vCross);
        painter.drawLine(hCross);
    }
    else {
        QPoint topleft(m_value.rect.topLeft().x() * m_scale + (this->width() - m_scaledImage.width()) / 2,
                       m_value.rect.topLeft().y() * m_scale + (this->height() - m_scaledImage.height()) / 2);
        QRect scaledRect(topleft, m_value.rect.size() * m_scale);
        painter.drawRect(scaledRect);
    }

}

void QImageDrawer::mousePressEvent(QMouseEvent *m)
{
    QWidget::mousePressEvent(m);
    if (m_image.isNull()) {
        return;
    }

    QPoint margin((this->width() - m_scaledImage.width()) / 2, (this->height() - m_scaledImage.height()) / 2);
    QPoint margin_r = margin + QPoint(m_scaledImage.width(), m_scaledImage.height());

    m_value.pressed.setX((m->pos().x() - margin.x()) / m_scale);
    m_value.pressed.setY((m->pos().y() - margin.y()) / m_scale);

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
}

void QImageDrawer::mouseMoveEvent(QMouseEvent *m)
{
    QWidget::mouseMoveEvent(m);
    if (m_image.isNull()) {
        return;
    }

    QPoint margin((this->width() - m_scaledImage.width()) / 2, (this->height() - m_scaledImage.height()) / 2);
    QPoint margin_r = margin + QPoint(m_scaledImage.width(), m_scaledImage.height());

    m_value.released.setX((m->pos().x() - margin.x()) / m_scale);
    m_value.released.setY((m->pos().y() - margin.y()) / m_scale);

    if (m_value.isCircle) {
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
            m_value.calibre = std::max(std::abs(m_value.calibreLine.dx()), std::abs(m_value.calibreLine.dy())) / r;
            emit calibreFinished(m_value.calibre);
        }
    }
}

void QImageDrawer::setDrawMode(int m)
{
    m_value.drawMode = m;
    if (m == -2) {
        m_value.isCircle = true;
    }
    else if (m == -3) {
        m_value.isCircle = false;
    }
}

void QImageDrawer::setPenColour(const QString &c)
{
    QColor colour(c);
    if (colour.isValid()) {
        m_penColour = colour;
    }
}

const QImage* QImageDrawer::image() const
{
    return &m_image;
}

bool QImageDrawer::isCircle()
{
    return m_value.isCircle;
}

/*
 * deprecated
 * let core do the dirty jobs
 *
QImage QImageDrawer::getCroppedImage()
{
    if (m_value.isCircle) {
        QRect enclosingRect(m_value.centre.x() - m_value.radius, m_value.centre.y() - m_value.radius, m_value.radius * 2, m_value.radius * 2);

        //QPainter cannot paint on an image with the QImage::Format_Indexed8 format
        QImage enclosingImage = m_image.copy(enclosingRect).convertToFormat(QImage::Format_RGB32);
        QImage circle(enclosingImage.size(), enclosingImage.format());
        QPainter p(&circle);
        QPainterPath path;
        path.addEllipse(enclosingImage.rect().center(), m_value.radius, m_value.radius);
        p.setClipPath(path);
        p.drawImage(0, 0, enclosingImage);
        return circle;
    }
    else {
        return m_image.copy(m_value.rect);
    }
}
*/

void QImageDrawer::setImage(const QImage &img)
{
    m_image = img;
    this->update();
}

void QImageDrawer::reset()
{
    m_value.drawMode = -2;
    m_value.isCircle = true;
    m_value.pressed = QPoint();
    m_value.released = QPoint();
    m_value.centre = QPoint();
    m_value.radius = 0;
    m_value.rect = QRect();
    m_value.calibreLine = QLine();
    this->update();
}

ccStruct QImageDrawer::getCCStruct()
{
    //m_value.croppedImage = Image::convertQImage2Mat(getCroppedImage(), true);//ensure it's a CV_8UC1 Mat.
    return m_value;
}

void QImageDrawer::restoreState(ccStruct c)
{
    m_value = c;
    this->update();
}
