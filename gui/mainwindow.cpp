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
    calibreSpace = NULL;
    processSpace = NULL;
    previewSpace = NULL;
    cameraDlg = NULL;
    parametersDlg = NULL;
    watershedDlg = NULL;
    analyser = NULL;
    analysisDelegate = NULL;
    reporter = NULL;

    //ui->rawViewer->setNoScale();
    //processTab button menu
    mouseBehaviourMenu = new QMenu(this);
    mouseBehaviourMenu->addAction(QIcon::fromTheme("edit-select"), tr("Normal Arrow"), this, SLOT(onMouseNormalArrow()));
    mouseBehaviourMenu->addAction(QIcon::fromTheme("draw-freehand"), tr("White Pencil"), this, SLOT(onMouseWhitePencil()));
    mouseBehaviourMenu->addAction(QIcon::fromTheme("draw-freehand"), tr("Black Pencil"), this, SLOT(onMouseBlackPencil()));
    mouseBehaviourMenu->addAction(QIcon::fromTheme("draw-eraser"), tr("White Eraser"), this, SLOT(onMouseWhiteEraser()));
    mouseBehaviourMenu->addAction(QIcon::fromTheme("draw-eraser"), tr("Black Eraser"), this, SLOT(onMouseBlackEraser()));
    ui->mouseBehaviourButton->setMenu(mouseBehaviourMenu);

    //report
    ui->diameterBarPlotter->setFont(QFont("Arial"));

    //add zoom information label to statusBar
    zoomLabel = new QLabel(ui->statusBar);
    zoomLabel->setFrameShape(QFrame::StyledPanel);
    zoomLabel->setFrameShadow(QFrame::Sunken);
    ui->statusBar->addPermanentWidget(zoomLabel);

    /*
     * Setup icons (only those can't be done with Designer)
     */
    ui->actionOptions->setIcon(QIcon::fromTheme("configure", QIcon::fromTheme("gconf-editor")));

    /*
     * SIGNALs and SLOTs
     * Only those that cannot be connected in Deisgner should be defined below
     */
    //calibre tab
    connect(ui->buttonGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &MainWindow::calibreModeChanged);//Operation Mode
    connect(ui->calibreDrawer, &QImageDrawer::calibreFinished, ui->scaleDoubleSpinBox, &QDoubleSpinBox::setValue);
    connect(ui->calibreDrawer, &QImageDrawer::gaugeLineResult, this, &MainWindow::onMeasureFinished);
    connect(ui->calibreLoadMacroButton, &QPushButton::clicked, this, &MainWindow::onCalibreLoadMacroButtonClicked);
    connect(ui->calibreSaveMacroButton, &QPushButton::clicked, this, &MainWindow::onCalibreSaveMacroButtonClicked);
    connect(ui->calibreButtonBox, &QDialogButtonBox::clicked, this, &MainWindow::onCalibreButtonBoxClicked);

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

    //Report
    connect(ui->splitSpinBox, static_cast<void (QSpinBox::*) (int)>(&QSpinBox::valueChanged), this, &MainWindow::onSplitSpinBoxValueChanged);
    connect(ui->splitOKPushButton,  &QPushButton::clicked, this, &MainWindow::onSplitButtonClicked);

    //MainWindow
    connect(ui->actionUndo, &QAction::triggered, this, &MainWindow::onUndoTriggered);
    connect(ui->actionRedo, &QAction::triggered, this, &MainWindow::onRedoTriggered);
    connect(ui->actionOpenFile, &QAction::triggered, this, &MainWindow::onAddDiskImageTriggered);
    connect(ui->actionOpenCamera, &QAction::triggered, this, &MainWindow::onAddCameraImageTriggered);
    connect(ui->actionReset, &QAction::triggered, this, &MainWindow::onResetActionTriggered);
    connect(ui->actionOptions, &QAction::triggered, this, &MainWindow::onOptionsActionTriggered);
    connect(ui->actionSaveCurrentImageAs, &QAction::triggered, this, &MainWindow::onSaveCurrentImageTriggered);
    connect(ui->actionQuickReportExport, &QAction::triggered, this, &MainWindow::onQuickExportTriggered);
    connect(ui->actionExportReportAs, &QAction::triggered, this, &MainWindow::onReportExportTriggered);
    connect(ui->actionAbout_Qt, &QAction::triggered, this, &MainWindow::onAboutQt);
    connect(ui->actionAbout_CAIGA, &QAction::triggered, this, &MainWindow::onAboutCAIGA);
    connect(ui->imageTabs, &QTabWidget::currentChanged, this, &MainWindow::onCurrentTabChanged);
    connect(this, &MainWindow::messageArrived, this, &MainWindow::onMessagesArrived);
    connect(this, &MainWindow::configReadFinished, this, &MainWindow::updateOptions);

    //Zoom
    connect(ui->calibreDrawer, &QImageDrawer::zoomUpdated, this, &MainWindow::onZoomUpdated);
    connect(ui->processDrawer, &QImageInteractiveDrawer::zoomUpdated, this, &MainWindow::onZoomUpdated);
    connect(ui->analysisInteracter, &QImageInteractiveDrawer::zoomUpdated, this, &MainWindow::onZoomUpdated);

    readConfig();

    //disable tabs because user shouldn't click buttons in other tabs if there is no active image
    ui->imageTabs->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
    // Qt delete children objects automatically once the parent is deleted
}

