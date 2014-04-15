#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cameradialog.h"
#include "optionsdialog.h"
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    restoreGeometry(settings.value("MainGeometry").toByteArray());
    restoreState(settings.value("MainState").toByteArray());
    if (ui->mainToolBar->isHidden()) {
        ui->actionToolbar->setChecked(false);
    }

    QDir::setCurrent(settings.value("CurrentDir").toString());

//Windows should use packaged theme since its lacking of **theme**
#if defined(_WIN32)
    QIcon::setThemeName("Oxygen");
#endif

    /*
     * Setup icons.
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
    ui->actionAddImgDisk->setIcon(QIcon::fromTheme("document-open"));
    ui->actionAddImgCamera->setIcon(QIcon::fromTheme("camera-photo"));
    ui->actionDeleteImg->setIcon(QIcon::fromTheme("edit-delete"));
    ui->actionOptions->setIcon(QIcon::fromTheme("configure"));
    ui->actionHelp->setIcon(QIcon::fromTheme("help-contents"));
    ui->actionAbout_CAIGA->setIcon(QIcon::fromTheme("documentinfo"));

    /*
     * SIGNALs and SLOTs
     * Only those that cannot be connected in Deisgner should be defined below
     */
    connect(ui->buttonGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &MainWindow::ccModeChanged);
    connect(ui->histogramCheckBox, &QCheckBox::stateChanged, this, &MainWindow::histogramCheckBoxStateChanged);
    connect(ui->histogramMethodComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::histogramMethodChanged);
    connect(ui->blurCheckBox, &QCheckBox::stateChanged, this, &MainWindow::blurCheckBoxStateChanged);
    connect(ui->blurMethodComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::blurMethodChanged);
    connect(ui->binaryzationCheckBox, &QCheckBox::stateChanged, this, &MainWindow::binaryzationCheckBoxStateChanged);
    connect(ui->apertureSizeSlider, &QSlider::valueChanged, this, &MainWindow::onEdgesParametersChanged);
    connect(ui->highThresholdDoubleSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::highThresholdChanged);
    connect(ui->lowThresholdDoubleSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::onEdgesParametersChanged);
    connect(ui->l2GradientCheckBox, &QCheckBox::stateChanged, this, &MainWindow::onEdgesParametersChanged);
    connect(ui->edgesButtonBox, &QDialogButtonBox::accepted, this, &MainWindow::saveEdges);
    connect(ui->edgesButtonBox, &QDialogButtonBox::rejected, this, &MainWindow::discardEdges);
    connect(ui->actionNew_Project, &QAction::triggered, this, &MainWindow::newProject);
    connect(ui->actionOpen_Project, &QAction::triggered, this, &MainWindow::openProjectDialog);
    connect(ui->actionSave_Project, &QAction::triggered, this, &MainWindow::saveProject);
    connect(ui->actionSave_Project_As, &QAction::triggered, this, &MainWindow::saveProjectAsDialog);
    connect(ui->actionClose_Project, &QAction::triggered, this, &MainWindow::closeProject);
    connect(ui->actionAddImgDisk, &QAction::triggered, this, &MainWindow::addDiskFileDialog);
    connect(ui->actionAddImgCamera, &QAction::triggered, this, &MainWindow::createCameraDialog);
    connect(ui->actionOptions, &QAction::triggered, this, &MainWindow::createOptionsDialog);
    connect(ui->actionExportImg_As, &QAction::triggered, this, &MainWindow::exportImgDialog);
    connect(ui->actionExportPro_As, &QAction::triggered, this, &MainWindow::exportProDialog);
    connect(ui->actionProject_Properties, &QAction::triggered, this, &MainWindow::projectPropertiesDialog);
    connect(ui->actionAbout_Qt, &QAction::triggered, this, &MainWindow::aboutQtDialog);
    connect(ui->actionAbout_CAIGA, &QAction::triggered, this, &MainWindow::aboutCAIGADialog);
    connect(ui->imageList, &QListView::activated, this, &MainWindow::setActivateImage);

    connect(ui->cropLabel, &QImageDrawer::calibreFinished, this, &MainWindow::onCalibreFinished);

    connect(this, &MainWindow::configReadFinished, this, &MainWindow::updateOptions);

    //underlying work
    imgNameModel = new QStringListModel();
    ui->imageList->setModel(imgNameModel);

    readConfig();
    projectUnsaved = false;
}

QString MainWindow::aboutText = QString("<h3>Computer-Aid Interactive Grain Analyser</h3><p>Version Pre Alpha on ")

#if defined(_WIN32)//_WIN32 is defined for both 32-bit and 64-bit environment
        + QString("Windows")
#elif defined(__linux__)
        + QString("Linux")
