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

    QPoint topleft;
    topleft.setX((this->width() - pixSize.width()) / 2);
    topleft.setY((this->height() - pixSize.height()) / 2);

    m_scaledImage = m_image.scaled(pixSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    painter.drawImage(topleft, m_scaledImage);

    //Draw
    QPen pen(m_penColour, 2, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
    painter.setPen(pen);

    //use if statement because switch statement forbids initialising variables inside case clauses
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
        painter.drawLine(m_drawedCalibre);
    }

    //keep circle or rectangle painted
    if (m_isCircle) {
        painter.drawEllipse(m_drawedCircleCentre, m_drawedCircleRadius, m_drawedCircleRadius);
    }
    else {
        painter.drawRect(m_drawedRect);
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
    if (m_drawMode == -4) {//calibre, we should pop up a dialog and get the real size
        bool ok;
        m_calibreRealSize = QInputDialog::getDouble(this, "Input the real size", "Unit: μm", 0, 0, 9999, 4, &ok);
        if (ok) {
            emit calibreFinished(std::max(std::abs(m_drawedCalibre.dx()), std::abs(m_drawedCalibre.dy())),
                                 m_calibreRealSize);
        }
    }
}

void QImageDrawer::mouseMoveEvent(QMouseEvent *m)
{
    QWidget::mouseMoveEvent(m);
    m_mouseReleased = m->pos();
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
        QPoint realCentre = m_drawedCircleCentre - QPoint(0, (this->height() - m_scaledImage.height()) / 2);
        QRect enclosingRect(realCentre.x() - m_drawedCircleRadius, realCentre.y() - m_drawedCircleRadius, m_drawedCircleRadius * 2, m_drawedCircleRadius * 2);
        QImage enclosingImage = m_scaledImage.copy(enclosingRect);
        QImage circle(enclosingImage.size(), enclosingImage.format());
        QPainter p(&circle);
        QPainterPath path;
        path.addEllipse(enclosingImage.rect().center(), m_drawedCircleRadius, m_drawedCircleRadius);
        p.setClipPath(path);
        p.drawImage(0, 0, enclosingImage);
        return circle;
    }
    else {
        return m_scaledImage.copy(m_drawedRect.translated(0, (m_scaledImage.height() - this->height()) / 2));
    }
}

void QImageDrawer::setImage(const QImage &img)
{
    m_image = img;
    this->update();
}