const QString MainWindow::aboutText = QString() + "<h3>Computer-Aid Interactive Grain Analyser</h3><p>Version 2014.1 for "

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

void MainWindow::calibreModeChanged(int i)
{
    ui->calibreDrawer->setDrawMode(i);
}

void MainWindow::onMeasureFinished(qreal r)
{
    ui->measureResultLabel->setText(QString("%1 μm").arg(r));
}

void MainWindow::onCalibreLoadMacroButtonClicked()
{
    QString macroFile = QFileDialog::getOpenFileName(this, tr("Load Macro for Calibre"), QDir::currentPath(), "Macro Text File (*.txt)");
    if (macroFile.isNull()) {
        return;
    }
    setCurrentDirbyFile(macroFile);
    CAIGA::Macro macro(calibreSpace, this);
    macro.doCalibreMacroFromFile(macroFile);
    macro.deleteLater();
    ui->calibreDrawer->update();
}

void MainWindow::onCalibreSaveMacroButtonClicked()
{
    QString macroFile = QFileDialog::getSaveFileName(this, tr("Save as Macro File for Calibre"), QDir::currentPath(), "Macro Text File (*.txt)");
    if (macroFile.isNull()) {
        return;
    }
    setCurrentDirbyFile(macroFile);
    CAIGA::Macro macro(calibreSpace, this);
    macro.saveCalibreAsMacroFile(macroFile);
    macro.deleteLater();
}

void MainWindow::onCalibreButtonBoxClicked(QAbstractButton *b)
{
    if (ui->calibreButtonBox->standardButton(b) == QDialogButtonBox::Reset) {
        ui->calibreDrawer->reset();
        ui->scaleDoubleSpinBox->setValue(0);
        ui->rectRadio->setChecked(true);
    }
    else {//save
        if (cgimg.getScaleValue() <= 0) {
            onMessagesArrived(tr("You must calibre image scale before move to next step."));
            return;
        }
        calibreSpace->cropImage();

        if (processSpace != NULL) {
            processSpace->disconnect();
            processSpace->deleteLater();
        }
        processSpace = new CAIGA::WorkSpace(cgimg.croppedMatrix(), m_undoSteps, true, false, this);
        connect(processSpace, &CAIGA::WorkSpace::workFinished, this, &MainWindow::onProcessWorkFinished);
        connect(processSpace, &CAIGA::WorkSpace::workStatusStringUpdated, this, &MainWindow::onMessagesArrived);

        if (previewSpace != NULL) {
            previewSpace->disconnect();
            previewSpace->deleteLater();
        }
        previewSpace = new CAIGA::WorkSpace(NULL, m_undoSteps, true, true, this);
        connect(previewSpace, &CAIGA::WorkSpace::workStatusStringUpdated, this, &MainWindow::onMessagesArrived);

        ui->processTab->setEnabled(true);
        ui->imageTabs->setCurrentIndex(2);
        onMessagesArrived(tr("Process and segment the image."));
        ui->processDrawer->setImage(processSpace->getLastDisplayImage(), true);
        updateRedoUndoStatus();
    }
}

