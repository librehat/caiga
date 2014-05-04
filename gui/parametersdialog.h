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
    void parametersChanged(int, double, double, bool);

public:
    explicit ParametersDialog(QWidget *parent = 0);
    ~ParametersDialog();
    void show();
    void setkSizeText(const QString &);
    void setSigColourText(const QString &);
    void setSigSpaceText(const QString &);
    void setMode(int mode);//should invoke only once because it won't change the default value which may be modified previously

public slots:
    void handleWorkStarted();
    void handleWorkFinished();

private:
    Ui::ParametersDialog *ui;
    QString kSizeText;

private slots:
    void onValuesChanged();
    void onSliderValueChanged(int);
};

#endif // PARAMETERSDIALOG_H
