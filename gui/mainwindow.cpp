#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cameradialog.h"
#include "optionsdialog.h"
#include "qimageinteractivedrawer.h"
#include <QDebug>
#include <QScreen>
#include <QInputDialog>

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

    //ParametersDialog settings
    boxFilterDlg.setMode(0);
    adaptiveBilateralDlg.setMode(-1);
    gaussianBinaryDlg.setMode(1);
    medianBinaryDlg.setMode(1);
    cannyDlg.setMode(2);
    floodFillDlg.setMode(3);
    floodFillDlg.setModal(false);

    ui->rawViewer->setNoScale();
    //preProcess tab button menu
    mouseBehaviourMenu = new QMenu();
    mouseBehaviourMenu->addAction("Normal Arrow", this, SLOT(onMouseNormalArrow()));
    mouseBehaviourMenu->addAction("White Pencil", this, SLOT(onMouseWhitePencil()));
    mouseBehaviourMenu->addAction("Black Pencil", this, SLOT(onMouseBlackPencil()));
    mouseBehaviourMenu->addAction("White Eraser", this, SLOT(onMouseWhiteEraser()));
    mouseBehaviourMenu->addAction("Black Eraser", this, SLOT(onMouseBlackEraser()));
    ui->mouseBehaviourButton->setMenu(mouseBehaviourMenu);

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
    connect(ui->ccDrawer, &QImageDrawer::gaugeLineResult, this, &MainWindow::onGaugeLineFinished);

    //preProcessTab

    connect(ui->histEqualiseButton, &QPushButton::clicked, this, &MainWindow::onHistEqualiseButtonClicked);
    connect(ui->boxFilterButton, &QPushButton::clicked, this, &MainWindow::onBoxFilterButtonClicked);
    connect(&boxFilterDlg, &ParametersDialog::parametersChanged, this, &MainWindow::onBoxFilterParametersChanged);
    connect(&boxFilterDlg, &ParametersDialog::accepted, this, &MainWindow::onPreParametersAccepted);
    connect(&boxFilterDlg, &ParametersDialog::rejected, this, &MainWindow::onPreParametersRejected);
    connect(&previewSpace, &CAIGA::WorkSpace::workStarted, &boxFilterDlg, &ParametersDialog::handleWorkStarted);
    connect(&previewSpace, &CAIGA::WorkSpace::workFinished, &boxFilterDlg, &ParametersDialog::handleWorkFinished);
    connect(ui->adaptiveBilateralFilter, &QPushButton::clicked, this, &MainWindow::onAdaptiveBilateralFilterButtonClicked);
    connect(&adaptiveBilateralDlg, &ParametersDialog::parametersChanged, this, &MainWindow::onAdaptiveBilateralFilterParametersChanged);
    connect(&adaptiveBilateralDlg, &ParametersDialog::accepted, this, &MainWindow::onPreParametersAccepted);
    connect(&adaptiveBilateralDlg, &ParametersDialog::rejected, this, &MainWindow::onPreParametersRejected);
    connect(&previewSpace, &CAIGA::WorkSpace::workStarted, &adaptiveBilateralDlg, &ParametersDialog::handleWorkStarted);
    connect(&previewSpace, &CAIGA::WorkSpace::workFinished, &adaptiveBilateralDlg, &ParametersDialog::handleWorkFinished);
    connect(ui->gaussianBinaryzationButton, &QPushButton::clicked, this, &MainWindow::onGaussianBinaryzationButtonClicked);
    connect(&gaussianBinaryDlg, &ParametersDialog::parametersChanged, this, &MainWindow::onGaussianBinaryzationParametersChanged);
    connect(&gaussianBinaryDlg, &ParametersDialog::accepted, this, &MainWindow::onPreParametersAccepted);
    connect(&gaussianBinaryDlg, &ParametersDialog::rejected, this, &MainWindow::onPreParametersRejected);
    connect(&previewSpace, &CAIGA::WorkSpace::workStarted, &gaussianBinaryDlg, &ParametersDialog::handleWorkStarted);
    connect(&previewSpace, &CAIGA::WorkSpace::workFinished, &gaussianBinaryDlg, &ParametersDialog::handleWorkFinished);
    connect(ui->medianBinaryzationButton, &QPushButton::clicked, this, &MainWindow::onMedianBinaryzationButtonClicked);
    connect(&medianBinaryDlg, &ParametersDialog::parametersChanged, this, &MainWindow::onMedianBinaryzationParametersChanged);
    connect(&medianBinaryDlg, &ParametersDialog::accepted, this, &MainWindow::onPreParametersAccepted);
    connect(&medianBinaryDlg, &ParametersDialog::rejected, this, &MainWindow::onPreParametersRejected);
    connect(&previewSpace, &CAIGA::WorkSpace::workStarted, &medianBinaryDlg, &ParametersDialog::handleWorkStarted);
    connect(&previewSpace, &CAIGA::WorkSpace::workFinished, &medianBinaryDlg, &ParametersDialog::handleWorkFinished);
    connect(ui->floodFillButton, &QPushButton::clicked, this, &MainWindow::onFloodFillButtonClicked);
    connect(&floodFillDlg, &ParametersDialog::parametersChanged, this, &MainWindow::onFloodFillParametersChanged);
    connect(&floodFillDlg, &ParametersDialog::undoButtonClicked, &previewSpace, &CAIGA::WorkSpace::undo);
    connect(&floodFillDlg, &ParametersDialog::redoButtonClicked, &previewSpace, &CAIGA::WorkSpace::redo);
    connect(&floodFillDlg, &ParametersDialog::accepted, this, &MainWindow::onFloodFillAccepted);
    connect(&floodFillDlg, &ParametersDialog::rejected, this, &MainWindow::onFloodFillRejected);
    connect(ui->cannyButton, &QPushButton::clicked, this, &MainWindow::onCannyButtonClicked);
    connect(&cannyDlg, &ParametersDialog::parametersChanged, this, &MainWindow::onCannyParametersChanged);
    connect(&cannyDlg, &ParametersDialog::accepted, this, &MainWindow::onPreParametersAccepted);
    connect(&cannyDlg, &ParametersDialog::rejected, this, &MainWindow::onPreParametersRejected);
    connect(&previewSpace, &CAIGA::WorkSpace::workStarted, &cannyDlg, &ParametersDialog::handleWorkStarted);
    connect(&previewSpace, &CAIGA::WorkSpace::workFinished, &cannyDlg, &ParametersDialog::handleWorkFinished);
    connect(ui->contoursButton, &QPushButton::clicked, this, &MainWindow::onContoursButtonClicked);
    connect(ui->preProcessButtonBox, &QDialogButtonBox::clicked, this, &MainWindow::onPreProcessButtonBoxClicked);
    connect(ui->actionUndo, &QAction::triggered, &preWorkSpace, &CAIGA::WorkSpace::undo);
    connect(ui->actionRedo, &QAction::triggered, &preWorkSpace, &CAIGA::WorkSpace::redo);

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
    connect(ui->imageTabs, &QTabWidget::currentChanged, this, &MainWindow::onCurrentTabChanged);
    connect(this, &MainWindow::messageArrived, this, &MainWindow::onMessagesArrived);

    connect(&previewSpace, &CAIGA::WorkSpace::workFinished, this, &MainWindow::onPreviewWorkFinished);
    connect(&preWorkSpace, &CAIGA::WorkSpace::workFinished, this, &MainWindow::onPreProcessWorkFinished);
    connect(&preWorkSpace, &CAIGA::WorkSpace::workStatusStringUpdated, this, &MainWindow::onMessagesArrived);

    connect(this, &MainWindow::configReadFinished, this, &MainWindow::updateOptions);

    //underlying work
    imgNameModel = new QStringListModel();
    ui->imageList->setModel(imgNameModel);

    readConfig();
    projectUnsaved = false;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mouseBehaviourMenu;
    delete imgNameModel;
}

