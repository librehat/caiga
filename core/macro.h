#ifndef MACRO_H
#define MACRO_H

#include <QObject>
#include "core_lib.h"
#include "calibrespace.h"

namespace CAIGA
{
class CORE_LIB Macro : public QObject
{
    Q_OBJECT
public:
    explicit Macro(CalibreSpace *c, QObject *parent = 0);
    void doCalibreMacroFromFile(const QString &);
    void saveCalibreAsMacroFile(const QString &);

private:
    CalibreSpace *m_calibreSpace;
};
}

#endif // MACRO_H
