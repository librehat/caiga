#include <QTime>
#include <QPrinter>
#include "reporter.h"

Reporter::Reporter(CAIGA::Analyser *analyser, CAIGA::WorkSpace *workSpace, int s, QObject *parent) :
    QObject(parent)
{
    m_analyser = analyser;
    m_workSpace = workSpace;
    split = s;

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
    for (int slice = split - 1; slice >= 0; --slice) {
        int count = 0;
        qreal sectionMin = maxRadius / split * slice;
        qreal sectionMax = maxRadius / split * (slice + 1);
        xticks.prepend(sectionMin);

        for (int cid = 0; cid < m_analyser->classCount(); ++cid) {
            for (QMap<int, CAIGA::Object>::iterator it = m_analyser->classObjMap[cid].rObjects().begin(); it != m_analyser->classObjMap[cid].rObjects().end(); ++it) {
                if (it->diameter() > sectionMin && it->diameter() <= sectionMax) {
                    ++grainCounts[cid][slice];//thankfully, Qt initialise the int and double as 0
                    ++count;
                }
            }
        }
        maxCountBySlice = std::max(count, maxCountBySlice);
    }

    //setup QCustomPlot x axis and y axis
    QFont cambriaMath("Cambria Math");
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
    plot->xAxis->setLabel("Diameter (μm)");
    plot->xAxis->setLabelFont(cambriaMath);
    plot->xAxis->setTickLabelFont(cambriaMath);

    plot->yAxis->setRange(0, static_cast<double>(maxCountBySlice + 1));
    plot->yAxis->setTickLabelType(QCPAxis::ltNumber);
    plot->yAxis->setAutoSubTicks(false);
    plot->yAxis->setSubTickCount(0);
    plot->yAxis->setTickStep(static_cast<double>(maxCountBySlice > 20 ? 5 : 2));
    plot->yAxis->setLabel("Count");
    plot->yAxis->setLabelFont(cambriaMath);
    plot->yAxis->setTickLabelFont(cambriaMath);

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
    plot->setInteraction(QCP::iSelectPlottables);
    plot->replot();
}