const QString MainWindow::aboutText = QString() + "<h3>Computer-Aid Interactive Grain Analyser</h3><p>Version Pre Alpha on "

#if defined(_WIN32)//_WIN32 is defined for both 32-bit and 64-bit environment
        + "Windows"
#elif defined(__linux__)
        + "Linux"
#elif defined(__APPLE__) && defined(__MACH__)
        + "Mac OS X"
#elif defined(BSD)
        + "BSD"
#elif defined(__ANDROID__)
        + "Android"
#elif defined(__unix__)
        + "UNIX"
#else
        + "Unknown platform"
#endif
        + "</p><p>Built on " + __DATE__ + " at " + __TIME__ + " using "
#if defined(__GNUC__)
        + "GNU Compiler Collection " + QString::number(__GNUC__) + "." + QString::number(__GNUC_MINOR__) + + "." + QString::number(__GNUC_PATCHLEVEL__)
#elif defined(_MSC_VER)
        + "Microsoft Visual C++ Compiler"
#elif defined(__ICC) || defined(__INTEL_COMPILER)
        + "Intel C++ Compiler " + QString::number(__INTEL_COMPILER)
#elif defined(__clang__)
        + "Clang " + __clang_version__
#else
        + "Unkown compiler"
#endif

        + "</p><p>Copyright &copy; 2014 William Wong and other contributors.<br />School of Materials Science and Engineering, Southeast University.</p><p>Licensed under <a href='http://en.wikipedia.org/wiki/MIT_License'>The MIT License</a>.<br /></p><p><strong>The software contains third-party libraries and artworks below.</strong><br /><a href='http://opencv.org'>OpenCV</a> licensed under <a href='https://github.com/Itseez/opencv/blob/master/LICENSE'>3-clause BSD License</a><br /><a href='http://www.oxygen-icons.org'>Oxygen Icons</a> licensed under <a href='http://www.gnu.org/licenses/lgpl-3.0.txt'>LGPLv3 License</a><br /></p><div>THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.</div>";

