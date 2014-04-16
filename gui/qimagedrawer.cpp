#include "qimagedrawer.h"
#include <cmath>
#include <QInputDialog>
#include <QDebug>

QImageDrawer::QImageDrawer(QWidget *parent) :
    QWidget(parent)
{
    m_drawMode = -2;//circle
    m_isCircle = true;
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
    if (m_drawMode == -2) {//circle
        QPoint delta = m_mousePressed - m_mouseReleased;
        m_drawedCircleRadius = static_cast<int>(std::sqrt(std::pow(delta.x(), 2) + std::pow(delta.y(), 2)));
        m_drawedCircleCentre = m_mousePressed;
    }
    else if (m_drawMode == -3) {//rectangle
        m_drawedRect = QRect(m_mousePressed, m_mouseReleased);
    }
    else if (m_drawMode == -4) {//calibre
        m_drawedCalibre = QLine(m_mousePressed, m_mouseReleased);
        if (std::abs(m_drawedCalibre.dx()) > std::abs(m_drawedCalibre.dy())) {
            m_drawedCalibre.setP2(QPoint(m_mouseReleased.x(), m_mousePressed.y()));
        }
        else {
            m_drawedCalibre.setP2(QPoint(m_mousePressed.x(), m_mouseReleased.y()));
        }
        QPoint p1(m_drawedCalibre.p1().x() * m_scale,
                  m_drawedCalibre.p1().y() * m_scale + (this->height() - m_scaledImage.height()) / 2);
        QPoint p2(m_drawedCalibre.p2().x() * m_scale,
                  m_drawedCalibre.p2().y() * m_scale + (this->height() - m_scaledImage.height()) / 2);
        painter.drawLine(p1, p2);
    }

    //keep circle or rectangle painted
    //resize to m_scaledImage in advance.
    if (m_isCircle) {
        QPoint scaledCentre(m_drawedCircleCentre.x() * m_scale,
                            m_drawedCircleCentre.y() * m_scale + (this->height() - m_scaledImage.height()) / 2);
        int scaledRadius = m_drawedCircleRadius * m_scale;
        painter.drawEllipse(scaledCentre, scaledRadius, scaledRadius);
    }
    else {
        QPoint topleft(m_drawedRect.topLeft().x() * m_scale,
                       m_drawedRect.topLeft().y() * m_scale + (this->height() - m_scaledImage.height()) / 2);
        QRect scaledRect(topleft, m_drawedRect.size() * m_scale);
        painter.drawRect(scaledRect);
    }

}

void QImageDrawer::mousePressEvent(QMouseEvent *m)
{
    QWidget::mousePressEvent(m);
    m_mousePressed.setX(m->pos().x() / m_scale);
    m_mousePressed.setY((m->pos().y() - (this->height() - m_scaledImage.height()) / 2) / m_scale);
}

void QImageDrawer::mouseReleaseEvent(QMouseEvent *m)
{
    QWidget::mouseReleaseEvent(m);
    if (m_drawMode == -4) {//calibre, we should pop up a dialog and get the real size
        bool ok;
        m_calibreRealSize = static_cast<qreal>(QInputDialog::getDouble(this, "Input the real size", "Unit: μm", 0, 0, 9999, 4, &ok));
        if (ok) {
            emit calibreFinished(std::max(std::abs(m_drawedCalibre.dx()), std::abs(m_drawedCalibre.dy())),
                                 m_calibreRealSize);
        }
    }
}

void QImageDrawer::mouseMoveEvent(QMouseEvent *m)
{
    QWidget::mouseMoveEvent(m);
    m_mouseReleased.setX(m->pos().x() / m_scale);
    m_mouseReleased.setY((m->pos().y() - (this->height() - m_scaledImage.height()) / 2) / m_scale);
    this->update();
}

void QImageDrawer::setDrawMode(int m)
{
    m_drawMode = m;
    if (m_drawMode == -2) {
        m_isCircle = true;
    }
    else if (m_drawMode == -3) {
        m_isCircle = false;
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
    return m_isCircle;
}

QImage QImageDrawer::getCroppedImage()
{
    if (m_isCircle) {
        QRect enclosingRect(m_drawedCircleCentre.x() - m_drawedCircleRadius, m_drawedCircleCentre.y() - m_drawedCircleRadius, m_drawedCircleRadius * 2, m_drawedCircleRadius * 2);
        //QPainter cannot paint on an image with the QImage::Format_Indexed8 format
        QImage enclosingImage = m_image.copy(enclosingRect).convertToFormat(QImage::Format_RGB32);
        QImage circle(enclosingImage.size(), enclosingImage.format());
        QPainter p(&circle);
        QPainterPath path;
        path.addEllipse(enclosingImage.rect().center(), m_drawedCircleRadius, m_drawedCircleRadius);
        p.setClipPath(path);
        p.drawImage(0, 0, enclosingImage);
        return circle;
    }
    else {
        return m_image.copy(m_drawedRect);
    }
}

void QImageDrawer::setImage(const QImage &img)
{
    m_image = img;
    this->update();
}
