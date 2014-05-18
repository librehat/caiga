#ifndef REPORTER_H
#define REPORTER_H

#include <QObject>
#include "analyser.h"
#include "3rdparty/qcustomplot.h"

class Reporter : public QObject
{
    Q_OBJECT
public:
    explicit Reporter(CAIGA::Analyser *analyser, QObject *parent = 0);
    void setBarChart(QCustomPlot *plot);

private:
    CAIGA::Analyser *m_analyser;

    qreal maxRadius;

};

#endif // REPORTER_H
