#include <QTime>
#include <QPrinter>
#include <QtConcurrent>
#include "reporter.h"

Reporter::Reporter(CAIGA::Analyser *analyser, CAIGA::WorkSpace *workSpace, int s, const QImage &raw, QObject *parent) :
    QObject(parent)
{
    m_analyser = analyser;
    m_workSpace = workSpace;
    split = s;
    rawImage = raw;

    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    tableFormat.setBorder(1);
    tableFormat.setCellPadding(2);
    tableFormat.setHeaderRowCount(1);
    tableFormat.setAlignment(Qt::AlignCenter);

    // register the plot document object (only needed once, no matter how many plots will be in the QTextDocument):
    QCPDocumentObject *iface = new QCPDocumentObject(this);
    textDoc.documentLayout()->registerHandler(QCPDocumentObject::PlotTextFormat, iface);
    QTime t;
    qsrand(t.currentTime().msec() + t.currentTime().second() * 1000);

    cursor = QTextCursor(&textDoc);
}

void Reporter::setBarChart(QCustomPlot *plot)
{
    m_plot = plot;
    plot->clearPlottables();
    QVector<double> xticks;

    //find the biggest diameter at first
    maxRadius = m_analyser->getMaximumDiameter();

    QMap<int, QVector<double> > grainCounts;
    //initialise the QMap and its QVector values
    for (int i = 0; i < m_analyser->classCount(); ++i) {
        grainCounts[i].fill(0, split + 1);//add one to fill the x-axis blank
    }

    int maxCountBySlice = 0;
    //count
    xticks.prepend(maxRadius);
    QList<int> sList;
    for (int i = 0; i < split; ++i) {
        sList.append(i);
        xticks.prepend(maxRadius / split * i);
    }
    QtConcurrent::blockingMap(sList, [&grainCounts, &maxCountBySlice, this] (const int &slice) {
        int count = 0;
        qreal sectionMin = maxRadius / split * slice;
        qreal sectionMax = maxRadius / split * (slice + 1);

        for (int cid = 0; cid < m_analyser->classCount(); ++cid) {
            for (QMap<int, CAIGA::Object>::iterator it = m_analyser->classObjMap[cid].rObjects().begin(); it != m_analyser->classObjMap[cid].rObjects().end(); ++it) {
                if (it->diameter() > sectionMin && it->diameter() <= sectionMax) {
                    ++grainCounts[cid][slice];//thankfully, Qt initialise the int and double as 0
                    ++count;
                }
            }
        }
        maxCountBySlice = std::max(count, maxCountBySlice);
    });

    //setup QCustomPlot x axis and y axis
    QFont labelFont("Arial");
    plot->xAxis->setRange(-maxRadius / split, maxRadius + 2);
    plot->xAxis->setTickLabelType(QCPAxis::ltNumber);
    plot->xAxis->setNumberPrecision(4);
    plot->xAxis->setAutoTicks(false);
    plot->xAxis->setTickVector(xticks);
    plot->xAxis->setTickLabelRotation(-30);
    plot->xAxis->setTickLength(2);
    plot->xAxis->setSubTickLength(6);
    plot->xAxis->setSubTickCount(1);
    plot->xAxis->grid()->setVisible(false);
    plot->xAxis->setLabel(tr("Diameter (μm)"));
    plot->xAxis->setLabelFont(labelFont);
    plot->xAxis->setTickLabelFont(labelFont);

    plot->yAxis->setRange(0, static_cast<double>(maxCountBySlice + 1));
    plot->yAxis->setTickLabelType(QCPAxis::ltNumber);
    plot->yAxis->setAutoSubTicks(false);
    plot->yAxis->setSubTickCount(0);
    plot->yAxis->setTickStep(static_cast<double>(maxCountBySlice > 20 ? 5 : 2));
    plot->yAxis->setLabel(tr("Count"));
    plot->yAxis->setLabelFont(labelFont);
    plot->yAxis->setTickLabelFont(labelFont);

    //setup data
    for (int i = 0; i < m_analyser->classCount(); ++i) {
        QCPBars *bars = new QCPBars(plot->xAxis, plot->yAxis);
        plot->addPlottable(bars);

        int r = qrand() % 255;
        int g = qrand() % 255;
        int b = qrand() % 255;
        bars->setPen(QColor(r, g, b));
        bars->setBrush(QColor(r, g, b, 80));

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
    plot->legend->setFont(labelFont);
    plot->setInteraction(QCP::iSelectPlottables);
    plot->replot();
}

void Reporter::generateReport()
{
    emit workStatusStrUpdated(tr("Generating report... Please wait......"));
    cursor.setBlockFormat(alignCentreBlockFormat());
    cursor.insertBlock(alignCentreBlockFormat(), boldRomanFormat());
    cursor.insertText("Computer-Aid Interactive Grain Analyser\nAnalysis Report");
    cursor.movePosition(QTextCursor::End);
    cursor.insertHtml("<hr /><br />");
    cursor.movePosition(QTextCursor::End);

    //insert the Date
    cursor.insertBlock(alignJustifyBlockFormat(), romanFormat());
    cursor.insertText(tr("Report was generated on  "));
    cursor.insertText(QDateTime::currentDateTime().toString(Qt::DefaultLocaleLongDate));
    cursor.insertHtml("<br /><br />");
    cursor.movePosition(QTextCursor::End);

    //insert the original image
    cursor.insertBlock(alignCentreBlockFormat(), romanFormat());
    cursor.insertImage(rawImage);
    cursor.insertHtml("<br />");
    cursor.insertText(tr("Figure 1. Orignial Image"), figureInfoFormat());
    cursor.insertHtml("<br /><br />");
    //insert the processed image
    cursor.insertImage(m_workSpace->getLastDisplayImage());
    cursor.insertHtml("<br />");
    cursor.insertText(tr("Figure 2. Segmented Image Result"), figureInfoFormat());
    cursor.insertHtml("<br /><br />");

    cursor.insertText(QString(QChar::ObjectReplacementCharacter), QCPDocumentObject::generatePlotFormat(m_plot, 500, 400));
    cursor.insertHtml("<br />");
    cursor.insertText(tr("Figure 3. Count-Diameter Histogram"), figureInfoFormat());
    cursor.insertHtml("<br /><br />");
    cursor.movePosition(QTextCursor::End);

    //insert some general information
    cursor.insertBlock(alignCentreBlockFormat(), romanFormat());
    cursor.insertText(tr("Scale: ") + m_analyser->getScale() + tr(" pixel/μm"));
    cursor.insertText("\n");
    cursor.insertText(tr("Measurement Area: ") + m_analyser->getImageArea() + tr(" μm^2"));
    cursor.insertHtml("<br /><br />");
    cursor.movePosition(QTextCursor::End);

    //insert class information table
    cursor.insertTable(m_analyser->classCount() + 1, 11, tableFormat);
    insertHeaderAndMoveNextCell(&cursor, tr("Class"));
    insertHeaderAndMoveNextCell(&cursor, tr("Count"));
    insertHeaderAndMoveNextCell(&cursor, tr("Total\nArea\n(μm^2)"));
    insertHeaderAndMoveNextCell(&cursor, tr("Area\nPercentage\n(%)"));
    insertHeaderAndMoveNextCell(&cursor, tr("Average\nArea\n(μm^2)"));
    insertHeaderAndMoveNextCell(&cursor, tr("Perimeter\n\n(μm)"));
    insertHeaderAndMoveNextCell(&cursor, tr("Diameter\n\n(μm)"));
    insertHeaderAndMoveNextCell(&cursor, tr("Flattening"));
    insertHeaderAndMoveNextCell(&cursor, tr("Intercept\n\n(μm)"));
    insertHeaderAndMoveNextCell(&cursor, tr("G\n\n(Planimetric)"));
    insertHeaderAndMoveNextCell(&cursor, tr("G\n\n(Intercept)"));
    for (int i = 0; i < m_analyser->classCount(); ++i) {
        insertTextAndMoveNextCell(&cursor, m_analyser->getClassesList()->at(i));
        insertTextAndMoveNextCell(&cursor, m_analyser->getCountOfClass(i));
        insertTextAndMoveNextCell(&cursor, m_analyser->getTotalAreaOfClass(i));
        insertTextAndMoveNextCell(&cursor, m_analyser->getAvgPercentOfClass(i));
        insertTextAndMoveNextCell(&cursor, m_analyser->getAvgAreaOfClass(i));
        insertTextAndMoveNextCell(&cursor, m_analyser->getAvgPerimeterOfClass(i));
        insertTextAndMoveNextCell(&cursor, m_analyser->getAvgDiameterOfClass(i));
        insertTextAndMoveNextCell(&cursor, m_analyser->getAvgFlatteningOfClass(i));
        insertTextAndMoveNextCell(&cursor, m_analyser->getAvgInterceptOfClass(i));
        insertTextAndMoveNextCell(&cursor, m_analyser->getSizeNumberByPlanimetricOfClass(i));
        insertTextAndMoveNextCell(&cursor, m_analyser->getSizeNumberByInterceptOfClass(i));
    }
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock(alignCentreBlockFormat(), romanFormat());
    cursor.insertHtml("<br />");
    cursor.insertText(tr("Table 1. Analysis results (average values)"), figureInfoFormat());
    cursor.insertHtml("<br /><br />");

    //insert the detailed table
    cursor.insertTable(m_analyser->count() + 1, 7, tableFormat);
    insertHeaderAndMoveNextCell(&cursor, tr("Index"));
    insertHeaderAndMoveNextCell(&cursor, tr("Position"));
    insertHeaderAndMoveNextCell(&cursor, tr("Class"));
    insertHeaderAndMoveNextCell(&cursor, tr("Area\n(μm^2)"));
    insertHeaderAndMoveNextCell(&cursor, tr("Perimeter\n(μm)"));
    insertHeaderAndMoveNextCell(&cursor, tr("Diameter\n(μm)"));
    insertHeaderAndMoveNextCell(&cursor, tr("Flattening"));
    for (int i = 0; i < m_analyser->count(); ++i) {
        insertTextAndMoveNextCell(&cursor, QString::number(m_analyser->getDataModel()->item(i, 0)->data(Qt::DisplayRole).toInt()));
        insertTextAndMoveNextCell(&cursor, m_analyser->getDataModel()->item(i, 1)->data(Qt::DisplayRole).toString());
        insertTextAndMoveNextCell(&cursor, m_analyser->getDataModel()->item(i, 2)->data(Qt::DisplayRole).toString());
        for (int j = 3; j < 7; ++j) {
            insertTextAndMoveNextCell(&cursor, QString::number(m_analyser->getDataModel()->item(i, j)->data(Qt::DisplayRole).toReal()));
        }
    }
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock(alignCentreBlockFormat(), romanFormat());
    cursor.insertHtml("<br />");
    cursor.insertText(tr("Table 2. Details of each object"), figureInfoFormat());
    cursor.insertHtml("<br />");
    emit reportGenerated(&textDoc);
    emit workStatusStrUpdated(tr("Report Generated."));
    emit reportAvailable(true);
}

QTextBlockFormat Reporter::alignCentreBlockFormat()
{
    QTextBlockFormat cbf;
    cbf.setAlignment(Qt::AlignCenter);
    return cbf;
}

QTextBlockFormat Reporter::alignJustifyBlockFormat()
{
    QTextBlockFormat lbf;
    lbf.setAlignment(Qt::AlignJustify | Qt::AlignBaseline);
    return lbf;
}

QTextCharFormat Reporter::boldRomanFormat()
{
    QTextCharFormat hf;
    hf.setFontFamily("Times New Roman");
    hf.setFontPointSize(14);
    hf.setFontWeight(QFont::Bold);
    return hf;
}

QTextCharFormat Reporter::arialBoldFormat()
{
    QTextCharFormat cf;
    cf.setFontFamily("Arial");
    cf.setFontPointSize(10);
    cf.setFontWeight(QFont::Bold);
    return cf;
}

QTextCharFormat Reporter::romanFormat()
{
    QTextCharFormat mf;
    mf.setFontFamily("Times New Roman");
    mf.setFontPointSize(10);
    mf.setFontWeight(QFont::Normal);
    return mf;
}

QTextCharFormat Reporter::figureInfoFormat()
{
    QTextCharFormat ff;
    ff.setFontFamily("Times New Roman");
    ff.setFontItalic(true);
    ff.setFontPointSize(10);
    ff.setFontWeight(QFont::DemiBold);
    return ff;
}

void Reporter::exportAsPDF(QString &filename)
{
    QPrinter printer;
    if (filename.right(4).compare(".pdf", Qt::CaseInsensitive) != 0) {
        filename.append(".pdf");
    }
    printer.setOutputFileName(filename);
    printer.setOutputFormat(QPrinter::PdfFormat);
#ifdef __linux__
    //these functions are only supported on X11
    printer.setFontEmbeddingEnabled(true);
    printer.setCreator("Computer-Aid Interactive Grain Analyser");
#endif
    printer.setPaperSize(QPrinter::A4);
    printer.setPageMargins(20, 26, 20, 26, QPrinter::Millimeter);
    printer.setFullPage(true);
    textDoc.setPageSize(QSizeF(printer.pageRect().size()));
    textDoc.print(&printer);
    emit workStatusStrUpdated(tr("PDF exported as ") + filename);
}

void Reporter::exportAsFormat(QString &filename, const QByteArray &format)
{
    QString formatStr = QString(format);
    formatStr.prepend('.');
    if (filename.right(4).compare(formatStr, Qt::CaseInsensitive) != 0) {
        filename.append(formatStr);
    }
    QTextDocumentWriter writer(filename, format);
    bool ok = writer.write(&textDoc);
    if (ok) {
        emit workStatusStrUpdated(tr("Exported Successfully."));
    }
    else {
        emit workStatusStrUpdated(tr("Export Failed."));
    }
}
