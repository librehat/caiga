#include "parametersdialog.h"
#include "ui_parametersdialog.h"

ParametersDialog::ParametersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParametersDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Tool);
    kSizeText = "Kernel Size";
    connect(ui->kSizeSlider, &QSlider::valueChanged, this, &ParametersDialog::onSliderValueChanged);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ParametersDialog::onAcceptButtonPressed);
}

ParametersDialog::~ParametersDialog()
{
    delete ui;
}

void ParametersDialog::onSliderValueChanged(int s)
{
    ui->kSizeLabel->setText(kSizeText + "(" + QString::number(s * 2 - 1) + ")");
}

void ParametersDialog::setSigColourText(const QString &c)
{
    ui->sigmaColourLabel->setText(c);
}

void ParametersDialog::setSigSpaceText(const QString &s)
{
    ui->sigmaSpaceLabel->setText(s);
}

void ParametersDialog::setkSizeText(const QString &k)
{
    kSizeText = k;
    ui->kSizeLabel->setText(k);
}

void ParametersDialog::onAcceptButtonPressed()
{
    emit parametersAccepted(ui->kSizeSlider->value() * 2 - 1, ui->sigmaSpace->value(), ui->sigmaColour->value());
}
