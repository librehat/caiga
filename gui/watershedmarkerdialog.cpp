#include "watershedmarkerdialog.h"
#include "ui_watershedmarkerdialog.h"

#ifdef _WIN32
#include <QtWinExtras>
#endif

WatershedMarkerDialog::WatershedMarkerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WatershedMarkerDialog)
{
    ui->setupUi(this);

#ifdef _WIN32
    QtWin::enableBlurBehindWindow(this);
    QtWin::extendFrameIntoClientArea(this, -1, -1, -1, -1);
    this->setAttribute(Qt::WA_TranslucentBackground);
#endif

    this->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
    m_colour = QColor(255, 255, 255);
    ui->drawer->setDrawMode(QImageInteractiveDrawer::PENCIL);

    connect(ui->previewButton, &QPushButton::clicked, this, &WatershedMarkerDialog::onPreviewButtonClicked);
    connect(ui->undoButton, &QPushButton::clicked, this, &WatershedMarkerDialog::onUndoButtonClicked);
    connect(ui->redoButton, &QPushButton::clicked, this, &WatershedMarkerDialog::onRedoButtonClicked);
    connect(ui->resetButton, &QPushButton::clicked, this, &WatershedMarkerDialog::onResetButtonClicked);
    connect(ui->drawer, &QImageInteractiveDrawer::mouseReleased, this, &WatershedMarkerDialog::onMarkFinished);
    connect(&viewSpace, &CAIGA::WorkSpace::workStarted, this, &WatershedMarkerDialog::onWorkStarted);
    connect(&viewSpace, &CAIGA::WorkSpace::workFinished, this, &WatershedMarkerDialog::onWorkFinished);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &WatershedMarkerDialog::handleAccepted);
    connect(ui->autoButton, &QPushButton::clicked, this, &WatershedMarkerDialog::onAutoClicked);

    m_originalMat = NULL;
}

WatershedMarkerDialog::~WatershedMarkerDialog()
{
    delete ui;
    if (m_originalMat != NULL) delete m_originalMat;
}

void WatershedMarkerDialog::setPenColour(const QColor &c)
{
    m_colour = c;
    ui->drawer->setPenColour(m_colour);
}

void WatershedMarkerDialog::setOrignialMat(cv::Mat *src)
{
    if (m_originalMat != NULL) delete m_originalMat;
    m_originalMat = new cv::Mat(src->clone());
    if (m_originalMat->type() == CV_8UC1) {
        cv::cvtColor(*src, *m_originalMat, CV_GRAY2RGB);
    }
    viewSpace.reset(m_originalMat);
    pts.clear();
    ptsVec.clear();
    QSize s = viewSpace.getLastDisplayImage().size();
    ui->drawer->setMinimumSize(s);
    ui->drawer->setMaximumSize(s);
}

void WatershedMarkerDialog::onAutoClicked()
{
    //TODO: draw automatically generated seeds on screen
    emit autoMarked();
}

void WatershedMarkerDialog::onMarkFinished(const QVector<QPoint> &pv)
{
    pts = pv;
    ptsVec.append(pv);
    viewSpace.newPencilWork(pv, m_colour);
    ui->resetButton->setEnabled(true);
    ui->undoButton->setEnabled(true);
    ui->previewButton->setEnabled(true);
}

void WatershedMarkerDialog::onResetButtonClicked()
{
    viewSpace.reset();
    pts.clear();
    ptsVec.clear();
    ptsVecUndone.clear();
    ui->undoButton->setEnabled(false);
    ui->previewButton->setEnabled(false);
    ui->redoButton->setEnabled(false);
    ui->resetButton->setEnabled(false);
    emit reseted();
}

void WatershedMarkerDialog::onUndoButtonClicked()
{
    if (ptsVec.size() > 0) {
        viewSpace.undo();
        ptsVecUndone.append(ptsVec.last());
        ptsVec.pop_back();
        ui->redoButton->setEnabled(true);
        if(ptsVec.isEmpty()) {
            pts.clear();
            ui->undoButton->setEnabled(false);
            ui->previewButton->setEnabled(false);
        }
        else {
            pts = ptsVec.last();
        }
        ui->resetButton->setEnabled(true);
    }
}

void WatershedMarkerDialog::onRedoButtonClicked()
{
    if (!ptsVecUndone.isEmpty()) {
        viewSpace.redo();
        ptsVec.append(ptsVecUndone.last());
        ptsVecUndone.pop_back();
        pts = ptsVec.last();
        if (ptsVecUndone.isEmpty()) {
            ui->redoButton->setEnabled(false);
        }
        ui->resetButton->setEnabled(true);
        ui->undoButton->setEnabled(true);
        ui->previewButton->setEnabled(true);
    }
}

void WatershedMarkerDialog::updateMarkers()
{
    ui->drawer->setImage(viewSpace.getLastDisplayImage());
}

void WatershedMarkerDialog::onPreviewStarted()
{
    ui->buttonBox->setEnabled(false);
    ui->previewButton->setEnabled(false);
}

void WatershedMarkerDialog::onPreviewFinished()
{
    ui->buttonBox->setEnabled(true);
    ui->previewButton->setEnabled(true);
}

void WatershedMarkerDialog::onWorkStarted()
{
    ui->buttonBox->setEnabled(false);
}

void WatershedMarkerDialog::onWorkFinished()
{
    ui->buttonBox->setEnabled(true);
    this->updateMarkers();
}
