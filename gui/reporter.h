#ifndef REPORTER_H
#define REPORTER_H

#include <QObject>
#include "workspace.h"
#include "analyser.h"
#include "3rdparty/qcustomplot.h"

class Reporter : public QObject
{
    Q_OBJECT
public:
    explicit Reporter(CAIGA::Analyser *analyser, CAIGA::WorkSpace *workSpace, int split, QObject *parent = 0);
    ~Reporter() {}
    void setBarChart(QCustomPlot *plot);
    void setTextBrowser(QTextBrowser *tb);

private:
    CAIGA::Analyser *m_analyser;
    CAIGA::WorkSpace *m_workSpace;
    QTextDocument *textDoc;
    int split;
    qreal maxRadius;

    static QTextBlockFormat alignCentreBlockFormat();
    static QTextBlockFormat alignJustifyBlockFormat();
    static QTextCharFormat boldRomanFormat();
    static QTextCharFormat cambriaFormat();

    inline void insertTextAndMoveNextCell(QTextCursor *cursor, const QString &text) { cursor->insertText(text); cursor->movePosition(QTextCursor::NextCell); }
    inline void insertHtmlAndMoveNextCell(QTextCursor *cursor, const QString &html) { cursor->insertHtml(html); cursor->movePosition(QTextCursor::NextCell); }

    QTextTableFormat tableFormat;
};

#endif // REPORTER_H
