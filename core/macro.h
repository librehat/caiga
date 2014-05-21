#ifndef MACRO_H
#define MACRO_H

#include <QObject>
#include "core_lib.h"
#include "ccspace.h"

namespace CAIGA
{
class CORE_LIB Macro : public QObject
{
    Q_OBJECT
public:
    explicit Macro(QObject *parent = 0);
    inline void setCCSpace(CCSpace *c) { m_ccSpace = c; }
    void doCropAndCalibreMacroFromFile(const QString &);
    void saveCropAndCalibreAsMacroFile(const QString &);

private:
    CCSpace *m_ccSpace;
};
}

#endif // MACRO_H
