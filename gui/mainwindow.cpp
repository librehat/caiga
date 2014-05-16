#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cameradialog.h"
#include "optionsdialog.h"
#include "qimageinteractivedrawer.h"
#include "macro.h"
#include <QDebug>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    restoreGeometry(settings.value("MainGeometry").toByteArray());
    restoreState(settings.value("MainState").toByteArray());

    QDir::setCurrent(settings.value("CurrentDir").toString());

    //ParametersDialog settings
    parametersDlg = NULL;
    watershedDlg = NULL;
    analyser = new Analyser(this);

    ui->ccDrawer->setSpace(&ccSpace);
    ui->rawViewer->setNoScale();
    //preProcess tab button menu
    mouseBehaviourMenu = new QMenu(this);
    mouseBehaviourMenu->addAction("Normal Arrow", this, SLOT(onMouseNormalArrow()));
    mouseBehaviourMenu->addAction("White Pencil", this, SLOT(onMouseWhitePencil()));
    mouseBehaviourMenu->addAction("Black Pencil", this, SLOT(onMouseBlackPencil()));
    mouseBehaviourMenu->addAction("White Eraser", this, SLOT(onMouseWhiteEraser()));
    mouseBehaviourMenu->addAction("Black Eraser", this, SLOT(onMouseBlackEraser()));
    ui->mouseBehaviourButton->setMenu(mouseBehaviourMenu);

    /*
     * Setup icons.
     */
    ui->actionReset->setIcon(QIcon::fromTheme("document-revert"));
    ui->actionQuit->setIcon(QIcon::fromTheme("application-exit"));
    ui->actionExportImg_As->setIcon(QIcon::fromTheme("document-export", QIcon::fromTheme("fileprint")));
    ui->actionQuick_Export->setIcon(QIcon::fromTheme("filequickprint", QIcon::fromTheme("document-export")));
    ui->actionRedo->setIcon(QIcon::fromTheme("edit-redo"));
    ui->actionUndo->setIcon(QIcon::fromTheme("edit-undo"));
    ui->actionOpenFile->setIcon(QIcon::fromTheme("document-open"));
    ui->actionOpenCamera->setIcon(QIcon::fromTheme("camera-photo"));
    ui->actionOptions->setIcon(QIcon::fromTheme("configure", QIcon::fromTheme("gconf-editor")));
    ui->actionHelp->setIcon(QIcon::fromTheme("help-contents"));
    ui->actionAbout_CAIGA->setIcon(QIcon::fromTheme("help-about"));

    /*
     * SIGNALs and SLOTs
     * Only those that cannot be connected in Deisgner should be defined below
     */
    //crop and calibre tab
    connect(ui->buttonGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &MainWindow::ccModeChanged);//Operation Mode
    connect(ui->ccDrawer, &QImageDrawer::calibreFinished, ui->scaleDoubleSpinBox, &QDoubleSpinBox::setValue);
    connect(ui->scaleDoubleSpinBox, static_cast<void (QDoubleSpinBox::*) (double)>(&QDoubleSpinBox::valueChanged), &ccSpace, &CAIGA::CCSpace::setScaleValue);
    connect(ui->ccDrawer, &QImageDrawer::gaugeLineResult, this, &MainWindow::onGaugeLineFinished);
    connect(ui->ccLoadMacroButton, &QPushButton::clicked, this, &MainWindow::onCCLoadMacroButtonClicked);
    connect(ui->ccButtonBox, &QDialogButtonBox::clicked, this, &MainWindow::onCCButtonBoxClicked);

    //preProcessTab
    connect(ui->invertGrayscaleButton, &QPushButton::clicked, this, &MainWindow::onInvertGrayscaleButtonClicked);
    connect(ui->histEqualiseButton, &QPushButton::clicked, this, &MainWindow::onHistEqualiseButtonClicked);
    connect(ui->gradientButton, &QPushButton::clicked, this, &MainWindow::onGradientButtonClicked);
    connect(ui->boxFilterButton, &QPushButton::clicked, this, &MainWindow::onBoxFilterButtonClicked);
    connect(ui->adaptiveBilateralFilter, &QPushButton::clicked, this, &MainWindow::onAdaptiveBilateralFilterButtonClicked);
    connect(ui->medianBlurButton, &QPushButton::clicked, this, &MainWindow::onMedianBlurButtonClicked);
    connect(ui->gaussianBinaryzationButton, &QPushButton::clicked, this, &MainWindow::onGaussianBinaryzationButtonClicked);
    connect(ui->medianBinaryzationButton, &QPushButton::clicked, this, &MainWindow::onMedianBinaryzationButtonClicked);
    connect(ui->floodFillButton, &QPushButton::clicked, this, &MainWindow::onFloodFillButtonClicked);
    connect(ui->scharrButton, &QPushButton::clicked, this, &MainWindow::onScharrButtonClicked);
    connect(ui->cannyButton, &QPushButton::clicked, this, &MainWindow::onCannyButtonClicked);
    connect(ui->watershedButton, &QPushButton::clicked, this, &MainWindow::onWatershedButtonClicked);
    connect(ui->contoursButton, &QPushButton::clicked, this, &MainWindow::onContoursButtonClicked);
    connect(ui->preProcessButtonBox, &QDialogButtonBox::clicked, this, &MainWindow::onPreProcessButtonBoxClicked);
    connect(ui->actionUndo, &QAction::triggered, &preWorkSpace, &CAIGA::WorkSpace::undo);
    connect(ui->actionRedo, &QAction::triggered, &preWorkSpace, &CAIGA::WorkSpace::redo);

    //Analysis
    connect(ui->classAddToolButton, &QToolButton::clicked, this, &MainWindow::onClassAddButtonClicked);
    connect(ui->classDelToolButton, &QToolButton::clicked, this, &MainWindow::onClassDelButtonClicked);

    //MainWindow
    connect(ui->actionOpenFile, &QAction::triggered, this, &MainWindow::addDiskFileDialog);
    connect(ui->actionOpenCamera, &QAction::triggered, this, &MainWindow::addCameraImageDialog);
    connect(ui->actionReset, &QAction::triggered, this, &MainWindow::onResetActionTriggered);
    connect(ui->actionOptions, &QAction::triggered, this, &MainWindow::onOptionsActionTriggered);
    connect(ui->actionExportImg_As, &QAction::triggered, this, &MainWindow::exportImgDialog);
    connect(ui->actionAbout_Qt, &QAction::triggered, this, &MainWindow::aboutQtDialog);
    connect(ui->actionAbout_CAIGA, &QAction::triggered, this, &MainWindow::aboutCAIGADialog);
    connect(ui->imageTabs, &QTabWidget::currentChanged, this, &MainWindow::onCurrentTabChanged);
    connect(this, &MainWindow::messageArrived, this, &MainWindow::onMessagesArrived);

    connect(&preWorkSpace, &CAIGA::WorkSpace::workFinished, this, &MainWindow::onPreProcessWorkFinished);
    connect(&preWorkSpace, &CAIGA::WorkSpace::workStatusStringUpdated, this, &MainWindow::onMessagesArrived);
    connect(&previewSpace, &CAIGA::WorkSpace::workStatusStringUpdated, this, &MainWindow::onMessagesArrived);

    connect(this, &MainWindow::configReadFinished, this, &MainWindow::updateOptions);

    readConfig();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mouseBehaviourMenu;
    if (parametersDlg != NULL) {
        delete parametersDlg;
    }
    if (watershedDlg != NULL) {
        delete watershedDlg;
    }
    delete analyser;
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

        + "</p><p>Copyright &copy; 2014 <a href='http://www.librehat.com'>William Wong</a> (<a href='mailto:hzwhuang@gmail.com'>hzwhuang@gmail.com</a>) and other contributors.<br /><a href= 'http://smse.seu.edu.cn'>School of Materials Science and Engineering</a>, <a href='http://www.seu.edu.cn'>Southeast University</a>.</p><p>Licensed under <a href='http://en.wikipedia.org/wiki/MIT_License'>The MIT License</a>.<br /></p><p><strong>The software contains third-party libraries and artworks below.</strong><br /><a href='http://opencv.org'>OpenCV</a> licensed under <a href='https://github.com/Itseez/opencv/blob/master/LICENSE'>3-clause BSD License</a><br /><a href='http://www.oxygen-icons.org'>Oxygen Icons</a> licensed under <a href='http://www.gnu.org/licenses/lgpl-3.0.txt'>LGPLv3 License</a><br /></p><div>THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.</div>";

