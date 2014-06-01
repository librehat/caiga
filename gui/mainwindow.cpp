#include "mainwindow.h"
#include "ui_mainwindow.h"
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

    //initialise pointers
    cropCalibreSpace = NULL;
    processSpace = NULL;
    previewSpace = NULL;
    cameraDlg = NULL;
    parametersDlg = NULL;
    watershedDlg = NULL;
    analyser = NULL;
    analysisDelegate = NULL;
    reporter = NULL;

    ui->rawViewer->setNoScale();
    //processTab button menu
    mouseBehaviourMenu = new QMenu(this);
    mouseBehaviourMenu->addAction(tr("Normal Arrow"), this, SLOT(onMouseNormalArrow()));
    mouseBehaviourMenu->addAction(tr("White Pencil"), this, SLOT(onMouseWhitePencil()));
    mouseBehaviourMenu->addAction(tr("Black Pencil"), this, SLOT(onMouseBlackPencil()));
    mouseBehaviourMenu->addAction(tr("White Eraser"), this, SLOT(onMouseWhiteEraser()));
    mouseBehaviourMenu->addAction(tr("Black Eraser"), this, SLOT(onMouseBlackEraser()));
    ui->mouseBehaviourButton->setMenu(mouseBehaviourMenu);

    //information
    ui->infoPlotter->setFont(QFont("Arial"));

    /*
     * Setup icons (only those can't be done with Designer)
     */
    ui->actionOptions->setIcon(QIcon::fromTheme("configure", QIcon::fromTheme("gconf-editor")));

    /*
     * SIGNALs and SLOTs
     * Only those that cannot be connected in Deisgner should be defined below
     */
    //crop and calibre tab
    connect(ui->buttonGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &MainWindow::ccModeChanged);//Operation Mode
    connect(ui->ccDrawer, &QImageDrawer::calibreFinished, ui->scaleDoubleSpinBox, &QDoubleSpinBox::setValue);
    connect(ui->ccDrawer, &QImageDrawer::gaugeLineResult, this, &MainWindow::onGaugeLineFinished);
    connect(ui->ccLoadMacroButton, &QPushButton::clicked, this, &MainWindow::onCCLoadMacroButtonClicked);
    connect(ui->ccSaveMacroButton, &QPushButton::clicked, this, &MainWindow::onCCSaveMacroButtonClicked);
    connect(ui->ccButtonBox, &QDialogButtonBox::clicked, this, &MainWindow::onCCButtonBoxClicked);

    //processTab
    connect(ui->invertGrayscaleButton, &QPushButton::clicked, this, &MainWindow::onInvertGrayscaleButtonClicked);
    connect(ui->histEqualiseButton, &QPushButton::clicked, this, &MainWindow::onHistEqualiseButtonClicked);
    connect(ui->gradientButton, &QPushButton::clicked, this, &MainWindow::onGradientButtonClicked);
    connect(ui->boxFilterButton, &QPushButton::clicked, this, &MainWindow::onBoxFilterButtonClicked);
    connect(ui->adaptiveBilateralFilter, &QPushButton::clicked, this, &MainWindow::onAdaptiveBilateralFilterButtonClicked);
    connect(ui->medianBlurButton, &QPushButton::clicked, this, &MainWindow::onMedianBlurButtonClicked);
    connect(ui->gaussianBinaryzationButton, &QPushButton::clicked, this, &MainWindow::onGaussianBinaryzationButtonClicked);
    connect(ui->medianBinaryzationButton, &QPushButton::clicked, this, &MainWindow::onMedianBinaryzationButtonClicked);
    connect(ui->floodFillButton, &QPushButton::clicked, this, &MainWindow::onFloodFillButtonClicked);
    connect(ui->cannyButton, &QPushButton::clicked, this, &MainWindow::onCannyButtonClicked);
    connect(ui->watershedButton, &QPushButton::clicked, this, &MainWindow::onWatershedButtonClicked);
    connect(ui->contoursButton, &QPushButton::clicked, this, &MainWindow::onContoursButtonClicked);
    connect(ui->processButtonBox, &QDialogButtonBox::clicked, this, &MainWindow::onProcessButtonBoxClicked);

    //Analysis
    connect(ui->analysisButtonBox, &QDialogButtonBox::clicked, this, &MainWindow::onAnalysisButtonBoxClicked);

    //Information
    connect(ui->splitSpinBox, static_cast<void (QSpinBox::*) (int)>(&QSpinBox::valueChanged), this, &MainWindow::onSplitSpinBoxValueChanged);
    connect(ui->splitOKPushButton,  &QPushButton::clicked, this, &MainWindow::onSplitButtonClicked);

    //MainWindow
    connect(ui->actionUndo, &QAction::triggered, this, &MainWindow::onUndoTriggered);
    connect(ui->actionRedo, &QAction::triggered, this, &MainWindow::onRedoTriggered);
    connect(ui->actionOpenFile, &QAction::triggered, this, &MainWindow::addDiskFileDialog);
    connect(ui->actionOpenCamera, &QAction::triggered, this, &MainWindow::addCameraImageDialog);
    connect(ui->actionReset, &QAction::triggered, this, &MainWindow::onResetActionTriggered);
    connect(ui->actionOptions, &QAction::triggered, this, &MainWindow::onOptionsActionTriggered);
    connect(ui->actionSaveCurrentImageAs, &QAction::triggered, this, &MainWindow::onSaveCurrentImageTriggered);
    connect(ui->actionQuickReportExport, &QAction::triggered, this, &MainWindow::onQuickExportTriggered);
    connect(ui->actionExportReportAs, &QAction::triggered, this, &MainWindow::onInformationExportTriggered);
    connect(ui->actionAbout_Qt, &QAction::triggered, this, &MainWindow::aboutQtDialog);
    connect(ui->actionAbout_CAIGA, &QAction::triggered, this, &MainWindow::aboutCAIGADialog);
    connect(ui->imageTabs, &QTabWidget::currentChanged, this, &MainWindow::onCurrentTabChanged);
    connect(this, &MainWindow::messageArrived, this, &MainWindow::onMessagesArrived);
    connect(this, &MainWindow::configReadFinished, this, &MainWindow::updateOptions);

    readConfig();

    //disable tabs because user shouldn't click buttons in other tabs if there is no active image
    ui->imageTabs->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
    // Qt delete children objects automatically once the parent is deleted
}

