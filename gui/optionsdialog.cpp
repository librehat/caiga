#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include <QColorDialog>

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);

    this->readConfigFile();
    confChanged = false;

    connect(this, &OptionsDialog::accepted, this, &OptionsDialog::optionsChanged);
    connect(this, &OptionsDialog::optionsAccepted, this, &OptionsDialog::writeConfigFile);
    connect(ui->toolbarStyleBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &OptionsDialog::configsChanged);
    connect(ui->tabPositionBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &OptionsDialog::configsChanged);
    connect(ui->penColourEdit, &QLineEdit::textChanged, this, &OptionsDialog::configsChanged);
    connect(ui->penColourToolButton, &QToolButton::clicked, this, &OptionsDialog::pickColour);
    connect(ui->undoStepsSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &OptionsDialog::configsChanged);
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

void OptionsDialog::optionsChanged()
{
    if (confChanged) {
        emit optionsAccepted(ui->toolbarStyleBox->currentIndex(), ui->tabPositionBox->currentIndex(), ui->penColourEdit->text(), ui->undoStepsSpinBox->value());
    }
}

void OptionsDialog::writeConfigFile(int toolbarStyle, int tabPos, const QString &colour, int undoSteps)
{
    settings.setValue("Toolbar Style", toolbarStyle);
    settings.setValue("Tab Position", tabPos);
    settings.setValue("Pen Colour", colour);
    settings.setValue("Undo Steps", undoSteps);
}

void OptionsDialog::readConfigFile()
{
    ui->toolbarStyleBox->setCurrentIndex(settings.value("Toolbar Style").toInt());
    ui->tabPositionBox->setCurrentIndex(settings.value("Tab Position", 1).toInt());//bottom (1) by default
    ui->penColourEdit->setText(settings.value("Pen Colour", "#F00").toString());
    ui->undoStepsSpinBox->setValue(settings.value("Undo Steps", 10).toInt());
}

void OptionsDialog::pickColour()
{
    QColor c = QColorDialog::getColor(QColor(ui->penColourEdit->text()), this);
    if (c.isValid()) {
        ui->penColourEdit->setText(c.name());
    }
}
