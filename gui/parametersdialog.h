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
    void parametersAccepted(int, double, double, bool);

public:
    explicit ParametersDialog(QWidget *parent = 0);
    ~ParametersDialog();
    void setkSizeText(const QString &);
    void setSigColourText(const QString &);
    void setSigSpaceText(const QString &);
    void setMode(int mode);//should invoke only once because it won't change the default value which may be modified previously

private:
    Ui::ParametersDialog *ui;
    QString kSizeText;

private slots:
    void onSliderValueChanged(int);
    void onAcceptButtonPressed();
};

#endif // PARAMETERSDIALOG_H