const QString MainWindow::aboutText = QString() + "<h3>Computer-Aid Interactive Grain Analyser</h3><p>Version 2014.0 Beta for "

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
        + "</p><p>Built on " + __DATE__ + " at " + __TIME__ + " by "
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

        + "</p><p>Copyright &copy; 2014 <a href='http://www.librehat.com'>William Wong</a> (<a href='mailto:hzwhuang@gmail.com'>hzwhuang@gmail.com</a>) and other contributors.<br /><a href= 'http://smse.seu.edu.cn'>School of Materials Science and Engineering</a>, <a href='http://www.seu.edu.cn'>Southeast University</a>.</p><p>Licensed under <a href='http://www.gnu.org/licenses/gpl-3.0.html'>GPLv3</a>.<br /></p><p><strong>The software contains third-party libraries and artwork below.</strong><br /><a href='http://qcustomplot.com'>QCustomPlot</a> licensed under <a href='http://www.gnu.org/licenses/gpl.html'>GPL</a><br /><a href='http://opencv.org'>OpenCV</a> licensed under <a href='https://github.com/Itseez/opencv/blob/master/LICENSE'>3-clause BSD License</a><br /><a href='http://www.oxygen-icons.org'>Oxygen Icons</a> licensed under <a href='http://www.gnu.org/licenses/lgpl-3.0.txt'>LGPLv3</a><br /></p><div>THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.</div>";

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
    QString macroFile = QFileDialog::getOpenFileName(this, tr("Load Macro for Crop and Calibre"), QDir::currentPath(), "Macro Text File (*.txt)");
    if (macroFile.isNull()) {
        return;
    }
    setCurrentDirbyFile(macroFile);
    CAIGA::Macro macro(this);
    macro.setCCSpace(cropCalibreSpace);
    macro.doCropAndCalibreMacroFromFile(macroFile);
    macro.deleteLater();
    ui->ccDrawer->update();
}