#elif defined(__APPLE__) && defined(__MACH__)
        + QString("Mac OS X")
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

        + QString("</p><p>Copyright &copy; 2014 William Wong and other contributors.<br />School of Materials Science and Engineering, Southeast University.</p><p>Licensed under <a href='http://en.wikipedia.org/wiki/MIT_License'>The MIT License</a>.<br /></p><p><strong>The software contains third-party libraries and artworks below.</strong><br /><a href='http://opencv.org'>OpenCV</a> licensed under <a href='https://github.com/Itseez/opencv/blob/master/LICENSE'>3-clause BSD License</a><br /><a href='http://www.oxygen-icons.org'>Oxygen Icons</a> licensed under <a href='http://www.gnu.org/licenses/lgpl-3.0.txt'>LGPLv3 License</a><br /></p><div>THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.</div>");

MainWindow::~MainWindow()
{
    delete ui;
    delete imgNameModel;
}

void MainWindow::ccModeChanged(int i)
{
    ui->cropLabel->setDrawMode(i);
}

void MainWindow::onCalibreFinished(int pixel, double rsize)
{
    cgimg.setCalibre(pixel, rsize);
    ui->calibreDoubleSpinBox->setValue(cgimg.getCalibre());
}

void MainWindow::histogramCheckBoxStateChanged(int)
{
    //TODO
}

void MainWindow::histogramMethodChanged(int)
{
    //TODO
}

void MainWindow::blurCheckBoxStateChanged(int)
{
    //TODO
}

void MainWindow::blurMethodChanged(int)
{
    //TODO
}

void MainWindow::binaryzationCheckBoxStateChanged(int)
{
    //TODO
}

void MainWindow::highThresholdChanged(double ht)
{
    ui->lowThresholdDoubleSpinBox->setMaximum(ht - 1);
    emit onEdgesParametersChanged();
}

void MainWindow::onEdgesParametersChanged()
{
    //TODO: Use Project
    int aSize = (ui->apertureSizeSlider->value() * 2 - 1);

    bool l2 = false;
    if (ui->l2GradientCheckBox->checkState() == Qt::Checked) {
        l2 = true;
    }

    ui->edgesLabel->setPixmap(CAIGA::Image::ImageToCannyedPixmap(cgimg.getRawMatrix(), ui->highThresholdDoubleSpinBox->value(), ui->lowThresholdDoubleSpinBox->value(), aSize, l2));
}

void MainWindow::saveEdges()
{
    cgimg.setEdges(CAIGA::Image::convertQImage2Mat(ui->edgesLabel->pixmap()->toImage()));
}

void MainWindow::discardEdges()
{
    //reset the edges tab
}

void MainWindow::updatePreProcessedImage()
{
    //TODO
}

void MainWindow::savePreProcessedImage()
{
    //TODO
}

void MainWindow::discardPreProcessedImage()
{
    //TODO
}

void MainWindow::newProject()
{
    switch (unSavedProject()) {
    case 2:
        return;
        break;
    default:
        project.close();
    }

    project = CAIGA::Project();
    this->setWidgetsEnabled(true);
    projectUnsaved = false;
}

void MainWindow::openProjectDialog()//TODO
{
    switch (unSavedProject()) {
    case 2:
        return;
        break;
    default:
        project.close();
    }

    QString filename = QFileDialog::getOpenFileName(this, "Open Project", QDir::currentPath(),
                                                    "CAIGA Project (*.caigap)");
    if (filename.isNull()) {
        return;
    }
    setCurrentDirbyFile(filename);

    if(project.setDataBase(filename)) {
        this->setWidgetsEnabled(true);
        projectUnsaved = false;
    }
}

void MainWindow::saveProject()//TODO
{
    if (projectUnsaved) {
        saveProjectAsDialog();
    }
    projectUnsaved = false;
}

void MainWindow::saveProjectAsDialog()//TODO
{
    QString filename = QFileDialog::getSaveFileName(this, "Save Project As", QDir::currentPath(),
                                                    "CAIGA Project (*.caigap)");
    if (filename.isNull()) {
        return;
    }
    setCurrentDirbyFile(filename);
    projectUnsaved = false;
}

void MainWindow::closeProject()
{
    switch (unSavedProject()) {
    case 2://Cancel
        return;//ignore
        break;
    default:
        project.close();
        setWidgetsEnabled(false);
        projectUnsaved = false;
    }
}