void MainWindow::ccModeChanged(int i)
{
    ui->ccDrawer->setDrawMode(i);
}

void MainWindow::onGaugeLineFinished(qreal r)
{
    ui->gaugeResultLabel->setText(QString("%1 μm").arg(r));
}

void MainWindow::onCCLoadMacroButtonClicked()
{
    QString macroFile = QFileDialog::getOpenFileName(this, "Load Macro for Crop and Calibre", QDir::currentPath(),
                       "Macro Text File (*.txt)");
    if (macroFile.isNull()) {
        return;
    }
    setCurrentDirbyFile(macroFile);
    CAIGA::Macro macro(this);
    macro.setImageObject(&cgimg);
    macro.doCropAndCalibreMacroFromFile(macroFile);
    macro.deleteLater();
}

void MainWindow::onCCButtonBoxClicked(QAbstractButton *b)
{
    if (ui->ccButtonBox->standardButton(b) == QDialogButtonBox::Reset) {
        ui->ccDrawer->reset();
        ui->scaleDoubleSpinBox->setValue(0);
        ui->cropCircleRadio->setChecked(true);
    }
    else {//save
        if (cgimg.getScaleValue() == 0) {
            onMessagesArrived("You must calibre image scale before move to next step.");
            return;
        }
        ccSpace.cropImage();
        preWorkSpace.reset(cgimg);
        ui->imageTabs->setCurrentIndex(2);
        onMessagesArrived("Pre-Process the image, then segment it.");
    }
}