void MainWindow::onCCSaveMacroButtonClicked()
{
    QString macroFile = QFileDialog::getSaveFileName(this, tr("Save as Macro File for Crop and Calibre"), QDir::currentPath(), "Macro Text File (*.txt)");
    if (macroFile.isNull()) {
        return;
    }
    setCurrentDirbyFile(macroFile);
    CAIGA::Macro macro(this);
    macro.setCCSpace(cropCalibreSpace);
    macro.saveCropAndCalibreAsMacroFile(macroFile);
    macro.deleteLater();
}

void MainWindow::onCCButtonBoxClicked(QAbstractButton *b)
{
    if (ui->ccButtonBox->standardButton(b) == QDialogButtonBox::Reset) {
        ui->ccDrawer->reset();
        ui->scaleDoubleSpinBox->setValue(0);
        ui->cropRectRadio->setChecked(true);
    }
    else {//save
        if (cgimg.getScaleValue() <= 0) {
            onMessagesArrived(tr("You must calibre image scale before move to next step."));
            return;
        }
        cropCalibreSpace->cropImage();

        if (processSpace != NULL) {
            processSpace->disconnect();
            processSpace->deleteLater();
        }
        processSpace = new CAIGA::WorkSpace(cgimg.croppedMatrix(), this);
        connect(processSpace, &CAIGA::WorkSpace::workFinished, this, &MainWindow::onProcessWorkFinished);
        connect(processSpace, &CAIGA::WorkSpace::workStatusStringUpdated, this, &MainWindow::onMessagesArrived);

        if (previewSpace != NULL) {
            previewSpace->disconnect();
            previewSpace->deleteLater();
        }
        previewSpace = new CAIGA::WorkSpace(NULL, this);
        connect(previewSpace, &CAIGA::WorkSpace::workStatusStringUpdated, this, &MainWindow::onMessagesArrived);

        ui->processTab->setEnabled(true);
        ui->imageTabs->setCurrentIndex(2);
        onMessagesArrived(tr("Process and segment the image."));
        onProcessWorkFinished();//update the image
    }
}

void MainWindow::onMouseNormalArrow()
{
    ui->mouseBehaviourButton->setText(tr("Normal Arrow"));
    ui->processDrawer->setDrawMode(QImageInteractiveDrawer::NONE);
    disconnect(ui->processDrawer, &QImageInteractiveDrawer::mouseReleased, 0, 0);
}

void MainWindow::onMouseWhitePencil()
{
    disconnect(ui->processDrawer, &QImageInteractiveDrawer::mouseReleased, 0, 0);
    ui->mouseBehaviourButton->setText(tr("White Pencil"));
    ui->processDrawer->setWhite();
    ui->processDrawer->setDrawMode(QImageInteractiveDrawer::PENCIL);
    connect(ui->processDrawer, &QImageInteractiveDrawer::mouseReleased, this, &MainWindow::onPencilDrawFinished);
}

void MainWindow::onMouseBlackPencil()
{
    disconnect(ui->processDrawer, &QImageInteractiveDrawer::mouseReleased, 0, 0);
    ui->mouseBehaviourButton->setText(tr("Black Pencil"));
    ui->processDrawer->setWhite(false);
    ui->processDrawer->setDrawMode(QImageInteractiveDrawer::PENCIL);
    connect(ui->processDrawer, &QImageInteractiveDrawer::mouseReleased, this, &MainWindow::onPencilDrawFinished);
}

void MainWindow::onMouseWhiteEraser()
{
    disconnect(ui->processDrawer, &QImageInteractiveDrawer::mouseReleased, 0, 0);
    ui->mouseBehaviourButton->setText(tr("White Eraser"));
    ui->processDrawer->setWhite();
    ui->processDrawer->setDrawMode(QImageInteractiveDrawer::ERASER);
    connect(ui->processDrawer, &QImageInteractiveDrawer::mouseReleased, this, &MainWindow::onEraserDrawFinished);
}

void MainWindow::onMouseBlackEraser()
{
    disconnect(ui->processDrawer, &QImageInteractiveDrawer::mouseReleased, 0, 0);
    ui->mouseBehaviourButton->setText(tr("Black Eraser"));
    ui->processDrawer->setWhite(false);
    ui->processDrawer->setDrawMode(QImageInteractiveDrawer::ERASER);
    connect(ui->processDrawer, &QImageInteractiveDrawer::mouseReleased, this, &MainWindow::onEraserDrawFinished);
}

