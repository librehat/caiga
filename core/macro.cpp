#include "macro.h"
using namespace CAIGA;

Macro::Macro(QObject *parent) :
    QObject(parent)
{
    m_space = NULL;
    m_img = NULL;
}

void Macro::setImageObject(Image *img)
{
    m_img = img;
}

void Macro::setWorkSpace(WorkSpace *wk)
{
    m_space = wk;
}

void Macro::doCropAndCalibreMacroFromFile(const QString &)
{
    //TODO
}

void Macro::doWorkMacroFromFile(const QString &)
{
    //TODO
}
