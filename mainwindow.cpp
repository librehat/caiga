#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cameradialog.h"
#include "optionsdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//Windows should use packaged theme since its lacking of **theme**
#if defined(_WIN32)
    QIcon::setThemeName("Oxygen");
#endif

    /*
     * Currently Qt won't load svg-format theme icons.
     */
    ui->actionNew_Project->setIcon(QIcon::fromTheme("document-new"));
    ui->actionOpen_Project->setIcon(QIcon::fromTheme("document-open-folder"));
    ui->actionRecent->setIcon(QIcon::fromTheme("document-open-recent"));
    ui->actionSave_Project->setIcon(QIcon::fromTheme("document-save"));
    ui->actionSave_Project_As->setIcon(QIcon::fromTheme("document-save-as"));
    ui->actionRevert_Project_to_Saved->setIcon(QIcon::fromTheme("document-revert"));
    ui->actionClose_Project->setIcon(QIcon::fromTheme("document-close"));
    ui->actionQuit->setIcon(QIcon::fromTheme("application-exit"));
    ui->actionProject_Properties->setIcon(QIcon::fromTheme("document-properties"));
    ui->actionExportImg_As->setIcon(QIcon::fromTheme("document-export"));
    ui->actionExportPro_As->setIcon(QIcon::fromTheme("document-export"));
    ui->actionRedo->setIcon(QIcon::fromTheme("edit-redo"));
    ui->actionUndo->setIcon(QIcon::fromTheme("edit-undo"));
    ui->actionAddImgDisk_toolbar->setIcon(QIcon::fromTheme("document-open"));
    ui->actionAddImgDisk->setIcon(QIcon::fromTheme("document-open"));
    ui->actionAddImgCamera->setIcon(QIcon::fromTheme("camera-photo"));
    ui->actionOptions->setIcon(QIcon::fromTheme("configure"));
    ui->actionHelp->setIcon(QIcon::fromTheme("help-contents"));
    ui->actionAbout_CAIGA->setIcon(QIcon::fromTheme("documentinfo"));

    /*
     * SIGNALs and SLOTs
     * Only those that cannot be connected in Deisgner should be defined below
     */
    connect(ui->actionOpen_Project, SIGNAL(triggered()), this, SLOT(openProjectDialog()));
    connect(ui->actionSave_Project_As, SIGNAL(triggered()), this, SLOT(saveProjectDialog()));
    connect(ui->actionAddImgDisk, SIGNAL(triggered()), this, SLOT(addDiskFileDialog()));
    connect(ui->actionAddImgDisk_toolbar, SIGNAL(triggered()), this, SLOT(addDiskFileDialog()));
    connect(ui->actionAddImgCamera, SIGNAL(triggered()), this, SLOT(createCameraDialog()));
    connect(ui->actionOptions, SIGNAL(triggered()), this, SLOT(createOptionsDialog()));
    connect(ui->actionExportImg_As, SIGNAL(triggered()), this, SLOT(exportImgDialog()));
    connect(ui->actionExportPro_As, SIGNAL(triggered()), this, SLOT(exportProDialog()));
    connect(ui->actionProject_Properties, SIGNAL(triggered()), this, SLOT(projectPropertiesDialog()));
    connect(ui->actionAbout_Qt, SIGNAL(triggered()), this, SLOT(aboutQtDialog()));
    connect(ui->actionAbout_CAIGA, SIGNAL(triggered()), this, SLOT(aboutCAIGADialog()));
    connect(ui->imageList, SIGNAL(activated(QModelIndex)), this, SLOT(setActivateImage(QModelIndex)));

    connect(this, SIGNAL(configReadFinished(int,int,int,bool,int)), this,
            SLOT(updateOptions(int,int,int,bool,int)));

    //underlying work
    imgNameModel = new QStringListModel();
    ui->imageList->setModel(imgNameModel);

    readConfig();
}

QString MainWindow::aboutText = QString("<h3>Computer-Aid Interactive Grain Analyser</h3><p>Version Pre Alpha on ")

#if defined(_WIN32)//_WIN32 is defined for both 32-bit and 64-bit environment
        + QString("Windows")
#elif defined(__linux__)
        + QString("Linux")
#elif defined(__APPLE__) && defined(__MACH__)
        + QString("Mac OS")
#elif defined(BSD)
        + QString("BSD")
#elif defined(__ANDROID__)
        + QString("Android")
#elif defined(__unix__)
        + QString("UNIX")
#else
        + QString("Unknown platform")
#endif
        + QString("</p><p>Built on ") + QString(__DATE__) + QString(" at ") + QString(__TIME__) + QString(" using ")
#if defined(__GNUC__)
        + QString("GNU Compiler Collection ") + QString::number(__GNUC__) + QString(".") + QString::number(__GNUC_MINOR__) + QString(".") + QString::number(__GNUC_PATCHLEVEL__)
#elif defined(_MSC_VER)
        + QString("Microsoft Visual C++ Compiler")
#elif defined(__ICC) || defined(__INTEL_COMPILER)
        + QString("Intel C++ Compiler ") + QString::number(__INTEL_COMPILER)