void MainWindow::onMouseNormalArrow()
{
    ui->mouseBehaviourButton->setText("Normal Arrow");
    ui->preProcessDrawer->setDrawMode(QImageInteractiveDrawer::NONE);
    disconnect(ui->preProcessDrawer, &QImageInteractiveDrawer::mouseReleased, 0, 0);
}

void MainWindow::onMouseWhitePencil()
{
    disconnect(ui->preProcessDrawer, &QImageInteractiveDrawer::mouseReleased, 0, 0);
    ui->mouseBehaviourButton->setText("White Pencil");
    ui->preProcessDrawer->setWhite();
    ui->preProcessDrawer->setDrawMode(QImageInteractiveDrawer::PENCIL);
    connect(ui->preProcessDrawer, &QImageInteractiveDrawer::mouseReleased, this, &MainWindow::onPencilDrawFinished);
}

void MainWindow::onMouseBlackPencil()
{
    disconnect(ui->preProcessDrawer, &QImageInteractiveDrawer::mouseReleased, 0, 0);
    ui->mouseBehaviourButton->setText("Black Pencil");
    ui->preProcessDrawer->setWhite(false);
    ui->preProcessDrawer->setDrawMode(QImageInteractiveDrawer::PENCIL);
    connect(ui->preProcessDrawer, &QImageInteractiveDrawer::mouseReleased, this, &MainWindow::onPencilDrawFinished);
}

void MainWindow::onMouseWhiteEraser()
{
    disconnect(ui->preProcessDrawer, &QImageInteractiveDrawer::mouseReleased, 0, 0);
    ui->mouseBehaviourButton->setText("White Eraser");
    ui->preProcessDrawer->setWhite();
    ui->preProcessDrawer->setDrawMode(QImageInteractiveDrawer::ERASER);
    connect(ui->preProcessDrawer, &QImageInteractiveDrawer::mouseReleased, this, &MainWindow::onEraserDrawFinished);
}