void MainWindow::onPreviewWorkFinished()
{
    ui->processDrawer->setImage(previewSpace->getLastDisplayImage());
}

void MainWindow::onInvertGrayscaleButtonClicked()
{
    processSpace->newInvertGrayscaleWork();
}

void MainWindow::onHistEqualiseButtonClicked()
{
    processSpace->newHistogramEqualiseWork();
}

void MainWindow::onGradientButtonClicked()
{
    previewSpace->reset(processSpace->getLastMatrix());
    if (parametersDlg != NULL) {
        parametersDlg->disconnect();
        parametersDlg->deleteLater();
    }
    parametersDlg = new ParametersDialog(CAIGA::WorkBase::Gradient, this);
    handleParametersDialogue(&MainWindow::onGradientParamatersChanged);
}

void MainWindow::onGradientParamatersChanged(int kSize, double, double, bool)
{
    previewSpace->newGradientWork(kSize);
}

void MainWindow::onBoxFilterButtonClicked()
{
    previewSpace->reset(processSpace->getLastMatrix());
    if (parametersDlg != NULL) {
        parametersDlg->disconnect();
        parametersDlg->deleteLater();
    }
    parametersDlg = new ParametersDialog(CAIGA::WorkBase::BoxFilter, this);
    handleParametersDialogue(&MainWindow::onBoxFilterParametersChanged);
}

void MainWindow::onBoxFilterParametersChanged(int kSize, double, double, bool)
{
    previewSpace->newBoxFilterWork(kSize);
}

void MainWindow::onAdaptiveBilateralFilterButtonClicked()
{
    previewSpace->reset(processSpace->getLastMatrix());
    if (parametersDlg != NULL) {
        parametersDlg->disconnect();
        parametersDlg->deleteLater();
    }
    parametersDlg = new ParametersDialog(CAIGA::WorkBase::AptBilateralFilter, this);
    handleParametersDialogue(&MainWindow::onAdaptiveBilateralFilterParametersChanged);
}

void MainWindow::onAdaptiveBilateralFilterParametersChanged(int k, double s, double c, bool)
{
    previewSpace->newAdaptiveBilateralFilterWork(k, s, c);
}

void MainWindow::onMedianBlurButtonClicked()
{
    previewSpace->reset(processSpace->getLastMatrix());
    if (parametersDlg != NULL) {
        parametersDlg->disconnect();
        parametersDlg->deleteLater();
    }
    parametersDlg = new ParametersDialog(CAIGA::WorkBase::MedianBlur, this);
    handleParametersDialogue(&MainWindow::onMedianBlurParametersChanged);
}

void MainWindow::onMedianBlurParametersChanged(int k, double, double, bool)
{
    previewSpace->newMedianBlurWork(k);
}

void MainWindow::onGaussianBinaryzationButtonClicked()
{
    previewSpace->reset(processSpace->getLastMatrix());
    if (parametersDlg != NULL) {
        parametersDlg->disconnect();
        parametersDlg->deleteLater();
    }
    parametersDlg = new ParametersDialog(CAIGA::WorkBase::Binaryzation, this);
    handleParametersDialogue(&MainWindow::onGaussianBinaryzationParametersChanged);
}

void MainWindow::onGaussianBinaryzationParametersChanged(int s, double c, double, bool inv)
{
    previewSpace->newBinaryzationWork(CV_ADAPTIVE_THRESH_GAUSSIAN_C, inv ? CV_THRESH_BINARY_INV : CV_THRESH_BINARY, s, c);
}

void MainWindow::onMedianBinaryzationButtonClicked()
{
    previewSpace->reset(processSpace->getLastMatrix());
    if (parametersDlg != NULL) {
        parametersDlg->disconnect();
        parametersDlg->deleteLater();
    }
    parametersDlg = new ParametersDialog(CAIGA::WorkBase::Binaryzation, this);
    handleParametersDialogue(&MainWindow::onMedianBinaryzationParametersChanged);
}

void MainWindow::onMedianBinaryzationParametersChanged(int s, double c, double, bool inv)
{
    previewSpace->newBinaryzationWork(CV_ADAPTIVE_THRESH_MEAN_C, inv ? CV_THRESH_BINARY_INV : CV_THRESH_BINARY, s, c);
}

