#include <QCloseEvent>
#include <QKeyEvent>
#include "parametersdialog.h"
#include "ui_parametersdialog.h"

ParametersDialog::ParametersDialog(CAIGA::WorkBase::WorkTypes mode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParametersDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::Tool);
    connect(ui->kSizeSlider, &QSlider::valueChanged, this, &ParametersDialog::onSliderValueChanged);
    connect(ui->kSizeSlider, &QSlider::valueChanged, this, &ParametersDialog::onValuesChanged);
    connect(ui->sigmaColour, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ParametersDialog::onValuesChanged);
    connect(ui->sigmaSpace, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ParametersDialog::onValuesChanged);
    connect(ui->checkBox, &QCheckBox::stateChanged, this, &ParametersDialog::onValuesChanged);
    connect(ui->undoButton, &QPushButton::clicked, this, &ParametersDialog::undoButtonClicked);
    connect(ui->redoButton, &QPushButton::clicked, this, &ParametersDialog::redoButtonClicked);
    connect(ui->resetButton, &QPushButton::clicked, this, &ParametersDialog::resetButtonClicked);
    ui->undoButton->setVisible(false);
    ui->redoButton->setVisible(false);
    ui->resetButton->setVisible(false);
    this->setMode(mode);
}

ParametersDialog::~ParametersDialog()
{
    delete ui;
}

void ParametersDialog::show()
{
    QWidget::show();
    this->onValuesChanged();
}

void ParametersDialog::onSliderValueChanged(int s)
{
    ui->kSizeLabel->setText(kSizeText + "(" + QString::number(s * 2 - 1) + ")");
}

void ParametersDialog::onValuesChanged()
{
    emit parametersChanged(ui->kSizeSlider->value() * 2 - 1, ui->sigmaSpace->value(), ui->sigmaColour->value(), ui->checkBox->isChecked());
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
    this->onSliderValueChanged(ui->kSizeSlider->value());
}

void ParametersDialog::setMode(CAIGA::WorkBase::WorkTypes mode)
{
    switch (mode) {
    case CAIGA::WorkBase::BoxFilter:
    case CAIGA::WorkBase::MedianBlur:
    case CAIGA::WorkBase::Gradient:
        this->setkSizeText(tr("Kernel Size"));
        ui->kSizeSlider->setMinimum(2);
        ui->sigmaColour->setVisible(false);
        ui->sigmaColourLabel->setVisible(false);
        ui->sigmaSpace->setVisible(false);
        ui->sigmaSpaceLabel->setVisible(false);
        ui->checkBox->setVisible(false);
        break;
    case CAIGA::WorkBase::Binaryzation:
        this->setkSizeText(tr("Block Size"));
        ui->kSizeSlider->setMinimum(2);
        ui->sigmaSpace->setMinimum(-999);
        ui->sigmaSpaceLabel->setText(tr("Constant C"));
        ui->sigmaColour->setVisible(false);
        ui->sigmaColourLabel->setVisible(false);
        break;
    case CAIGA::WorkBase::Canny:
        this->setkSizeText(tr("Aperture Size"));
        ui->kSizeSlider->setMinimum(2);
        ui->kSizeSlider->setMaximum(4);
        ui->sigmaSpaceLabel->setText(tr("High Threshold"));
        ui->sigmaSpace->setValue(400);
        ui->sigmaColourLabel->setText(tr("Low Threshold"));
        ui->sigmaColour->setValue(150);
        ui->checkBox->setText("L2 Gradient");
        break;
    case CAIGA::WorkBase::FloodFill:
        ui->kSizeLabel->setVisible(false);
        ui->kSizeSlider->setVisible(false);
        ui->sigmaSpace->setMinimum(0);
        ui->sigmaSpace->setValue(20);
        ui->sigmaSpaceLabel->setText(tr("High Difference"));
        ui->sigmaColour->setMinimum(0);
        ui->sigmaColour->setValue(20);
        ui->sigmaColourLabel->setText(tr("Low Difference"));
        ui->checkBox->setText(tr("8 Connectivity Mode"));
        ui->undoButton->setVisible(true);
        ui->redoButton->setVisible(true);
        ui->resetButton->setVisible(true);
        break;
    default://adaptiveBilateralFilter
        this->setkSizeText(tr("Kernel Size"));
        ui->kSizeSlider->setValue(6);
        ui->checkBox->setVisible(false);
        break;
    }
}

void ParametersDialog::handleWorkStarted()
{
    ui->buttonBox->setEnabled(false);
}

void ParametersDialog::handleWorkFinished()
{
    ui->buttonBox->setEnabled(true);
}

void ParametersDialog::closeEvent(QCloseEvent *e)
{
    if (!ui->buttonBox->isEnabled()) {
        e->ignore();
    }
    else {
        QDialog::closeEvent(e);
    }
}

void ParametersDialog::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape) {
        this->close();
    }
    else {
        QDialog::keyPressEvent(e);
    }
}
