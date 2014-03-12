/*
 * This is an abstract base class for all "Analyser".
 */

#ifndef ABSTRACTANALYSER_H
#define ABSTRACTANALYSER_H

#include "core_lib.h"
#include <QString>
#include <QImage>

namespace CAIGA
{
class CORE_LIB AbstractAnalyser
{
public:
    virtual bool isAnalysed() const = 0;
    virtual bool Analyse(QImage &) const = 0;
    virtual QString getResult() const = 0;
};

}
#endif // ABSTRACTANALYSER_H