void MainWindow::onMouseNormalArrow()
{
    ui->mouseBehaviourButton->setText(tr("Normal Arrow"));
    ui->mouseBehaviourButton->setIcon(QIcon::fromTheme("edit-select"));
    ui->processDrawer->setDrawMode(QImageInteractiveDrawer::NONE);
    disconnect(ui->processDrawer, &QImageInteractiveDrawer::mouseReleased, 0, 0);
}

void MainWindow::onMouseWhitePencil()
{
    disconnect(ui->processDrawer, &QImageInteractiveDrawer::mouseReleased, 0, 0);
    ui->mouseBehaviourButton->setText(tr("White Pencil"));
    ui->mouseBehaviourButton->setIcon(QIcon::fromTheme("draw-freehand"));
    ui->processDrawer->setWhite();
    ui->processDrawer->setDrawMode(QImageInteractiveDrawer::PENCIL);
    connect(ui->processDrawer, &QImageInteractiveDrawer::mouseReleased, this, &MainWindow::onPencilDrawFinished);
}

void MainWindow::onMouseBlackPencil()
{
    disconnect(ui->processDrawer, &QImageInteractiveDrawer::mouseReleased, 0, 0);
    ui->mouseBehaviourButton->setText(tr("Black Pencil"));
    ui->mouseBehaviourButton->setIcon(QIcon::fromTheme("draw-freehand"));
    ui->processDrawer->setWhite(false);
    ui->processDrawer->setDrawMode(QImageInteractiveDrawer::PENCIL);
    connect(ui->processDrawer, &QImageInteractiveDrawer::mouseReleased, this, &MainWindow::onPencilDrawFinished);
}

void MainWindow::onMouseWhiteEraser()
{
    disconnect(ui->processDrawer, &QImageInteractiveDrawer::mouseReleased, 0, 0);
    ui->mouseBehaviourButton->setText(tr("White Eraser"));
    ui->mouseBehaviourButton->setIcon(QIcon::fromTheme("draw-eraser"));
    ui->processDrawer->setWhite();
    ui->processDrawer->setDrawMode(QImageInteractiveDrawer::ERASER);
    connect(ui->processDrawer, &QImageInteractiveDrawer::mouseReleased, this, &MainWindow::onEraserDrawFinished);
}

void MainWindow::onMouseBlackEraser()
{
    disconnect(ui->processDrawer, &QImageInteractiveDrawer::mouseReleased, 0, 0);
    ui->mouseBehaviourButton->setText(tr("Black Eraser"));
    ui->mouseBehaviourButton->setIcon(QIcon::fromTheme("draw-eraser"));
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

void MainWindow::onPencilDrawFinished(const QVector<QPointF> &pts)
{
    processSpace->newPencilWork(pts, ui->processDrawer->isWhite());
}

void MainWindow::onEraserDrawFinished(const QVector<QPointF> &pts)
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
    watershedDlg->setPenColour(ui->calibreDrawer->getPenColour());
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
    updateRedoUndoStatus();
}

