#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cameradialog.h"
#include "optionsdialog.h"
#include "qimageinteractivedrawer.h"
#include <QDebug>

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

    //Question: is it necessary?
    ui->rawViewer->setNotLarger(true);
    ui->preProcessViewer->setNotLarger(true);
    ui->edgesViewer->setNotLarger(true);

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
    //crop and calibre tab
    connect(ui->buttonGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &MainWindow::ccModeChanged);//Operation Mode
    connect(ui->ccButtonBox, &QDialogButtonBox::clicked, this, &MainWindow::onCCButtonBoxClicked);
    connect(ui->ccDrawer, &QImageDrawer::calibreFinished, ui->calibreDoubleSpinBox, &QDoubleSpinBox::setValue);

    //preProcessTab
    connect(ui->histogramCheckBox, &QCheckBox::toggled, this, &MainWindow::histogramEqualiseChecked);
    connect(ui->blurCheckBox, &QCheckBox::toggled, this, &MainWindow::onBlurCheckBoxChecked);
    connect(ui->blurMethodComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::blurMethodChanged);
    connect(ui->blurKSizeSlider, &QSlider::valueChanged, this, &MainWindow::onBlurParameterChanged);
    connect(ui->blurKSizeSlider, &QSlider::valueChanged, this, &MainWindow::onBlurKSizeSliderChanged);
    connect(ui->blurSigma1SpinBox, static_cast<void (QDoubleSpinBox::*) (double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::onBlurParameterChanged);
    connect(ui->blurSigma2SpinBox, static_cast<void (QDoubleSpinBox::*) (double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::onBlurParameterChanged);
    connect(ui->binaryzationCheckBox, &QCheckBox::toggled, this, &MainWindow::binaryzationCheckBoxStateChanged);
    connect(ui->binaryzationCDoubleSpinBox, static_cast<void (QDoubleSpinBox::*) (double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::onBinaryzationParameterChanged);
    connect(ui->binaryzationMethodComboBox, static_cast<void (QComboBox::*) (int)>(&QComboBox::currentIndexChanged), this, &MainWindow::onBinaryzationParameterChanged);
    connect(ui->binaryzationTypeComboBox, static_cast<void (QComboBox::*) (int)>(&QComboBox::currentIndexChanged), this, &MainWindow::onBinaryzationParameterChanged);
    connect(ui->binaryzationSizeSlider, &QSlider::valueChanged, this, &MainWindow::onBinaryzationParameterChanged);
    connect(ui->binaryzationSizeSlider, &QSlider::valueChanged, this, &MainWindow::onBinaryzationSizeSliderValueChanged);
    connect(ui->preProcessButtonBox, &QDialogButtonBox::clicked, this, &MainWindow::onPPButtonBoxClicked);

    //edgesTab
    connect(this, &MainWindow::edgesParametersChanged, this, &MainWindow::onEdgesParametersChanged);
    connect(ui->apertureSizeSlider, &QSlider::valueChanged, this, &MainWindow::onEdgesParametersChanged);
    connect(ui->highThresholdDoubleSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::highThresholdChanged);
    connect(ui->lowThresholdDoubleSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::onEdgesParametersChanged);
    connect(ui->l2GradientCheckBox, &QCheckBox::toggled, this, &MainWindow::onEdgesParametersChanged);
    //connect(ui->edgesButtonBox, &QDialogButtonBox::accepted, this, &MainWindow::saveEdges);
    connect(ui->edgesButtonBox, &QDialogButtonBox::rejected, this, &MainWindow::discardEdges);

    //SegmentationTab
    connect(ui->higherDiffSlider, &QSlider::valueChanged, this, &MainWindow::onSegmentHighDiffValueChanged);
    connect(ui->lowerDiffSlider, &QSlider::valueChanged, this, &MainWindow::onSegmentLowDiffValueChanged);
    connect(ui->higherDiffSlider, &QSlider::valueChanged, this, &MainWindow::onSegmentParametersChanged);
    connect(ui->lowerDiffSlider, &QSlider::valueChanged, this, &MainWindow::onSegmentParametersChanged);
    connect(ui->seg4Con, &QRadioButton::toggled, this, &MainWindow::onSegmentParametersChanged);//seg8Con's toggled signal is linked with seg4Con
    connect(ui->refreshSegButton, &QPushButton::clicked, this, &MainWindow::onSegmentationRefreshButtonPressed);
    connect(ui->segmentationViewer, &QImageInteractiveDrawer::mouseClickedFinished, this, &MainWindow::onSegmentViewerClicked);

    //MainWindow
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
    connect(this, &MainWindow::messageArrived, this, &MainWindow::onMessagesArrived);
    connect(&worker, &CAIGA::WorkerThread::workStatusUpdated, this, &MainWindow::onMessagesArrived);

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
    ui->ccDrawer->setDrawMode(i);
}

void MainWindow::onCCButtonBoxClicked(QAbstractButton *b)
{
    if (ui->ccButtonBox->standardButton(b) == QDialogButtonBox::Reset) {
        ui->ccDrawer->reset();
        ui->calibreDoubleSpinBox->setValue(0.0);
        ui->cropCircleRadio->setChecked(true);
    }
    else {//save
        cgimg.setCropCalibreStruct(ui->ccDrawer->getCCStruct());
        ui->preProcessViewer->setPixmap(cgimg.getCroppedPixmap());
    }
}

void MainWindow::histogramEqualiseChecked(bool toggle)
{
    if (toggle) {
        if (cgimg.validateHistogramEqualise()) {
            disconnect(&worker, &WorkerThread::workFinished, 0, 0);
            connect(&worker, &WorkerThread::workFinished, this, &MainWindow::onPreProcessWorkFinished);
            worker.histogramEqualiseWork(cgimg);
        }
    }
    else {
        //TODO kind of undo
        ui->preProcessViewer->setPixmap(cgimg.getCroppedPixmap());
    }
}

void MainWindow::onBlurCheckBoxChecked(bool t)
{
    if (t) {
        emit onBlurParameterChanged();
    }
    else {
        //TODO
        ui->preProcessViewer->setPixmap(cgimg.getCroppedPixmap());
    }
}

void MainWindow::onBlurParameterChanged()
{
    int ksize = ui->blurKSizeSlider->value() * 2 - 1;
    double sx = ui->blurSigma1SpinBox->value();
    double sy = ui->blurSigma2SpinBox->value();

    disconnect(&worker, &WorkerThread::workFinished, 0, 0);
    connect(&worker, &WorkerThread::workFinished, this, &MainWindow::onPreProcessWorkFinished);

    switch (ui->blurMethodComboBox->currentIndex()) {
    case 0://AdaptiveBilateralFilter
        if (cgimg.validateAdaptiveBilateralFilter()) {
            worker.adaptiveBilateralFilterWork(cgimg, ksize, sx, sy);
        }
        break;
    case 1://Gaussian
        if (cgimg.validateGaussianMedianBlur()) {
            worker.gaussianBlurWork(cgimg, ksize, sx, sy);
        }
        break;
    case 2://Median
        if (cgimg.validateGaussianMedianBlur()) {
            worker.medianBlurWork(cgimg, ksize);
        }
        break;
    }
}

void MainWindow::onBlurKSizeSliderChanged(int s)
{
    ui->blurKSizeSlider->setToolTip(QString::number(s * 2 - 1));
}

void MainWindow::blurMethodChanged(int mID)
{
    switch(mID) {
    case 0://AdaptiveBilateralFilter
        ui->blurSigma1SpinBox->setEnabled(true);
        ui->blurSigma2SpinBox->setEnabled(true);
        ui->blurSigma1SpinBox->setMinimum(1.0);
        ui->blurSigma2SpinBox->setMinimum(1.0);
        ui->blurSigma1SpinBox->setValue(1.0);
        ui->blurSigma2SpinBox->setValue(20.0);//set 20 to default
        ui->blurSigma1Label->setText("Sigma Space");
        ui->blurSigma2Label->setText("Max Sigma Colour");
        break;
    case 1://Gaussian
        ui->blurSigma1SpinBox->setEnabled(true);
        ui->blurSigma2SpinBox->setEnabled(true);
        ui->blurSigma1SpinBox->setMinimum(0.0);
        ui->blurSigma2SpinBox->setMinimum(0.0);
        ui->blurSigma1SpinBox->setValue(0.0);//when use zeros, sigma will be calculated from kernel size.
        ui->blurSigma2SpinBox->setValue(0.0);
        ui->blurSigma1Label->setText("Sigma X");
        ui->blurSigma2Label->setText("Sigma Y");
        break;
    case 2://Median
        ui->blurSigma1SpinBox->setEnabled(false);
        ui->blurSigma2SpinBox->setEnabled(false);
    }
}

void MainWindow::binaryzationCheckBoxStateChanged(bool s)
{
    if (s) {
        onBinaryzationParameterChanged();
    }
    else {
        //TODO kind of redo
        ui->preProcessViewer->setPixmap(cgimg.getCroppedPixmap());
    }
}

void MainWindow::onBinaryzationParameterChanged()
{
    int blockSize = ui->binaryzationSizeSlider->value() * 2 - 1;
    int method = cv::ADAPTIVE_THRESH_MEAN_C;
    int type = cv::THRESH_BINARY;
    if (ui->binaryzationMethodComboBox->currentIndex() == 1) {
        method = cv::ADAPTIVE_THRESH_GAUSSIAN_C;
    }
    if (ui->binaryzationTypeComboBox->currentIndex() == 1) {
        method = cv::THRESH_BINARY_INV;
    }
    disconnect(&worker, &WorkerThread::workFinished, 0, 0);
    connect(&worker, &WorkerThread::workFinished, this, &MainWindow::onPreProcessWorkFinished);
    worker.binaryzationWork(cgimg, method, type, blockSize, ui->binaryzationCDoubleSpinBox->value());
}

void MainWindow::onBinaryzationSizeSliderValueChanged(int s)
{
    ui->binaryzationSizeSlider->setToolTip(QString::number(s * 2 - 1));
}

void MainWindow::onPreProcessWorkFinished()
{
    ui->preProcessViewer->setPixmap(cgimg.getPreProcessedPixmap());
}

void MainWindow::onPPButtonBoxClicked(QAbstractButton *b)
{
    if (ui->preProcessButtonBox->standardButton(b) == QDialogButtonBox::Reset) {//reset
        //TODO
        ui->preProcessViewer->setPixmap(cgimg.getCroppedPixmap());
    }
    else {//save
        //TODO
    }
}

void MainWindow::highThresholdChanged(double ht)
{
    ui->lowThresholdDoubleSpinBox->setMaximum(ht - 1);
    emit edgesParametersChanged();
}

void MainWindow::onEdgesParametersChanged()
{
    int aSize = (ui->apertureSizeSlider->value() * 2 - 1);

    bool l2 = false;
    if (ui->l2GradientCheckBox->checkState() == Qt::Checked) {
        l2 = true;
    }

    disconnect(&worker, &WorkerThread::workFinished, 0, 0);
    connect(&worker, &WorkerThread::workFinished, this, &MainWindow::onEdgesDetectionWorkFinished);
    worker.cannyEdgesWork(cgimg, ui->highThresholdDoubleSpinBox->value(), ui->lowThresholdDoubleSpinBox->value(), aSize, l2);
}

void MainWindow::onEdgesDetectionWorkFinished()
{
    ui->edgesViewer->setPixmap(cgimg.getEdgesPixmap());
}

void MainWindow::discardEdges()
{
    ui->edgesViewer->setPixmap(cgimg.getPreProcessedPixmap());
}

void MainWindow::onSegmentLowDiffValueChanged(int l)
{
    ui->lowerDiffSlider->setToolTip(QString::number(l));
}

void MainWindow::onSegmentHighDiffValueChanged(int h)
{
    ui->higherDiffSlider->setToolTip(QString::number(h));
}

void MainWindow::onSegmentParametersChanged()
{
    onSegmentationRefreshButtonPressed();
}

void MainWindow::onSegmentationRefreshButtonPressed()
{
    cgimg.prepareFloodFill();
    ui->segmentationViewer->setImage(cgimg.getProcessedImage());
}

void MainWindow::onSegmentViewerClicked(QPoint p)
{
    disconnect(&worker, &WorkerThread::workFinished, 0, 0);
    connect(&worker, &WorkerThread::workFinished, this, &MainWindow::onSegmentWorkFinished);
    worker.floodfillSegmentWork(cgimg, p, ui->higherDiffSlider->value(), ui->lowerDiffSlider->value(), ui->seg4Con->isChecked());
}

void MainWindow::onSegmentWorkFinished()
{
    ui->segmentationViewer->setImage(cgimg.getProcessedImage());
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
    ui->rawViewer->setPixmap(cgimg.getRawPixmap());

    if (cgimg.isProcessed()) {
        ui->analysisLabel->setPixmap(cgimg.getProcessedPixmap());
    }
    else {
        ui->analysisLabel->setPixmap(cgimg.getEdgesPixmap());
    }

    if (cgimg.hasEdges()) {
        ui->edgesViewer->setPixmap(cgimg.getEdgesPixmap());
    }
    else {
        emit edgesParametersChanged();//Initialise a premature edges image
    }

    if (cgimg.isPreProcessed()) {
        ui->preProcessViewer->setPixmap(cgimg.getPreProcessedPixmap());
    }
    else {
        ui->preProcessViewer->setPixmap(cgimg.getCroppedPixmap());
    }

    ui->ccDrawer->setImage(cgimg.getRawImage());
    if (cgimg.isCropped()) {
        ui->ccDrawer->restoreState(cgimg.getCropCalibreStruct());
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

    ui->ccDrawer->setPenColour(colour);
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

void MainWindow::onMessagesArrived(const QString &str)
{
    ui->statusBar->showMessage(str);
}