#elif defined(__clang__)
        + QString("Clang ") + QString(__clang_version__)
#else
        + QString("Unkown compiler")
#endif

        + QString("</p><p>Copyright &copy; 2014 William Wong and other contributors.</p><p>Licensed under <a href='http://en.wikipedia.org/wiki/MIT_License'>The MIT License</a></p><p>THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.</p>");

MainWindow::~MainWindow()
{
    delete ui;
    delete imgNameModel;
}

void MainWindow::openProjectDialog()//TODO
{
    QString filename = QFileDialog::getOpenFileName(this, "Open Project", QDir::currentPath(),
                                                    "CAIGA Project (*.caigap)");
    if (filename.isNull()) {
        return;
    }
    CAIGA::setCurrentDir(filename);
}

void MainWindow::saveProjectDialog()//TODO
{
    QString filename = QFileDialog::getSaveFileName(this, "Save Project As", QDir::currentPath(),
                                                    "CAIGA Project (*.caigap)");
    if (filename.isNull()) {
        return;
    }
    CAIGA::setCurrentDir(filename);
}

void MainWindow::addDiskFileDialog()//TODO
{
    QString filename = QFileDialog::getOpenFileName(this, "Add Image from Disk", QDir::currentPath(),
                                                    "Images (*.png *.jpg *.jpeg *.tiff *.bmp *.gif *.xpm)");
    if (filename.isNull()) {
        return;
    }
    CAIGA::setCurrentDir(filename);
    int row = imgNameModel->rowCount();
    //insertRow: Inserts a single row before the given row in the child items of the parent specified.
    imgNameModel->insertRow(row);
    imgNameModel->setData(imgNameModel->index(row), QVariant(filename));
}

void MainWindow::createCameraDialog()//TODO camera image should be involved with SQLite
{
    CameraDialog camDlg;
    camDlg.exec();
}

void MainWindow::createOptionsDialog()//TODO
{
    OptionsDialog optDlg;
    connect(&optDlg, SIGNAL(optionsAccepted(int,int,int,bool,int)), this,
            SLOT(updateOptions(int,int,int,bool,int)));
    optDlg.exec();
}

void MainWindow::exportImgDialog()//TODO
{
    QString filename = QFileDialog::getSaveFileName(this,
                       "Export Image Information As",
                       QDir::currentPath(),
                       "Adobe Portable Document Format (*.pdf);;Open Document File (*.odf);;HTML Document (*.html)");
    if (filename.isNull()) {
        return;
    }
    CAIGA::setCurrentDir(filename);
}

void MainWindow::exportProDialog()//TODO
{
    QString filename = QFileDialog::getSaveFileName(this ,"Export Project Information As", QDir::currentPath(),
                       "Comma-Separated Values (*.csv);;Microsoft Office Open XML Workbook (*.xlsx)");
    if (filename.isNull()) {
        return;
    }
    CAIGA::setCurrentDir(filename);
}

void MainWindow::projectPropertiesDialog()//TODO
{
    QMessageBox proDlg;
    proDlg.setWindowIcon(QIcon::fromTheme("document-properties"));
    proDlg.setWindowTitle("Properties");
    proDlg.setText("<strong>TODO</strong>");
    proDlg.setStandardButtons(QMessageBox::Ok);
    proDlg.exec();
}

void MainWindow::aboutQtDialog()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::aboutCAIGADialog()
{
    QMessageBox::about(this, "About CAIGA", aboutText);
}

void MainWindow::setActivateImage(QModelIndex i)
{
    QString imgfile = imgNameModel->data(i, Qt::DisplayRole).toString();
    QPixmap a(imgfile);
    ui->origLabel->setPixmap(a);
}

void MainWindow::updateOptions(int lang, int toolbarStyle, int tabPos, bool autoSave, int interval)
{
    Q_UNUSED(lang);//TODO: i18n

    switch(toolbarStyle) {
    case 0://<system>
        ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonFollowStyle);
        break;
    case 1://icon-only
        ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
        break;
    case 2://text below
        ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        break;
    case 3://text beside
        ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        break;
    default:
        qWarning("Invalid Toolbar Style");
    }

    switch(tabPos) {
    case 0://top
        ui->imageTabs->setTabPosition(QTabWidget::North);
        break;
    case 1://bottom
        ui->imageTabs->setTabPosition(QTabWidget::South);
        break;
    case 2://left
        ui->imageTabs->setTabPosition(QTabWidget::West);
        break;
    case 3://right
        ui->imageTabs->setTabPosition(QTabWidget::East);
        break;
    default:
        qWarning("Invalid Tab Position");
    }

    Q_UNUSED(autoSave);//TODO: project saving
    Q_UNUSED(interval);
}

void MainWindow::readConfig()
{
    emit configReadFinished(settings.value("Language").toInt(),
                            settings.value("Toolbar Style").toInt(),
                            settings.value("Tab Position", 1).toInt(),
                            settings.value("AutoSave", false).toBool(),
                            settings.value("AutoSave Interval", 5).toInt());
}
