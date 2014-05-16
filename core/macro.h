#ifndef MACRO_H
#define MACRO_H

#include <QObject>
#include "core_lib.h"
#include "ccspace.h"
#include "workspace.h"

namespace CAIGA
{
class CORE_LIB Macro : public QObject
{
    Q_OBJECT
public:
    explicit Macro(QObject *parent = 0);
    inline void setCCSpace(CCSpace *c) { m_ccSpace = c; }
    inline void setWorkSpace(WorkSpace *w) { m_space = w; }
    void doCropAndCalibreMacroFromFile(const QString &);
    void doWorkMacroFromFile(const QString &);

signals:
    void workStatusUpdated(const QString &);
    void cropAndCalibreMacroFinished();
    void workMacroFinished();

public slots:

private:
    WorkSpace *m_space;
    CCSpace *m_ccSpace;
};
}

#endif // MACRO_H