void MainWindow::onProcessButtonBoxClicked(QAbstractButton *b)
{
    if (ui->processButtonBox->standardButton(b) == QDialogButtonBox::Reset) {//reset
        processSpace->reset(cgimg.croppedMatrix());
    }
    else {//save
        //check if it's eligible
        if (processSpace->getMarkerMatrix() == NULL) {
            onMessagesArrived(tr("Error. Processing is not finished yet! You must segment the image before analysis."));
            return;
        }

        ui->analysisTab->setEnabled(true);
        ui->analysisInteracter->setImage(processSpace->getLastDisplayImage());
        ui->imageTabs->setCurrentIndex(3);

        onMessagesArrived(tr("Analysing... Please Wait......"));
        //clean the previous delegate
        if (analysisDelegate != NULL) {
            analysisDelegate->disconnect();
            analysisDelegate->deleteLater();
        }
        analysisDelegate = new AnalysisItemDelegate(ui->analysisTableView);
        ui->analysisTableView->setItemDelegate(analysisDelegate);
        //setup analyser and retrive report
        if (analyser != NULL) {
            analyser->disconnect();
            analyser->deleteLater();
        }
        analyser = new CAIGA::Analyser(cgimg.getScaleValue(), processSpace->getMarkerMatrix(), processSpace->getContours(), this);
        connect(analyser, &CAIGA::Analyser::finished, this, &MainWindow::onAnalysisFinished);
        analyser->reset();
    }
    previewSpace->clear();
}

void MainWindow::onAnalysisFinished()
{
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
        ui->reportTab->setEnabled(true);
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
    ui->reportTextBrowser->clear();
    updateReportTextBrowser(ui->splitSpinBox->value());
    ui->splitOKPushButton->setEnabled(false);
}

void MainWindow::onReportAvailable(bool a)
{
    ui->actionExportReportAs->setEnabled(a);
    ui->actionQuickReportExport->setEnabled(a);
}

void MainWindow::updateReportTextBrowser(int split)
{
    onReportAvailable(false);
    if (reporter != NULL) {
        reporter->disconnect();
        reporter->deleteLater();
    }
    reporter = new Reporter(ui->diameterBarPlotter, analyser, processSpace, split, cgimg.getRawImage(), this);
    connect(reporter, &Reporter::reportAvailable, this, &MainWindow::onReportAvailable);
    connect(reporter, &Reporter::workStatusStrUpdated, this, &MainWindow::onMessagesArrived);
    connect(reporter, &Reporter::reportGenerated, ui->reportTextBrowser, &QTextBrowser::setDocument);
    reporter->generateReport();
}

void MainWindow::onCurrentTabChanged(int i)
{
    ui->calibrePixelViewer->setLivePreviewEnabled(false);
    ui->actionRedo->setEnabled(false);
    ui->actionUndo->setEnabled(false);
    ui->actionSaveCurrentImageAs->setEnabled(true);

    switch (i) {
    case 0://raw
        zoomLabel->setText(tr("Auto"));
        break;
    case 1://calibre
        ui->calibrePixelViewer->setLivePreviewEnabled(true);
        ui->actionSaveCurrentImageAs->setEnabled(false);
        onZoomUpdated(ui->calibreDrawer->getZoom());
        break;
    case 2://process
        updateRedoUndoStatus();
        onZoomUpdated(ui->processDrawer->getZoom());
        break;
    case 3://analysis
        onZoomUpdated(ui->analysisInteracter->getZoom());
        break;
    case 4://report
        ui->actionSaveCurrentImageAs->setEnabled(false);
        break;
    }
}

void MainWindow::onUndoTriggered()
{
    processSpace->undo();
    updateRedoUndoStatus();
}

void MainWindow::onRedoTriggered()
{
    processSpace->redo();
    updateRedoUndoStatus();
}

void MainWindow::onAddDiskImageTriggered()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Add Image from Disk"), QDir::currentPath(),
                       "Supported Images (*.png *.jpg *.jpeg *.tif *.tiff *.bmp)");
    if (filename.isNull()) {
        return;
    }
    setCurrentDirbyFile(filename);
    cgimg.setRawImageByFile(filename);
    onNewImageOpened();
}

void MainWindow::onAddCameraImageTriggered()
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
    onNewImageOpened();
}

