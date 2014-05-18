#include <opencv2/core/core.hpp>
#include "reporter.h"

Reporter::Reporter(CAIGA::Analyser *analyser, QObject *parent) :
    QObject(parent)
{
    m_analyser = analyser;
}

void Reporter::setBarChart(QCustomPlot *plot)
{
    plot->clearPlottables();
    QVector<double> xticks;

    //find the biggest radius at first
    QVector<qreal> radii = *m_analyser->getEquivalentRadii();
    maxRadius = 0;
    for (QVector<qreal>::iterator it = radii.begin(); it != radii.end(); ++it) {
        maxRadius = std::max((*it), maxRadius);
    }

    QMap<int, QVector<double> > grainCounts;
    //initialise the QMap and its QVector values
    for (int i = 0; i < m_analyser->getClassesList()->size(); ++i) {
        grainCounts[i].fill(0, 21);//the 21st one to fill the x-axis blank
    }

    int maxCountBySlice = 0;
    //count
    xticks.prepend(maxRadius);
    for (int slice = 19; slice >= 0; --slice) {
        int count = 0;
        double sectionMin = maxRadius / 20 * slice;
        xticks.prepend(sectionMin);

        //search every radius
        for (int idx = 0; idx < radii.size();) {
            if (radii.at(idx) > sectionMin) {
                radii.removeAt(idx);//remove it so it won't be used as duplicate next time
                ++grainCounts[m_analyser->getClassIndice()->at(idx)][slice];//thankfully, Qt initialise the int and double as 0
                ++count;
            }
            else {
                ++idx;
            }
        }
        maxCountBySlice = std::max(count, maxCountBySlice);
    }

    //setup QCustomPlot x axis and y axis
    plot->xAxis->setRange(-0.5, maxRadius + 2);
    plot->xAxis->setTickLabelType(QCPAxis::ltNumber);
    plot->xAxis->setNumberPrecision(4);
    plot->xAxis->setAutoTicks(false);
    plot->xAxis->setTickVector(xticks);
    plot->xAxis->setTickLabelRotation(-20);
    plot->xAxis->setTickLength(0);
    plot->xAxis->setSubTickLength(4);
    plot->xAxis->setSubTickCount(1);
    plot->xAxis->grid()->setVisible(false);
    plot->xAxis->setLabel("Minimum Equivalent Radii / μm");

    plot->yAxis->setRange(0, static_cast<double>(maxCountBySlice + 1));
    plot->yAxis->setTickLabelType(QCPAxis::ltNumber);
    plot->yAxis->setNumberPrecision(0);
    plot->yAxis->setAutoSubTicks(false);
    plot->yAxis->setSubTickCount(0);
    plot->yAxis->setTickStep(static_cast<double>(maxCountBySlice > 20 ? 5 : 2));
    plot->yAxis->setLabel("Grain Count");

    //setup data
    for (int i = 0; i < m_analyser->getClassesList()->size(); ++i) {
        QCPBars *bars = new QCPBars(plot->xAxis, plot->yAxis);
        plot->addPlottable(bars);

        int r = static_cast<int>(cv::theRNG().uniform(0, 255));
        int g = static_cast<int>(cv::theRNG().uniform(0, 255));
        int b = static_cast<int>(cv::theRNG().uniform(0, 255));
        bars->setPen(QColor(r, g, b));
        bars->setBrush(QColor(r, g, b, i == 0 ? 80 : 50));

        bars->setName(m_analyser->getClassesList()->at(i));
        bars->setData(xticks, grainCounts[i]);
        bars->setWidth(0.55);

        //stack above the previous one
        if (i > 0) {
            if (QCPBars *previousBars = qobject_cast<QCPBars *>(plot->plottable(i - 1))) {
                bars->moveAbove(previousBars);
            }
        }
    }

    plot->legend->setVisible(true);
    plot->setInteraction(QCP::iSelectPlottables);
    plot->replot();
}