void MainWindow::onPencilDrawFinished(const QVector<QPoint> &pts)
{
    processSpace->newPencilWork(pts, ui->processDrawer->isWhite());
}

void MainWindow::onEraserDrawFinished(const QVector<QPoint> &pts)
{
    processSpace->newEraserWork(pts, ui->processDrawer->isWhite());
}

void MainWindow::onFloodFillButtonClicked()
{
    previewSpace->reset(processSpace->getLastMatrix());
    if (parametersDlg != NULL) {
        parametersDlg->disconnect();
        parametersDlg->deleteLater();
    }
    parametersDlg = new ParametersDialog(CAIGA::WorkBase::FloodFill, this);

    connect(previewSpace, &CAIGA::WorkSpace::workFinished, this, &MainWindow::onPreviewWorkFinished);
    connect(parametersDlg, &ParametersDialog::parametersChanged, this, &MainWindow::onFloodFillParametersChanged);
    connect(parametersDlg, &ParametersDialog::undoButtonClicked, previewSpace, &CAIGA::WorkSpace::undo);
    connect(parametersDlg, &ParametersDialog::redoButtonClicked, previewSpace, &CAIGA::WorkSpace::redo);
    connect(parametersDlg, &ParametersDialog::accepted, this, &MainWindow::onFloodFillAccepted);
    connect(parametersDlg, &ParametersDialog::rejected, this, &MainWindow::onFloodFillRejected);
    connect(parametersDlg, &ParametersDialog::resetButtonClicked, previewSpace, static_cast<void (CAIGA::WorkSpace::*) (void)>(&CAIGA::WorkSpace::reset));
    connect(ui->processDrawer, &QImageInteractiveDrawer::mousePressed, this, &MainWindow::onFloodFillMouseClicked);
    parametersDlg->setModal(false);
    parametersDlg->show();
    parametersDlg->exec();
}

void MainWindow::onFloodFillAccepted()
{
    this->onParametersAccepted();
    disconnect(ui->processDrawer, &QImageInteractiveDrawer::mousePressed, this, &MainWindow::onFloodFillMouseClicked);
    floodfillPts.clear();
}

void MainWindow::onFloodFillRejected()
{
    this->onParametersRejected();
    disconnect(ui->processDrawer, &QImageInteractiveDrawer::mousePressed, this, &MainWindow::onFloodFillMouseClicked);
    floodfillPts.clear();
}

void MainWindow::onCannyButtonClicked()
{
    previewSpace->reset(processSpace->getLastMatrix());
    if (parametersDlg != NULL) {
        parametersDlg->disconnect();
        parametersDlg->deleteLater();
    }
    parametersDlg = new ParametersDialog(CAIGA::WorkBase::Canny, this);
    handleParametersDialogue(&MainWindow::onCannyParametersChanged);
}

void MainWindow::onWatershedButtonClicked()
{
    previewSpace->reset(processSpace->getLastMatrix());
    if (watershedDlg != NULL) {
        watershedDlg->disconnect();
        watershedDlg->deleteLater();
    }
    watershedDlg = new WatershedMarkerDialog(this);
    watershedDlg->setPenColour(ui->ccDrawer->getPenColour());
    watershedDlg->setOriginalMat(processSpace->getLastMatrix(), processSpace->last()->inputMarker);
    connect(watershedDlg, &WatershedMarkerDialog::reseted, this, &MainWindow::onProcessWorkFinished);
    connect(watershedDlg, &WatershedMarkerDialog::previewTriggered, this, &MainWindow::onWatershedPreviewed);
    connect(watershedDlg, &WatershedMarkerDialog::accepted, this, &MainWindow::onWatershedAccepted);
    connect(watershedDlg, &WatershedMarkerDialog::rejected, this, &MainWindow::onParametersRejected);
    connect(previewSpace, &CAIGA::WorkSpace::workStarted, watershedDlg, &WatershedMarkerDialog::onPreviewStarted);
    connect(previewSpace, &CAIGA::WorkSpace::workFinished, watershedDlg, &WatershedMarkerDialog::onPreviewFinished);
    connect(previewSpace, &CAIGA::WorkSpace::workFinished, this, &MainWindow::onPreviewWorkFinished);
    watershedDlg->show();
    watershedDlg->exec();
}