void MainWindow::ccModeChanged(int i)
{
    ui->ccDrawer->setDrawMode(i);
}

void MainWindow::onGaugeLineFinished(qreal r)
{
    ui->gaugeResultLabel->setText(QString("%1 μm").arg(r));
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
        preWorkSpace.reset(cgimg);
        this->onPreProcessWorkFinished();
    }
}

void MainWindow::onMouseNormalArrow()
{
    ui->mouseBehaviourButton->setText("Normal Arrow");
    ui->preProcessDrawer->setCursor(QCursor(Qt::ArrowCursor));
    ui->preProcessDrawer->setDrawMode(QImageInteractiveDrawer::NONE);
    disconnect(ui->preProcessDrawer, &QImageInteractiveDrawer::mouseReleased, 0, 0);
}

void MainWindow::onMouseWhitePencil()
{
    disconnect(ui->preProcessDrawer, &QImageInteractiveDrawer::mouseReleased, 0, 0);
    ui->mouseBehaviourButton->setText("White Pencil");
    ui->preProcessDrawer->setCursor(QCursor(Qt::CrossCursor));
    ui->preProcessDrawer->setWhite();
    ui->preProcessDrawer->setDrawMode(QImageInteractiveDrawer::POLY_LINE);
    connect(ui->preProcessDrawer, &QImageInteractiveDrawer::mouseReleased, this, &MainWindow::onPencilDrawFinished);
}

void MainWindow::onMouseBlackPencil()
{
    disconnect(ui->preProcessDrawer, &QImageInteractiveDrawer::mouseReleased, 0, 0);
    ui->mouseBehaviourButton->setText("Black Pencil");
    ui->preProcessDrawer->setCursor(QCursor(Qt::CrossCursor));
    ui->preProcessDrawer->setWhite(false);
    ui->preProcessDrawer->setDrawMode(QImageInteractiveDrawer::POLY_LINE);
    connect(ui->preProcessDrawer, &QImageInteractiveDrawer::mouseReleased, this, &MainWindow::onPencilDrawFinished);
}

void MainWindow::onMouseWhiteEraser()
{
    disconnect(ui->preProcessDrawer, &QImageInteractiveDrawer::mouseReleased, 0, 0);
    ui->mouseBehaviourButton->setText("White Eraser");
    ui->preProcessDrawer->setCursor(QCursor(Qt::CrossCursor));
    ui->preProcessDrawer->setWhite();
    ui->preProcessDrawer->setDrawMode(QImageInteractiveDrawer::CIRCLE_LINE);
    connect(ui->preProcessDrawer, &QImageInteractiveDrawer::mouseReleased, this, &MainWindow::onEraserDrawFinished);
}

void MainWindow::onMouseBlackEraser()
{
    disconnect(ui->preProcessDrawer, &QImageInteractiveDrawer::mouseReleased, 0, 0);
    ui->mouseBehaviourButton->setText("Black Eraser");
    ui->preProcessDrawer->setCursor(QCursor(Qt::CrossCursor));
    ui->preProcessDrawer->setWhite(false);
    ui->preProcessDrawer->setDrawMode(QImageInteractiveDrawer::CIRCLE_LINE);
    connect(ui->preProcessDrawer, &QImageInteractiveDrawer::mouseReleased, this, &MainWindow::onEraserDrawFinished);
}

void MainWindow::onPreviewWorkFinished()
{
    ui->preProcessDrawer->setImage(previewSpace.getLatestQImg());
}

void MainWindow::onHistEqualiseButtonClicked()
{
    if (cgimg.validateHistogramEqualise()) {
        preWorkSpace.newHistogramEqualiseWork();
    }
    else {
        emit messageArrived("Abort. This operation is illegal!");
    }
}

void MainWindow::onBoxFilterButtonClicked()
{
    previewSpace.clear();
    boxFilterDlg.show();
    boxFilterDlg.exec();
}

void MainWindow::onBoxFilterParametersChanged(int kSize, double, double, bool)
{
    previewSpace.appendAndClone(preWorkSpace.last());
    previewSpace.newBoxFilterWork(kSize);
}

void MainWindow::onAdaptiveBilateralFilterButtonClicked()
{
    if (cgimg.validateAdaptiveBilateralFilter()) {
        previewSpace.clear();
        adaptiveBilateralDlg.show();
        adaptiveBilateralDlg.exec();
    }
    else {
        emit messageArrived("Abort. This operation is illegal!");
    }
}

