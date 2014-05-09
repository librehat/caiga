#include "watershedmarkerdialog.h"
#include "ui_watershedmarkerdialog.h"

WatershedMarkerDialog::WatershedMarkerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WatershedMarkerDialog)
{
    ui->setupUi(this);
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

    m_originalMat = new cv::Mat();
}

WatershedMarkerDialog::~WatershedMarkerDialog()
{
    delete ui;
    delete m_originalMat;
}

void WatershedMarkerDialog::setPenColour(const QColor &c)
{
    m_colour = c;
    ui->drawer->setPenColour(m_colour);
}

void WatershedMarkerDialog::setOrignialMat(cv::Mat *src)
{
    delete m_originalMat;
    m_originalMat = new cv::Mat(src->clone());
    if (m_originalMat->type() == CV_8UC1) {
        cv::cvtColor(*src, *m_originalMat, CV_GRAY2RGB);
    }
    viewSpace.reset(m_originalMat);
    pts.clear();
    ptsVec.clear();
    QSize s = viewSpace.getLatestQImg().size();
    ui->drawer->setMinimumSize(s);
    ui->drawer->setMaximumSize(s);
}

void WatershedMarkerDialog::onMarkFinished(const QVector<QPoint> &pv)
{
    pts = pv;
    ptsVec.append(pv);
    viewSpace.newPencilWork(pv, m_colour);
}

void WatershedMarkerDialog::onResetButtonClicked()
{
    viewSpace.reset();
    pts.clear();
    ptsVec.clear();
}

void WatershedMarkerDialog::onUndoButtonClicked()
{
    if (ptsVec.size() > 0) {
        viewSpace.undo();
        ptsVecUndone.append(ptsVec.last());
        ptsVec.pop_back();
        if(ptsVec.isEmpty()) {
            pts.clear();
        }
        else {
            pts = ptsVec.last();
        }
    }
}

void WatershedMarkerDialog::onRedoButtonClicked()
{
    if (!ptsVecUndone.isEmpty()) {
        viewSpace.redo();
        ptsVec.append(ptsVecUndone.last());
        ptsVecUndone.pop_back();
        pts = ptsVec.last();
    }
}

void WatershedMarkerDialog::onPreviewButtonClicked()
{
    emit previewTriggled(ptsVec);
}

void WatershedMarkerDialog::updateMarkers()
{
    ui->drawer->setImage(viewSpace.getLatestQImg());
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

void WatershedMarkerDialog::handleAccepted()
{
    emit accepted(ptsVec);
}