void MainWindow::onWatershedAccepted()
{
    disconnect(previewSpace, &CAIGA::WorkSpace::workStarted, 0, 0);
    disconnect(previewSpace, &CAIGA::WorkSpace::workFinished, 0, 0);
    processSpace->append(previewSpace->takeLast());
}

void MainWindow::onParametersAccepted()
{
    disconnect(previewSpace, &CAIGA::WorkSpace::workStarted, 0, 0);
    disconnect(previewSpace, &CAIGA::WorkSpace::workFinished, 0, 0);
    processSpace->append(previewSpace->takeLast());
}

void MainWindow::onParametersRejected()
{
    disconnect(previewSpace, &CAIGA::WorkSpace::workStarted, 0, 0);
    disconnect(previewSpace, &CAIGA::WorkSpace::workFinished, 0, 0);
    this->onProcessWorkFinished();
}

void MainWindow::handleParametersDialogue(void (MainWindow::*paraChangedSlot)(int, double, double, bool))
{
    connect(previewSpace, &CAIGA::WorkSpace::workStarted, parametersDlg, &ParametersDialog::handleWorkStarted);
    connect(previewSpace, &CAIGA::WorkSpace::workFinished, parametersDlg, &ParametersDialog::handleWorkFinished);
    connect(previewSpace, &CAIGA::WorkSpace::workFinished, this, &MainWindow::onPreviewWorkFinished);
    connect(parametersDlg, &ParametersDialog::parametersChanged, this, paraChangedSlot);
    connect(parametersDlg, &ParametersDialog::accepted, this, &MainWindow::onParametersAccepted);
    connect(parametersDlg, &ParametersDialog::rejected, this, &MainWindow::onParametersRejected);
    parametersDlg->show();
    parametersDlg->exec();
}

void MainWindow::onProcessWorkFinished()
{
    ui->processDrawer->setImage(processSpace->getLastDisplayImage());
}

void MainWindow::onProcessButtonBoxClicked(QAbstractButton *b)
{
    if (ui->processButtonBox->standardButton(b) == QDialogButtonBox::Reset) {//reset
        processSpace->reset(cgimg.croppedMatrix());
    }
    else {//save
        //check if it's eligible
        if (processSpace->getMarkerMatrix() == NULL) {
            onMessagesArrived(tr("Error. Processing is not finished yet!"));
            return;
        }

        ui->analysisTab->setEnabled(true);
        ui->analysisInteracter->setImage(processSpace->getLastDisplayImage());
        ui->imageTabs->setCurrentIndex(3);

        onMessagesArrived(tr("Analysing... Please Wait......"));
        //setup analyser and retrive information
        if (analyser != NULL) {
            analyser->disconnect();
            analyser->deleteLater();
        }
        analyser = new CAIGA::Analyser(cgimg.getScaleValue(), processSpace->getMarkerMatrix(), processSpace->getContours(), this);

        if (analysisDelegate != NULL) {
            analysisDelegate->disconnect();
            analysisDelegate->deleteLater();
        }
        analysisDelegate = new AnalysisItemDelegate(ui->analysisTableView);
        ui->analysisTableView->setItemDelegate(analysisDelegate);
        analysisDelegate->setClassesList(analyser->getClassesList());

        ui->analysisTableView->setModel(analyser->getDataModel());
        ui->analysisTableView->resizeColumnsToContents();
        connect(analyser, &CAIGA::Analyser::foundContourIndex, this, &MainWindow::onContourIndexFound);
        connect(analyser, &CAIGA::Analyser::currentClassChanged, this, &MainWindow::onCurrentClassChanged);
        connect(ui->analysisInteracter, &QImageInteractiveDrawer::mousePressed, analyser, &CAIGA::Analyser::findContourHasPoint);
        connect(ui->analysisTableView, &QTableView::activated, analyser, &CAIGA::Analyser::onModelIndexChanged);
        connect(analysisDelegate, &AnalysisItemDelegate::classChanged, analyser, &CAIGA::Analyser::onClassChanged);
        onMessagesArrived(tr("Analysed."));
    }
    previewSpace->clear();
}

