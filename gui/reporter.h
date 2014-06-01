#ifndef REPORTER_H
#define REPORTER_H

#include <QObject>
#include <QTextDocumentWriter>
#include "workspace.h"
#include "analyser.h"
#include "3rdparty/qcustomplot.h"
#include "3rdparty/qcpdocumentobject.h"

class Reporter : public QObject
{
    Q_OBJECT
public:
    explicit Reporter(CAIGA::Analyser *analyser, CAIGA::WorkSpace *workSpace, int s, const QImage &raw, QObject *parent = 0);
    ~Reporter() {}
    void setBarChart(QCustomPlot *plot);
    void generateReport();
    void exportAsPDF(QString &filename);
    void exportAsFormat(QString &filename, const QByteArray &format);

    void setTextBrowser();

signals:
    void workStatusStrUpdated(const QString &);
    void reportGenerated(QTextDocument *);
    void reportAvailable(bool);

private:
    QCustomPlot *m_plot;
    CAIGA::Analyser *m_analyser;
    CAIGA::WorkSpace *m_workSpace;
    QTextDocument textDoc;
    QTextCursor cursor;
    QImage rawImage;
    int split;
    qreal maxRadius;

    static QTextBlockFormat alignCentreBlockFormat();
    static QTextBlockFormat alignJustifyBlockFormat();
    static QTextCharFormat boldRomanFormat();
    static QTextCharFormat arialBoldFormat();
    static QTextCharFormat romanFormat();
    static QTextCharFormat figureInfoFormat();

    inline void insertTextAndMoveNextCell(QTextCursor *cursor, const QString &text) { cursor->insertText(text, romanFormat()); cursor->movePosition(QTextCursor::NextCell); }
    inline void insertHeaderAndMoveNextCell(QTextCursor *cursor, const QString &text) { cursor->insertText(text, arialBoldFormat()); cursor->movePosition(QTextCursor::NextCell); }
    inline void insertHtmlAndMoveNextCell(QTextCursor *cursor, const QString &html) { cursor->insertHtml(html); cursor->movePosition(QTextCursor::NextCell); }

    QTextTableFormat tableFormat;
};

#endif // REPORTER_H