void MainWindow::addDiskFileDialog()//TODO
{
    QString filename = QFileDialog::getOpenFileName(this, "Add Image from Disk", QDir::currentPath(),
                       "Supported Images (*.png *.jpg *.jpeg *.tif *.tiff *.bmp)");
    if (filename.isNull()) {
        return;
    }
    setCurrentDirbyFile(filename);
    int row = imgNameModel->rowCount();
    //insertRow: Inserts a single row before the given row in the child items of the parent specified.
    imgNameModel->insertRow(row);
    imgNameModel->setData(imgNameModel->index(row, 0), QVariant(filename));
}

void MainWindow::createCameraDialog()//TODO camera image should be involved with SQLite
{
    CameraDialog camDlg;
    camDlg.exec();
}

void MainWindow::createOptionsDialog()
{
    OptionsDialog optDlg;
    connect(&optDlg, &OptionsDialog::optionsAccepted, this, &MainWindow::updateOptions);
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
    setCurrentDirbyFile(filename);
}

void MainWindow::exportProDialog()//TODO
{
    QString filename = QFileDialog::getSaveFileName(this ,"Export Project Information As",
                       QDir::currentPath(), "Open Document SpreadSheet (*.ods)");
    if (filename.isNull()) {
        return;
    }
    setCurrentDirbyFile(filename);
}

void MainWindow::projectPropertiesDialog()//TODO
{
    QMessageBox proDlg(this);
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
    cgimg.setRawImage(imgfile);
    ui->rawLabel->setPixmap(cgimg.getRawPixmap());

    /*
     * reverse priority to avoid corruption.
     */
    if (cgimg.isProcessed()) {
        ui->analysisLabel->setPixmap(cgimg.getProcessedPixmap());
    }
    else {
        ui->analysisLabel->setPixmap(cgimg.getEdgesPixmap());
    }

    if (cgimg.hasEdges()) {
        ui->analysisTab->setEnabled(true);
    }
    else {
        emit onEdgesParametersChanged();//Initialise a premature edges image
        ui->analysisTab->setEnabled(false);
    }

    if (cgimg.isPreProcessed()) {
        ui->preProcessLabel->setPixmap(cgimg.getPreProcessedPixmap());
        ui->edgesTab->setEnabled(true);
    }
    else {
        ui->preProcessLabel->setPixmap(cgimg.getCroppedPixmap());
        ui->preProcessTab->setEnabled(false);
    }

    if (cgimg.isCropped()) {
        ui->cropLabel->setPixmap(cgimg.getCroppedPixmap());
        ui->preProcessTab->setEnabled(true);
    }
    else {
        ui->cropLabel->setPixmap(cgimg.getRawPixmap());
        ui->preProcessTab->setEnabled(false);
    }
}

void MainWindow::updateOptions(int lang, int toolbarStyle, int tabPos, bool autoSave, int interval, const QString &colour)
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

    ui->cropLabel->setPenColour(colour);
}

void MainWindow::readConfig()
{
    emit configReadFinished(settings.value("Language").toInt(),
                            settings.value("Toolbar Style").toInt(),
                            settings.value("Tab Position", 1).toInt(),
                            settings.value("AutoSave", false).toBool(),
                            settings.value("AutoSave Interval", 5).toInt(),
                            settings.value("Pen Colour", "#F00").toString());
}

void MainWindow::setWidgetsEnabled(bool b)
{
    ui->centralWidget->setEnabled(b);
    ui->actionSave_Project->setEnabled(b);
    ui->actionSave_Project_As->setEnabled(b);
    ui->actionRevert_Project_to_Saved->setEnabled(b);
    ui->actionClose_Project->setEnabled(b);
    ui->actionExportImg_As->setEnabled(b);
    ui->actionExportPro_As->setEnabled(b);
    ui->actionProject_Properties->setEnabled(b);
    ui->menuImage->setEnabled(b);
    ui->mainToolBar->setEnabled(b);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    switch (unSavedProject()) {
    case 2:
        event->ignore();
        break;
    default:
        settings.setValue("MainGeometry", saveGeometry());//save MainWindow geometry
        settings.setValue("MainState", saveState());//save MainWindow toolbar state
        settings.setValue("CurrentDir", QDir::currentPath());
        event->accept();
    }
}

int MainWindow::unSavedProject()
{
    if (projectUnsaved) {
        int Ans = QMessageBox::question(this, "Unsaved Project",
                  "Do you want to save this unsaved project?",
                  QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
        switch (Ans) {
        case QMessageBox::No:
            return 0;
            break;
        case QMessageBox::Yes:
            saveProjectAsDialog();
            return 1;
            break;
        default://including Cancel and Escape
            return 2;
        }
    }
    else {
        return -1;//No unsaved project
    }
}

void MainWindow::setCurrentDirbyFile(QString &f)
{
    int trimIndex = f.lastIndexOf('/');
    if (trimIndex != -1) {
        QDir::setCurrent(f.left(trimIndex));
    }
}