void MainWindow::onContourIndexFound(const QModelIndex &i)
{
    /*
     * setCurrentIndex won't emit the activated signal
     * so we need to send out this signal manually
     */
    ui->analysisTableView->setCurrentIndex(i);
    emit ui->analysisTableView->activated(i);
}

void MainWindow::onCurrentClassChanged(int idx)
{
    ui->analysisCurrentClassLabel->setText(analyser->getClassValues(idx));
}

void MainWindow::onAnalysisButtonBoxClicked(QAbstractButton *b)
{
    if (ui->analysisButtonBox->standardButton(b) == QDialogButtonBox::Reset) {
        analyser->reset();
        ui->analysisTableView->resizeColumnsToContents();
    }
    else {//save
        ui->infoTab->setEnabled(true);
        onSplitButtonClicked();
        ui->imageTabs->setCurrentIndex(4);
    }
}

void MainWindow::onSplitSpinBoxValueChanged(int)
{
    ui->splitOKPushButton->setEnabled(true);
}

void MainWindow::onSplitButtonClicked()
{
    ui->infoTextBrowser->clear();
    updateInformationReport(ui->splitSpinBox->value());
    ui->splitOKPushButton->setEnabled(false);
}

void MainWindow::onReportAvailable(bool a)
{
    ui->actionExportReportAs->setEnabled(a);
    ui->actionQuickReportExport->setEnabled(a);
}

void MainWindow::updateInformationReport(int split)
{
    onReportAvailable(false);
    if (reporter != NULL) {
        reporter->disconnect();
        reporter->deleteLater();
    }
    reporter = new Reporter(analyser, processSpace, split, cgimg.getRawImage(), this);
    connect(reporter, &Reporter::reportAvailable, this, &MainWindow::onReportAvailable);
    connect(reporter, &Reporter::workStatusStrUpdated, this, &MainWindow::onMessagesArrived);
    connect(reporter, &Reporter::reportGenerated, ui->infoTextBrowser, &QTextBrowser::setDocument);
    reporter->setBarChart(ui->infoPlotter);
    reporter->generateReport();
}

void MainWindow::onCurrentTabChanged(int i)
{
    ui->ccPixelViewer->setLivePreviewEnabled(false);
    ui->actionRedo->setEnabled(false);
    ui->actionUndo->setEnabled(false);
    ui->actionSaveCurrentImageAs->setEnabled(false);

    switch (i) {
    case 1://crop and calibre
        ui->ccPixelViewer->setLivePreviewEnabled(true);
        ui->actionSaveCurrentImageAs->setEnabled(false);
        break;
    case 2://process
        ui->actionRedo->setEnabled(true);
        ui->actionUndo->setEnabled(true);
        ui->actionSaveCurrentImageAs->setEnabled(true);
        break;
    case 3://analysis
        ui->actionSaveCurrentImageAs->setEnabled(true);
        break;
    }
}

void MainWindow::onUndoTriggered()
{
    processSpace->undo();
    if (processSpace->count() < 2) {
        ui->actionUndo->setEnabled(false);
    }
    ui->actionRedo->setEnabled(true);
}

void MainWindow::onRedoTriggered()
{
    processSpace->redo();
    if (processSpace->countUndone() < 1) {
        ui->actionRedo->setEnabled(false);
    }
    ui->actionUndo->setEnabled(true);
}

void MainWindow::addDiskFileDialog()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Add Image from Disk"), QDir::currentPath(),
                       "Supported Images (*.png *.jpg *.jpeg *.tif *.tiff *.bmp)");
    if (filename.isNull()) {
        return;
    }
    setCurrentDirbyFile(filename);
    cgimg.setRawImageByFile(filename);
    setupCropCalibreSpace();
    ui->rawViewer->setPixmap(cgimg.getRawPixmap());
    ui->ccDrawer->setImage(cgimg.getRawImage());
    onReportAvailable(false);
    ui->imageTabs->setEnabled(true);
    ui->processTab->setEnabled(false);
    ui->analysisTab->setEnabled(false);
    ui->infoTab->setEnabled(false);
    ui->imageTabs->setCurrentIndex(0);
}

void MainWindow::addCameraImageDialog()
{
    cameraDlg = new CameraDialog(this);
    connect(cameraDlg, &CameraDialog::imageAccepted, this, &MainWindow::onCameraImageAccepted);
    cameraDlg->show();
    cameraDlg->exec();
    cameraDlg->deleteLater();
}

