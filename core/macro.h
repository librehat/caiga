#ifndef MACRO_H
#define MACRO_H

#include <QObject>
#include "core_lib.h"
#include "image.h"
#include "workspace.h"

namespace CAIGA
{
class CORE_LIB Macro : public QObject
{
    Q_OBJECT
public:
    explicit Macro(QObject *parent = 0);
    void setImageObject(Image *);
    void setWorkSpace(WorkSpace *);
    void doCropAndCalibreMacroFromFile(const QString &);
    void doWorkMacroFromFile(const QString &);

signals:
    void workStatusUpdated(const QString &);
    void cropAndCalibreMacroFinished();
    void workMacroFinished();

public slots:

private:
    WorkSpace *m_space;
    Image *m_img;
};
}

#endif // MACRO_H
