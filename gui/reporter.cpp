#include <opencv2/core/core.hpp>
#include "reporter.h"

Reporter::Reporter(CAIGA::Analyser *analyser, CAIGA::WorkSpace *workSpace, int s, QObject *parent) :
    QObject(parent)
{
    m_analyser = analyser;
    m_workSpace = workSpace;
    split = s;
    textDoc = new QTextDocument(this);

    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    tableFormat.setBorder(1);
    tableFormat.setCellPadding(6);
    tableFormat.setHeaderRowCount(1);
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
    for (int i = 0; i < m_analyser->classCount(); ++i) {
        grainCounts[i].fill(0, split + 1);//add one to fill the x-axis blank
    }

    int maxCountBySlice = 0;
    //count
    xticks.prepend(maxRadius);
    for (int slice = split - 1; slice >= 0; --slice) {
        int count = 0;
        double sectionMin = maxRadius / split * slice;
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
    plot->xAxis->setRange(-maxRadius / split, maxRadius + 2);
    plot->xAxis->setTickLabelType(QCPAxis::ltNumber);
    plot->xAxis->setNumberPrecision(4);
    plot->xAxis->setAutoTicks(false);
    plot->xAxis->setTickVector(xticks);
    plot->xAxis->setTickLabelRotation(-20);
    plot->xAxis->setTickLength(2);
    plot->xAxis->setSubTickLength(6);
    plot->xAxis->setSubTickCount(1);
    plot->xAxis->grid()->setVisible(false);
    plot->xAxis->setLabel("Equivalent Radii / μm");

    plot->yAxis->setRange(0, static_cast<double>(maxCountBySlice + 1));
    plot->yAxis->setTickLabelType(QCPAxis::ltNumber);
    plot->yAxis->setAutoSubTicks(false);
    plot->yAxis->setSubTickCount(0);
    plot->yAxis->setTickStep(static_cast<double>(maxCountBySlice > 20 ? 5 : 2));
    plot->yAxis->setLabel("Grain Count");

    //setup data
    for (int i = 0; i < m_analyser->classCount(); ++i) {
        QCPBars *bars = new QCPBars(plot->xAxis, plot->yAxis);
        plot->addPlottable(bars);

        int r = static_cast<int>(cv::theRNG().uniform(0, 255));
        int g = static_cast<int>(cv::theRNG().uniform(0, 255));
        int b = static_cast<int>(cv::theRNG().uniform(0, 255));
        bars->setPen(QColor(r, g, b));
        bars->setBrush(QColor(r, g, b, i == 0 ? 80 : 50));

        bars->setName(m_analyser->getClassesList()->at(i));
        bars->setData(xticks, grainCounts[i]);
        bars->setWidth(0.5 * 20 / split);

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

void Reporter::setTextBrowser(QTextBrowser *tb)
{
    QTextCursor cursor(textDoc);
    cursor.setBlockFormat(alignJustifyBlockFormat());
    cursor.insertBlock(alignCentreBlockFormat(), boldRomanFormat());
    cursor.insertText("School of Materials Science and Engineering, Southeast University");
    cursor.movePosition(QTextCursor::End);
    cursor.insertHtml("<hr /><br />");
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock(alignJustifyBlockFormat(), cambriaFormat());
    cursor.insertImage(m_workSpace->getRawImage(), "Orignial Image");
    cursor.insertTable(m_analyser->count() + 1, 5, tableFormat);
    insertHtmlAndMoveNextCell(&cursor, "<b>Index</b>");
    insertHtmlAndMoveNextCell(&cursor, "<b>Class</b>");
    insertHtmlAndMoveNextCell(&cursor, "<b>Area</b><br />(μm<sup>2</sup>)");
    insertHtmlAndMoveNextCell(&cursor, "<b>Perimeter</b><br />(μm)");
    insertHtmlAndMoveNextCell(&cursor, "<b>Equivalent Radius</b><br />(μm)");
    for (int i = 0; i < m_analyser->count(); ++i) {
        insertTextAndMoveNextCell(&cursor, QString::number(i + 1));
        insertTextAndMoveNextCell(&cursor, m_analyser->getClassNameAt(i));
        insertTextAndMoveNextCell(&cursor, m_analyser->getAreaAt(i));
        insertTextAndMoveNextCell(&cursor, m_analyser->getPerimeterAt(i));
        insertTextAndMoveNextCell(&cursor, m_analyser->getRadiusAt(i));
    }
    cursor.movePosition(QTextCursor::EndOfBlock);
    cursor.insertImage(m_workSpace->getLastDisplayImage(), "Segmented Image");

    tb->setDocument(textDoc);
}

QTextBlockFormat Reporter::alignCentreBlockFormat()
{
    static QTextBlockFormat cbf;
    cbf.setAlignment(Qt::AlignCenter);
    return cbf;
}

QTextBlockFormat Reporter::alignJustifyBlockFormat()
{
    static QTextBlockFormat lbf;
    lbf.setAlignment(Qt::AlignJustify | Qt::AlignBaseline);
    return lbf;
}

QTextCharFormat Reporter::boldRomanFormat()
{
    static QTextCharFormat hf;
    hf.setFontFamily("Times New Roman");
    hf.setFontPointSize(16);
    hf.setFontWeight(QFont::Bold);
    return hf;
}

QTextCharFormat Reporter::cambriaFormat()
{
    static QTextCharFormat mf;
    mf.setFontFamily("Cambria");
    mf.setFontPointSize(12);
    mf.setFontWeight(QFont::Normal);
    return mf;
}
