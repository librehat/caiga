/*
 * This is an abstract base class for all "Analyser".
 */

#include <QString>
#include <QImage>

#ifndef ABSTRACTANALYSER_H
#define ABSTRACTANALYSER_H
namespace CAIGA
{
class AbstractAnalyser
{
public:
    virtual bool isAnalysed() const = 0;
    virtual bool Analyse(QImage &) const = 0;
    virtual QString getResult() const = 0;
};

}
#endif // ABSTRACTANALYSER_H