void MainWindow::onMouseBlackEraser()
{
    disconnect(ui->preProcessDrawer, &QImageInteractiveDrawer::mouseReleased, 0, 0);
    ui->mouseBehaviourButton->setText("Black Eraser");
    ui->preProcessDrawer->setWhite(false);
    ui->preProcessDrawer->setDrawMode(QImageInteractiveDrawer::ERASER);
    connect(ui->preProcessDrawer, &QImageInteractiveDrawer::mouseReleased, this, &MainWindow::onEraserDrawFinished);
}

void MainWindow::onPreviewWorkFinished()
{
    ui->preProcessDrawer->setImage(previewSpace.getLastDisplayImage());
}

void MainWindow::onInvertGrayscaleButtonClicked()
{
    preWorkSpace.newInvertGrayscaleWork();
}

void MainWindow::onHistEqualiseButtonClicked()
{
    preWorkSpace.newHistogramEqualiseWork();
}

void MainWindow::onGradientButtonClicked()
{
    previewSpace.reset(preWorkSpace.getLastMatrix());
    if (parametersDlg != NULL) {
        delete parametersDlg;
    }
    parametersDlg = new ParametersDialog(CAIGA::WorkBase::Gradient, this);
    handleParametersDialogue(&MainWindow::onGradientParamatersChanged);
}

void MainWindow::onGradientParamatersChanged(int kSize, double, double, bool)
{
    previewSpace.newGradientWork(kSize);
}

void MainWindow::onBoxFilterButtonClicked()
{
    previewSpace.reset(preWorkSpace.getLastMatrix());
    if (parametersDlg != NULL) {
        delete parametersDlg;
    }
    parametersDlg = new ParametersDialog(CAIGA::WorkBase::BoxFilter, this);
    handleParametersDialogue(&MainWindow::onBoxFilterParametersChanged);
}

void MainWindow::onBoxFilterParametersChanged(int kSize, double, double, bool)
{
    previewSpace.newBoxFilterWork(kSize);
}

void MainWindow::onAdaptiveBilateralFilterButtonClicked()
{
    previewSpace.reset(preWorkSpace.getLastMatrix());
    if (parametersDlg != NULL) {
        delete parametersDlg;
    }
    parametersDlg = new ParametersDialog(CAIGA::WorkBase::AptBilateralFilter, this);
    handleParametersDialogue(&MainWindow::onAdaptiveBilateralFilterParametersChanged);
}

void MainWindow::onAdaptiveBilateralFilterParametersChanged(int k, double s, double c, bool)
{
    previewSpace.newAdaptiveBilateralFilterWork(k, s, c);
}

void MainWindow::onMedianBlurButtonClicked()
{
    previewSpace.reset(preWorkSpace.getLastMatrix());
    if (parametersDlg != NULL) {
        delete parametersDlg;
    }
    parametersDlg = new ParametersDialog(CAIGA::WorkBase::MedianBlur, this);
    handleParametersDialogue(&MainWindow::onMedianBlurParametersChanged);
}

void MainWindow::onMedianBlurParametersChanged(int k, double, double, bool)
{
    previewSpace.newMedianBlurWork(k);
}

void MainWindow::onGaussianBinaryzationButtonClicked()
{
    previewSpace.reset(preWorkSpace.getLastMatrix());
    if (parametersDlg != NULL) {
        delete parametersDlg;
    }
    parametersDlg = new ParametersDialog(CAIGA::WorkBase::Binaryzation, this);
    handleParametersDialogue(&MainWindow::onGaussianBinaryzationParametersChanged);
}

void MainWindow::onGaussianBinaryzationParametersChanged(int s, double c, double, bool inv)
{
    previewSpace.newBinaryzationWork(CV_ADAPTIVE_THRESH_GAUSSIAN_C, inv ? CV_THRESH_BINARY_INV : CV_THRESH_BINARY, s, c);
}

