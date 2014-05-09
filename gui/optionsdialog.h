#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QSettings>

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
    void optionsAccepted(int, int, int, const QString &);

private slots:
    void pickColour();
    void optionsChanged();
    void writeConfigFile(int, int, int, const QString &);
    void configsChanged();

private:
    Ui::OptionsDialog *ui;
    QSettings settings;
    void readConfigFile();
    bool confChanged;
};

#endif // OPTIONSDIALOG_H