void MainWindow::onCameraImageAccepted(const QImage &img)
{
    cgimg.setRawImageFromCamera(img);
    setupCropCalibreSpace();
    ui->rawViewer->setPixmap(cgimg.getRawPixmap());
    ui->ccDrawer->setImage(cgimg.getRawImage());
    onReportAvailable(false);
    ui->imageTabs->setEnabled(true);
    ui->processTab->setEnabled(false);
    ui->analysisTab->setEnabled(false);
    ui->infoTab->setEnabled(false);
    ui->imageTabs->setCurrentIndex(0);
}

void MainWindow::onResetActionTriggered()
{
    ui->ccDrawer->reset();
    ui->imageTabs->setCurrentIndex(0);
    ui->processDrawer->setImage(QImage());
    if (processSpace != NULL) {
        processSpace->clear();
    }
    if (previewSpace != NULL) {
        previewSpace->clear();
    }
}

void MainWindow::onOptionsActionTriggered()
{
    OptionsDialog optDlg(this);
    connect(&optDlg, &OptionsDialog::optionsAccepted, this, &MainWindow::updateOptions);
    optDlg.exec();
}

void MainWindow::onQuickExportTriggered()
{
    QFileInfo imgFile(cgimg.getFileName());
    QString pdfname = imgFile.canonicalPath() + QString("/") + imgFile.completeBaseName();
    reporter->exportAsPDF(pdfname);
}

void MainWindow::onInformationExportTriggered()
{
    QString filter;
    QString filename = QFileDialog::getSaveFileName(this,
                       tr("Export Image Information As"),
                       QDir::currentPath(),
                       "Adobe Portable Document Format (*.pdf);;Open Document File (*.odf);;HyperText Markup Language (*.html)", &filter);
    if (filename.isNull()) {
        return;
    }
    setCurrentDirbyFile(filename);
    if (filter.contains("pdf")) {
        reporter->exportAsPDF(filename);
    }
    else {
        QByteArray format;
        if (filter.contains("odf")) {
            format.append("ODF");
        }
        else {
            format.append("HTML");
        }
        reporter->exportAsFormat(filename, format);
    }
}

void MainWindow::onSaveCurrentImageTriggered()
{
    QString filter;
    QString filename = QFileDialog::getSaveFileName(this,
                       tr("Save Current Image As"),
                       QDir::currentPath(),
                       "Joint Photographic Experts Group (*.jpg);;Portable Network Graphics (*.png)", &filter);
    if (filename.isNull()) {
        return;
    }
    setCurrentDirbyFile(filename);
    QByteArray format;
    if (filter.contains("jpg")) {
        format.append("JPG");
    }
    else {
        format.append("PNG");
    }
    bool ok = false;
    switch (ui->imageTabs->currentIndex()) {
    case 2://process
        ok = ui->processDrawer->image()->save(filename, format.data());
        break;
    case 3://analysis
        ok = ui->analysisInteracter->image()->save(filename, format.data());
        break;
    default:
        onMessagesArrived(tr("Invalid operation."));
    }
    if (ok) {
        onMessagesArrived(tr("Current image saved successfully."));
    }
    else {
        onMessagesArrived(tr("Saving current image failed."));
    }
}

void MainWindow::updateOptions(int lang, int toolbarStyle, int tabPos, const QString &colour)
{
    Q_UNUSED(lang);//TODO: i18n, 0: <system>, 1: en, 2: zh_CN

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

void MainWindow::setupCropCalibreSpace()
{
    if (cropCalibreSpace != NULL) {
        cropCalibreSpace->disconnect();
        cropCalibreSpace->deleteLater();
    }
    cropCalibreSpace = new CAIGA::CCSpace(&cgimg, this);
    connect(ui->scaleDoubleSpinBox, static_cast<void (QDoubleSpinBox::*) (double)>(&QDoubleSpinBox::valueChanged), cropCalibreSpace, &CAIGA::CCSpace::setScaleValue);
    ui->ccDrawer->setSpace(cropCalibreSpace);
}

void MainWindow::onMessagesArrived(const QString &str)
{
    ui->statusBar->showMessage(str);
}