void MainWindow::onMedianBinaryzationButtonClicked()
{
    previewSpace.reset(preWorkSpace.getLastMatrix());
    if (parametersDlg != NULL) {
        delete parametersDlg;
    }
    parametersDlg = new ParametersDialog(CAIGA::WorkBase::Binaryzation, this);
    handleParametersDialogue(&MainWindow::onMedianBinaryzationParametersChanged);
}

void MainWindow::onMedianBinaryzationParametersChanged(int s, double c, double, bool inv)
{
    previewSpace.newBinaryzationWork(CV_ADAPTIVE_THRESH_MEAN_C, inv ? CV_THRESH_BINARY_INV : CV_THRESH_BINARY, s, c);
}

void MainWindow::onPencilDrawFinished(const QVector<QPoint> &pts)
{
    preWorkSpace.newPencilWork(pts, ui->preProcessDrawer->isWhite());
}

void MainWindow::onEraserDrawFinished(const QVector<QPoint> &pts)
{
    preWorkSpace.newEraserWork(pts, ui->preProcessDrawer->isWhite());
}

void MainWindow::onFloodFillButtonClicked()
{
    previewSpace.reset(preWorkSpace.getLastMatrix());
    if (parametersDlg != NULL) {
        delete parametersDlg;
    }
    parametersDlg = new ParametersDialog(CAIGA::WorkBase::FloodFill, this);

    connect(&previewSpace, &CAIGA::WorkSpace::workFinished, this, &MainWindow::onPreviewWorkFinished);
    connect(parametersDlg, &ParametersDialog::parametersChanged, this, &MainWindow::onFloodFillParametersChanged);
    connect(parametersDlg, &ParametersDialog::undoButtonClicked, &previewSpace, &CAIGA::WorkSpace::undo);
    connect(parametersDlg, &ParametersDialog::redoButtonClicked, &previewSpace, &CAIGA::WorkSpace::redo);
    connect(parametersDlg, &ParametersDialog::accepted, this, &MainWindow::onFloodFillAccepted);
    connect(parametersDlg, &ParametersDialog::rejected, this, &MainWindow::onFloodFillRejected);
    connect(parametersDlg, &ParametersDialog::resetButtonClicked, &previewSpace, static_cast<void (CAIGA::WorkSpace::*) (void)>(&CAIGA::WorkSpace::reset));
    connect(ui->preProcessDrawer, &QImageInteractiveDrawer::mousePressed, this, &MainWindow::onFloodFillMouseClicked);
    parametersDlg->setModal(false);
    parametersDlg->show();
    parametersDlg->exec();
}

void MainWindow::onFloodFillParametersChanged(int, double h, double l, bool c8)
{
    previewSpace.setFloodFillWorkParameters(h, l, c8);
}

