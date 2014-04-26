#ifndef PARAMETERSDIALOG_H
#define PARAMETERSDIALOG_H

#include <QDialog>

namespace Ui {
class ParametersDialog;
}

class ParametersDialog : public QDialog
{
    Q_OBJECT

signals:
    void parametersAccepted(int, double, double);

public:
    explicit ParametersDialog(QWidget *parent = 0);
    ~ParametersDialog();
    void setkSizeText(const QString &);
    void setSigColourText(const QString &);
    void setSigSpaceText(const QString &);

private:
    Ui::ParametersDialog *ui;
    QString kSizeText;

private slots:
    void onSliderValueChanged(int);
    void onAcceptButtonPressed();
};

#endif // PARAMETERSDIALOG_H