void MainWindow::onAdaptiveBilateralFilterParametersChanged(int k, double s, double c, bool)
{
    previewSpace.appendAndClone(preWorkSpace.last());
    previewSpace.newAdaptiveBilateralFilterWork(k, s, c);
}

void MainWindow::onGaussianBinaryzationButtonClicked()
{
    previewSpace.clear();
    gaussianBinaryDlg.show();
    gaussianBinaryDlg.exec();
}

void MainWindow::onGaussianBinaryzationParametersChanged(int s, double c, double, bool inv)
{
    previewSpace.appendAndClone(preWorkSpace.last());
    previewSpace.newBinaryzationWork(CV_ADAPTIVE_THRESH_GAUSSIAN_C, inv ? CV_THRESH_BINARY_INV : CV_THRESH_BINARY, s, c);
}

void MainWindow::onMedianBinaryzationButtonClicked()
{
    previewSpace.clear();
    medianBinaryDlg.show();
    medianBinaryDlg.exec();
}

void MainWindow::onMedianBinaryzationParametersChanged(int s, double c, double, bool inv)
{
    previewSpace.appendAndClone(preWorkSpace.last());
    previewSpace.newBinaryzationWork(CV_ADAPTIVE_THRESH_MEAN_C, inv ? CV_THRESH_BINARY_INV : CV_THRESH_BINARY, s, c);
}

void MainWindow::onPencilDrawFinished(QVector<QPoint> pts)
{
    preWorkSpace.newPencilWork(pts, ui->preProcessDrawer->isWhite());
}

void MainWindow::onEraserDrawFinished(QVector<QPoint> pts)
{
    preWorkSpace.newEraserWork(pts, ui->preProcessDrawer->isWhite());
}

void MainWindow::onFloodFillButtonClicked()
{
    connect(ui->preProcessDrawer, &QImageInteractiveDrawer::mousePressed, this, &MainWindow::onFloodFillMouseClicked);
    previewSpace.clear();
    floodFillDlg.show();
    floodFillDlg.exec();
}

void MainWindow::onFloodFillParametersChanged(int, double h, double l, bool c8)
{
    previewSpace.appendAndClone(preWorkSpace.last());
    previewSpace.setFloodFillWorkParameters(h, l, c8);
}

void MainWindow::onFloodFillMouseClicked(QPoint p)
{
    previewSpace.newFloodFillWork(p.x(), p.y());
}

void MainWindow::onFloodFillAccepted()
{
    this->onPreParametersAccepted();
    disconnect(ui->preProcessDrawer, &QImageInteractiveDrawer::mousePressed, this, &MainWindow::onFloodFillMouseClicked);
}

void MainWindow::onFloodFillRejected()
{
    this->onPreParametersRejected();
    disconnect(ui->preProcessDrawer, &QImageInteractiveDrawer::mousePressed, this, &MainWindow::onFloodFillMouseClicked);
}

void MainWindow::onCannyButtonClicked()
{
    previewSpace.clear();
    cannyDlg.show();
    cannyDlg.exec();
}

void MainWindow::onCannyParametersChanged(int aSize, double high, double low, bool l2)
{
    previewSpace.appendAndClone(preWorkSpace.last());
    previewSpace.newCannyWork(aSize, high, low, l2);
}

void MainWindow::onPreParametersAccepted()
{
    preWorkSpace.append(previewSpace.takeLast());
}

void MainWindow::onPreParametersRejected()
{
    this->onPreProcessWorkFinished();
}

void MainWindow::onContoursButtonClicked()
{
    preWorkSpace.newContoursWork();
}

void MainWindow::onPreProcessWorkFinished()
{
    ui->preProcessDrawer->setImage(preWorkSpace.getLatestQImg());
}

void MainWindow::onPreProcessButtonBoxClicked(QAbstractButton *b)
{
    if (ui->preProcessButtonBox->standardButton(b) == QDialogButtonBox::Reset) {//reset
        //TODO
        preWorkSpace.reset(cgimg);
        this->onPreProcessWorkFinished();
    }
    else {//save
        //TODO
        preWorkSpace.simplified();
    }
}

void MainWindow::onCurrentTabChanged(int i)
{
    switch (i) {
    case 2://crop and calibre
        ui->ccPixelViewer->setLivePreviewEnabled(true);
        ui->actionRedo->setEnabled(false);
        ui->actionUndo->setEnabled(false);
        break;
    case 3://preprocess
        ui->ccPixelViewer->setLivePreviewEnabled(false);
        ui->actionRedo->setEnabled(true);
        ui->actionUndo->setEnabled(true);
        break;
    default:
        ui->ccPixelViewer->setLivePreviewEnabled(false);
        ui->actionRedo->setEnabled(false);
        ui->actionUndo->setEnabled(false);
    }
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
    ui->ccDrawer->setImage(cgimg.getRawImage());
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