void Reporter::setTextBrowser(QTextBrowser *tb)
{
    emit workStatusStrUpdated("Generating report... Please wait......");
    QTextCursor cursor(&textDoc);
    cursor.setBlockFormat(alignCentreBlockFormat());
    cursor.insertBlock(alignCentreBlockFormat(), boldRomanFormat());
    cursor.insertText("Computer-Aid Interactive Grain Analyser\nAnalysis Report");
    cursor.movePosition(QTextCursor::End);
    cursor.insertHtml("<hr /><br />");
    cursor.movePosition(QTextCursor::End);

    //insert the Date
    cursor.insertBlock(alignJustifyBlockFormat(), romanFormat());
    cursor.insertText("Report was generated on  ");
    cursor.insertText(QDateTime::currentDateTime().toString(Qt::DefaultLocaleLongDate));
    cursor.insertHtml("<br /><br />");
    cursor.movePosition(QTextCursor::End);

    //insert the original image
    cursor.insertBlock(alignCentreBlockFormat(), romanFormat());
    cursor.insertImage(m_workSpace->getRawImage());
    cursor.insertHtml("<br />");
    cursor.insertText("Figure 1. Orignial Image", figureInfoFormat());
    cursor.insertHtml("<br /><br />");
    //insert the processed image
    cursor.insertImage(m_workSpace->getLastDisplayImage());
    cursor.insertHtml("<br />");
    cursor.insertText("Figure 2. Segmented Image Result", figureInfoFormat());
    cursor.insertHtml("<br /><br />");

    cursor.insertText(QString(QChar::ObjectReplacementCharacter), QCPDocumentObject::generatePlotFormat(m_plot, 500, 400));
    cursor.insertHtml("<br />");
    cursor.insertText("Figure 3. Diameter Histogram", figureInfoFormat());
    cursor.insertHtml("<br /><br />");
    cursor.movePosition(QTextCursor::End);

    //insert class information table
    cursor.insertTable(m_analyser->classCount() + 1, 10, tableFormat);
    insertHeaderAndMoveNextCell(&cursor, "Class");
    insertHeaderAndMoveNextCell(&cursor, "Count");
    insertHeaderAndMoveNextCell(&cursor, "Area\nPercentage\n(%)");
    insertHeaderAndMoveNextCell(&cursor, "Area\n\n(μm^2)");
    insertHeaderAndMoveNextCell(&cursor, "Perimeter\n\n(μm)");
    insertHeaderAndMoveNextCell(&cursor, "Diameter\n\n(μm)");
    insertHeaderAndMoveNextCell(&cursor, "Flattening");
    insertHeaderAndMoveNextCell(&cursor, "Intercept\n\n(μm)");
    insertHeaderAndMoveNextCell(&cursor, "Grain Size Number\n(Area Method)\n(G)");
    insertHeaderAndMoveNextCell(&cursor, "Grain Size Number\n(Intercept Method)\n(G)");
    for (int i = 0; i < m_analyser->classCount(); ++i) {
        insertTextAndMoveNextCell(&cursor, m_analyser->getClassesList()->at(i));
        insertTextAndMoveNextCell(&cursor, m_analyser->getCountOfClass(i));
        insertTextAndMoveNextCell(&cursor, m_analyser->getAvgPercentOfClass(i));
        insertTextAndMoveNextCell(&cursor, m_analyser->getAvgAreaOfClass(i));
        insertTextAndMoveNextCell(&cursor, m_analyser->getAvgPerimeterOfClass(i));
        insertTextAndMoveNextCell(&cursor, m_analyser->getAvgDiameterOfClass(i));
        insertTextAndMoveNextCell(&cursor, m_analyser->getAvgFlatteningOfClass(i));
        insertTextAndMoveNextCell(&cursor, m_analyser->getAvgInterLengthOfClass(i));
        insertTextAndMoveNextCell(&cursor, m_analyser->getSizeNumberByAreaOfClass(i));
        insertTextAndMoveNextCell(&cursor, m_analyser->getSizeNumberByInterceptOfClass(i));
    }
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock(alignCentreBlockFormat(), romanFormat());
    cursor.insertHtml("<br />");
    cursor.insertText("Table 1. Analysis results (average values)", figureInfoFormat());
    cursor.insertHtml("<br /><br />");

    //insert the detailed table
    cursor.insertTable(m_analyser->count() + 1, 6, tableFormat);
    insertHeaderAndMoveNextCell(&cursor, "Index");
    insertHeaderAndMoveNextCell(&cursor, "Class");
    insertHeaderAndMoveNextCell(&cursor, "Area\n(μm^2)");
    insertHeaderAndMoveNextCell(&cursor, "Perimeter\n(μm)");
    insertHeaderAndMoveNextCell(&cursor, "Diameter\n(μm)");
    insertHeaderAndMoveNextCell(&cursor, "Flattening");
    for (int i = 0; i < m_analyser->count(); ++i) {
        insertTextAndMoveNextCell(&cursor, QString::number(i + 1));
        insertTextAndMoveNextCell(&cursor, m_analyser->getClassNameAt(i));
        insertTextAndMoveNextCell(&cursor, m_analyser->getAreaAt(i));
        insertTextAndMoveNextCell(&cursor, m_analyser->getPerimeterAt(i));
        insertTextAndMoveNextCell(&cursor, m_analyser->getDiameterAt(i));
        insertTextAndMoveNextCell(&cursor, m_analyser->getFlatteningAt(i));
    }
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock(alignCentreBlockFormat(), romanFormat());
    cursor.insertHtml("<br />");
    cursor.insertText("Table 2. Details of each object", figureInfoFormat());
    cursor.insertHtml("<br />");

    tb->setDocument(&textDoc);
    emit workStatusStrUpdated("Report Generated.");
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

QTextCharFormat Reporter::cambriaMathFormat()
{
    QTextCharFormat mf;
    mf.setFontFamily("Cambria Math");
    mf.setFontPointSize(10);
    mf.setFontWeight(QFont::Normal);
    return mf;
}

QTextCharFormat Reporter::cambriaMathBoldFormat()
{
    QTextCharFormat cf;
    cf.setFontFamily("Cambria Math");
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
    ff.setFontPointSize(9);
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
    emit workStatusStrUpdated("PDF exported as " + filename);
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
        emit workStatusStrUpdated("Exported Successfully.");
    }
    else {
        emit workStatusStrUpdated("Export Failed.");
    }
}
