#ifndef PARAMETERSDIALOG_H
#define PARAMETERSDIALOG_H

#include <QDialog>
#include <workbase.h>

namespace Ui {
class ParametersDialog;
}

class ParametersDialog : public QDialog
{
    Q_OBJECT

signals:
    void parametersChanged(int, double, double, bool);
    void undoButtonClicked();
    void redoButtonClicked();
    void resetButtonClicked();

public:
    explicit ParametersDialog(CAIGA::WorkBase::WorkTypes mode, QWidget *parent = 0);
    ~ParametersDialog();
    void show();
    void setkSizeText(const QString &);
    void setSigColourText(const QString &c);
    void setSigSpaceText(const QString &);

public slots:
    void handleWorkStarted();
    void handleWorkFinished();

private:
    Ui::ParametersDialog *ui;
    QString kSizeText;
    void setMode(CAIGA::WorkBase::WorkTypes mode);//should invoke only once because it won't change the default value which may be modified previously

private slots:
    void onValuesChanged();
    void onSliderValueChanged(int);
};

#endif // PARAMETERSDIALOG_H
