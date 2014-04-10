#include "optionsdialog.h"
#include "ui_optionsdialog.h"

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);
    this->readConfigFile();
    confChanged = false;

    connect(this, &OptionsDialog::accepted, this, &OptionsDialog::optionsChanged);
    connect(this, &OptionsDialog::optionsAccepted, this, &OptionsDialog::writeConfigFile);
    connect(ui->languageBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &OptionsDialog::configsChanged);
    connect(ui->toolbarStyleBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &OptionsDialog::configsChanged);
    connect(ui->tabPositionBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &OptionsDialog::configsChanged);
    connect(ui->autosave, &QCheckBox::stateChanged, this, &OptionsDialog::configsChanged);
    connect(ui->autosaveInterval, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &OptionsDialog::configsChanged);
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

void OptionsDialog::optionsChanged()
{
    if (confChanged) {
        emit optionsAccepted(ui->languageBox->currentIndex(), ui->toolbarStyleBox->currentIndex(),
                             ui->tabPositionBox->currentIndex(), ui->autosave->isChecked(),
                             ui->autosaveInterval->value());
    }
}

void OptionsDialog::writeConfigFile(int lang, int toolbarStyle, int tabPos, bool autoSave, int interval)
{
    settings.setValue("Language", lang);
    settings.setValue("Toolbar Style", toolbarStyle);
    settings.setValue("Tab Position", tabPos);
    settings.setValue("AutoSave", autoSave);
    settings.setValue("AutoSave Interval", interval);
}

void OptionsDialog::readConfigFile()
{
    ui->languageBox->setCurrentIndex(settings.value("Language").toInt());
    ui->toolbarStyleBox->setCurrentIndex(settings.value("Toolbar Style").toInt());
    ui->tabPositionBox->setCurrentIndex(settings.value("Tab Position", 1).toInt());//bottom (1) by default
    ui->autosave->setChecked(settings.value("AutoSave", false).toBool());//off by default
    ui->autosaveInterval->setValue(settings.value("AutoSave Interval", 5).toInt());//5min by default
}

void OptionsDialog::configsChanged(int)
{
    confChanged = true;
}
