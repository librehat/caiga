#include "qimagedrawer.h"
#include <cmath>
#include <QInputDialog>

QImageDrawer::QImageDrawer(QWidget *parent) :
    QWidget(parent)
{
    m_drawMode = -2;//circle
    m_penColour = QColor(255, 0, 0);//Red
}

void QImageDrawer::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    if (m_origPixmap.isNull())
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    QSize pixSize = m_origPixmap.size();
    pixSize.scale(event->rect().size(), Qt::KeepAspectRatio);

    QPoint topleft;
    topleft.setX((this->width() - pixSize.width()) / 2);
    topleft.setY((this->height() - pixSize.height()) / 2);

    painter.drawPixmap(topleft, m_origPixmap.scaled(pixSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    //Draw
    QPen pen(m_penColour, 2, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
    painter.setPen(pen);

    //use if statement because switch statement forbids initialising variables inside case clauses
    if (m_drawMode == -2) {//circle
        QPoint delta = m_mousePressed - m_mouseReleased;
        m_drawedCircleRadius = static_cast<int>(std::sqrt(std::pow(delta.x(), 2) + std::pow(delta.y(), 2)));
        m_drawedCircleCentre = m_mousePressed;
        painter.drawEllipse(m_drawedCircleCentre, m_drawedCircleRadius, m_drawedCircleRadius);
    }
    else if (m_drawMode == -3) {//rectangle
        m_drawedRect = QRect(m_mousePressed, m_mouseReleased);
        painter.drawRect(m_drawedRect);
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
}

void QImageDrawer::setPenColour(const QString &c)
{
    QColor colour(c);
    if (colour.isValid()) {
        m_penColour = colour;
    }
}

const QPixmap* QImageDrawer::getOrigPixmap() const
{
    return &m_origPixmap;
}

const QPixmap* QImageDrawer::getCroppedPixmap() const
{
    return &m_croppedPixmap;
}

void QImageDrawer::setPixmap(const QPixmap &pix)
{
    m_origPixmap = pix;
    this->update();
}