void MainWindow::onFloodFillMouseClicked(QPoint p)
{
    previewSpace.newFloodFillWork(p.x(), p.y(), true);
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

void MainWindow::onScharrButtonClicked()
{
    preWorkSpace.newScharrWork();
}

void MainWindow::onCannyButtonClicked()
{
    previewSpace.reset(preWorkSpace.getLastMatrix());
    if (parametersDlg != NULL) {
        delete parametersDlg;
    }
    parametersDlg = new ParametersDialog(CAIGA::WorkBase::Canny, this);
    handleParametersDialogue(&MainWindow::onCannyParametersChanged);
}

void MainWindow::onCannyParametersChanged(int aSize, double high, double low, bool l2)
{
    previewSpace.newCannyWork(aSize, high, low, l2);
}

void MainWindow::onWatershedButtonClicked()
{
    previewSpace.reset(preWorkSpace.getLastMatrix());
    if (watershedDlg != NULL) {
        delete watershedDlg;
    }
    watershedDlg = new WatershedMarkerDialog(this);
    watershedDlg->setOrignialMat(preWorkSpace.getLastMatrix());
    watershedDlg->setPenColour(ui->ccDrawer->getPenColour());
    connect(watershedDlg, &WatershedMarkerDialog::reseted, this, &MainWindow::onPreProcessWorkFinished);
    connect(watershedDlg, &WatershedMarkerDialog::previewTriggled, this, &MainWindow::onWatershedPreviewed);
    connect(watershedDlg, &WatershedMarkerDialog::accepted, this, &MainWindow::onWatershedAccepted);
    connect(watershedDlg, &WatershedMarkerDialog::rejected, this, &MainWindow::onPreParametersRejected);
    connect(&previewSpace, &CAIGA::WorkSpace::workStarted, watershedDlg, &WatershedMarkerDialog::onPreviewStarted);
    connect(&previewSpace, &CAIGA::WorkSpace::workFinished, watershedDlg, &WatershedMarkerDialog::onPreviewFinished);
    connect(&previewSpace, &CAIGA::WorkSpace::workFinished, this, &MainWindow::onPreviewWorkFinished);
    watershedDlg->show();
    watershedDlg->exec();
}

void MainWindow::onWatershedPreviewed(const QVector<QVector<QPoint> > &pvv)
{
    previewSpace.newWatershedWork(pvv);
}

void MainWindow::onWatershedAccepted(const QVector<QVector<QPoint> > &pvv)
{
    disconnect(&previewSpace, &CAIGA::WorkSpace::workStarted, 0, 0);
    disconnect(&previewSpace, &CAIGA::WorkSpace::workFinished, 0, 0);
    preWorkSpace.newWatershedWork(pvv, true);
}

void MainWindow::onPreParametersAccepted()
{
    disconnect(&previewSpace, &CAIGA::WorkSpace::workStarted, 0, 0);
    disconnect(&previewSpace, &CAIGA::WorkSpace::workFinished, 0, 0);
    preWorkSpace.append(previewSpace.takeLast());
}

void MainWindow::onPreParametersRejected()
{
    disconnect(&previewSpace, &CAIGA::WorkSpace::workStarted, 0, 0);
    disconnect(&previewSpace, &CAIGA::WorkSpace::workFinished, 0, 0);
    this->onPreProcessWorkFinished();
}

void MainWindow::onContoursButtonClicked()
{
    preWorkSpace.newContoursWork();
}

void MainWindow::handleParametersDialogue(void (MainWindow::*paraChangedSlot)(int, double, double, bool))
{
    connect(&previewSpace, &CAIGA::WorkSpace::workStarted, parametersDlg, &ParametersDialog::handleWorkStarted);
    connect(&previewSpace, &CAIGA::WorkSpace::workFinished, parametersDlg, &ParametersDialog::handleWorkFinished);
    connect(&previewSpace, &CAIGA::WorkSpace::workFinished, this, &MainWindow::onPreviewWorkFinished);
    connect(parametersDlg, &ParametersDialog::parametersChanged, this, paraChangedSlot);
    connect(parametersDlg, &ParametersDialog::accepted, this, &MainWindow::onPreParametersAccepted);
    connect(parametersDlg, &ParametersDialog::rejected, this, &MainWindow::onPreParametersRejected);
    parametersDlg->show();
    parametersDlg->exec();
}

void MainWindow::onPreProcessWorkFinished()
{
    ui->preProcessDrawer->setImage(preWorkSpace.getLastDisplayImage());
}

void MainWindow::onPreProcessButtonBoxClicked(QAbstractButton *b)
{
    if (ui->preProcessButtonBox->standardButton(b) == QDialogButtonBox::Reset) {//reset
        //TODO
        preWorkSpace.reset(cgimg);
    }
    else {//save
        preWorkSpace.simplified();
        ui->analysisInteracter->setImage(preWorkSpace.getLastDisplayImage());
        ui->imageTabs->setCurrentIndex(3);

        //setup analyser and retrive information
        onMessagesArrived("Analysing... Please Wait......");
        analyser->setScaleValue(cgimg.getScaleValue());
        analyser->setMarkers(preWorkSpace.getMarkerMatrix());
        analyser->setContours(preWorkSpace.getContours());

        ui->analysisTableView->setModel(analyser->getDataModel());
        ui->analysisTableView->resizeColumnsToContents();
        //ui->analysisGeneralLabel->setText();//TODO
        ui->classComboBox->insertItems(0, analyser->getClassesList());
        connect(ui->analysisInteracter, &QImageInteractiveDrawer::mousePressed, analyser, &CAIGA::Analyser::findContourHasPoint);
        connect(analyser, &CAIGA::Analyser::statusString, this, &MainWindow::onMessagesArrived);
        connect(analyser, &CAIGA::Analyser::foundContourClass, ui->classComboBox, &QComboBox::setCurrentText);
        connect(analyser, &CAIGA::Analyser::foundContourIndex, ui->analysisTableView, &QTableView::setCurrentIndex);
        connect(ui->classComboBox, static_cast<void (QComboBox::*) (int)>(&QComboBox::currentIndexChanged), analyser, &CAIGA::Analyser::changeClass);
        onMessagesArrived("Analysed.");
    }
    previewSpace.clear();
}

void MainWindow::onClassAddButtonClicked()
{
    bool ok;
    QString clsName = QInputDialog::getText(this, "New Class", "Name", QLineEdit::Normal, QString(), &ok);
    if (ok) {
        analyser->addClass(clsName);
        ui->classComboBox->addItem(clsName);
    }
}

void MainWindow::onClassDelButtonClicked()
{
    int i = ui->classComboBox->currentIndex();
    if (ui->classComboBox->count() > 1 || i > 0) {
        analyser->deleteClass(i);
        ui->classComboBox->removeItem(i);
    }
}

void MainWindow::onCurrentTabChanged(int i)
{
    switch (i) {
    case 1://crop and calibre
        ui->ccPixelViewer->setLivePreviewEnabled(true);
        ui->actionRedo->setEnabled(false);
        ui->actionUndo->setEnabled(false);
        break;
    case 2://preprocess
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

void MainWindow::addDiskFileDialog()
{
    QString filename = QFileDialog::getOpenFileName(this, "Add Image from Disk", QDir::currentPath(),
                       "Supported Images (*.png *.jpg *.jpeg *.tif *.tiff *.bmp)");
    if (filename.isNull()) {
        return;
    }
    setCurrentDirbyFile(filename);
    cgimg.setRawImage(filename);
    ui->rawViewer->setPixmap(cgimg.getRawPixmap());
    ui->ccDrawer->setImage(cgimg.getRawImage());
    ccSpace.setImage(&cgimg);
    ui->imageTabs->setCurrentIndex(0);
}

void MainWindow::addCameraImageDialog()//TODO camera image should be involved with SQLite
{
    CameraDialog camDlg(this);
    camDlg.exec();
}

void MainWindow::onResetActionTriggered()
{
    ui->ccDrawer->reset();
    ui->imageTabs->setCurrentIndex(0);
    ui->preProcessDrawer->setImage(QImage());
    preWorkSpace.clear();
    previewSpace.clear();
}

void MainWindow::onOptionsActionTriggered()
{
    OptionsDialog optDlg(this);
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

void MainWindow::aboutQtDialog()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::aboutCAIGADialog()
{
    QMessageBox::about(this, "About CAIGA", aboutText);
}

void MainWindow::updateOptions(int lang, int toolbarStyle, int tabPos, const QString &colour)
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
    ui->ccDrawer->setPenColour(colour);
}

void MainWindow::readConfig()
{
    emit configReadFinished(settings.value("Language").toInt(),
                            settings.value("Toolbar Style").toInt(),
                            settings.value("Tab Position", 1).toInt(),
                            settings.value("Pen Colour", "#F00").toString());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    settings.setValue("MainGeometry", saveGeometry());//save MainWindow geometry
    settings.setValue("MainState", saveState());//save MainWindow toolbar state
    settings.setValue("CurrentDir", QDir::currentPath());
    event->accept();
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