void MainWindow::onResetActionTriggered()
{
    ui->calibreDrawer->reset();
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

void MainWindow::onReportExportTriggered()
{
    QString filter;
    QString filename = QFileDialog::getSaveFileName(this,
                       tr("Export Report As"),
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
            format.append("odf");
        }
        else {
            format.append("html");
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
                       "Portable Network Graphics (*.png);;Joint Photographic Experts Group (*.jpg *.jpeg);;Tagged Image File Format (*.tif *.tiff);;Bitmap Image File (*.bmp)", &filter);
    if (filename.isNull()) {
        return;
    }
    setCurrentDirbyFile(filename);
    QByteArray format;
    if (filter.contains("png")) {
        format.append("PNG");
    }
    else if (filter.contains("jpg")) {
        format.append("JPG");
    }
    else if (filter.contains("tif")) {
        format.append("TIF");
    }
    else {
        format.append("BMP");
    }
    bool ok = false;
    switch (ui->imageTabs->currentIndex()) {
    case 0://raw
        ok = ui->rawViewer->pixmap()->save(filename, format.data());
        break;
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

void MainWindow::updateOptions(int toolbarStyle, int tabPos, const QString &colour, int undoSteps)
{
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
    ui->calibreDrawer->setPenColour(colour);
    m_undoSteps = undoSteps;
    if (processSpace != NULL) {
        processSpace->setUndoSteps(m_undoSteps);
    }
    if (previewSpace != NULL) {
        previewSpace->setUndoSteps(m_undoSteps);
    }
}

void MainWindow::readConfig()
{
    emit configReadFinished(settings.value("Toolbar Style").toInt(),
                            settings.value("Tab Position", 1).toInt(),
                            settings.value("Pen Colour", "#F00").toString(),
                            settings.value("Undo Steps", 10).toInt());
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

void MainWindow::onNewImageOpened()
{
    //setup calibreSpace
    if (calibreSpace != NULL) {
        calibreSpace->disconnect();
        calibreSpace->deleteLater();
    }
    calibreSpace = new CAIGA::CalibreSpace(&cgimg, this);
    connect(ui->scaleDoubleSpinBox, static_cast<void (QDoubleSpinBox::*) (double)>(&QDoubleSpinBox::valueChanged), calibreSpace, static_cast<void (CAIGA::CalibreSpace::*) (qreal)>(&CAIGA::CalibreSpace::setScaleValue));
    ui->calibreDrawer->setSpace(calibreSpace);

    //clean up process, analysis and report
    ui->reportTextBrowser->clear();
    ui->diameterBarPlotter->clearPlottables();
    ui->diameterBarPlotter->replot(QCustomPlot::rpImmediate);
    ui->analysisInteracter->setImage(QImage());
    ui->analysisTableView->setModel(NULL);
    ui->analysisCurrentClassLabel->clear();
    ui->processDrawer->setImage(QImage());

    //setup tabs and some actions
    ui->rawViewer->setPixmap(cgimg.getRawPixmap());
    ui->calibreDrawer->setImage(cgimg.getRawImage(), true);
    onReportAvailable(false);
    ui->imageTabs->setEnabled(true);
    ui->processTab->setEnabled(false);
    ui->analysisTab->setEnabled(false);
    ui->reportTab->setEnabled(false);
    ui->imageTabs->setCurrentIndex(0);
    ui->actionSaveCurrentImageAs->setEnabled(true);
}

void MainWindow::updateRedoUndoStatus()
{
    if (ui->processTab->isEnabled()) {//prevent from crashes when processTab isn't ready (i.e. user click the processTab before clicking save in calibre)
        ui->actionRedo->setEnabled(processSpace->countUndone() > 0);
        ui->actionUndo->setEnabled(processSpace->count() > 1);
    }
}

void MainWindow::onMessagesArrived(const QString &str)
{
    ui->statusBar->showMessage(str);
}

void MainWindow::onZoomUpdated(qreal z)
{
    zoomLabel->setText(QString::number(qFloor(z * 100)) + "%");
}
