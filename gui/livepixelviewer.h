//basically it's a lite version of qpixeltool

#ifndef LIVEPIXELVIEWER_H
#define LIVEPIXELVIEWER_H

#include <QWidget>

class LivePixelViewer : public QWidget
{
    Q_OBJECT
public:
    explicit LivePixelViewer(QWidget *parent = 0);
    void setLivePreviewEnabled(bool = true);

private:
    bool m_live;
    int m_updateId;
    void grabScreen();
    QPixmap m_bufferPix;
    QPoint m_lastMousePos;

protected:
    void paintEvent(QPaintEvent *);
    void timerEvent(QTimerEvent *);
};

#endif // LIVEPIXELVIEWER_H
