#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QSettings>

#include "caigaglobal.h"

namespace Ui
{
class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent = 0);
    ~OptionsDialog();

signals:
    void optionsAccepted(int, int, int, bool, int);

private slots:
    void optionsChanged();
    void writeConfigFile(int, int, int, bool, int);
    void configsChanged(int);

private:
    Ui::OptionsDialog *ui;

    QSettings settings;
    void readConfigFile();
    bool confChanged;
};

#endif // OPTIONSDIALOG_H
