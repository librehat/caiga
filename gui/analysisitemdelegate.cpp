#include <QComboBox>
#include "analysisitemdelegate.h"

AnalysisItemDelegate::AnalysisItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
    classesList = NULL;
}

QWidget *AnalysisItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //do not allow edition except in column 2 (Class), starting from 0
    if(index.column() != 1) {
        return QAbstractItemDelegate::createEditor(parent, option, index);
    }

    //create the combobox and populate it
    QComboBox *cb = new QComboBox(parent);
    cb->setEditable(true);
    if (classesList == NULL) {
        cb->addItem(QString("Base"));
    }
    else {
        cb->addItems(*classesList);
    }
    return cb;
}

void AnalysisItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(QComboBox *cb = qobject_cast<QComboBox *>(editor)) {
        // get the index of the text in the combobox that matches the current value of the itenm
        QString currentText = index.data(Qt::DisplayRole).toString();
        int cbIndex = cb->findText(currentText, Qt::MatchFixedString);
        // if it is valid, adjust the combobox
        if(cbIndex >= 0) {
            cb->setCurrentIndex(cbIndex);
            emit classChanged(index, cbIndex);
        }
        // otherwise, create a new item
        else {
            classesList->append(currentText);
            cb->addItem(currentText);
            int newIndex = cb->findText(currentText);
            cb->setCurrentIndex(newIndex);
            emit classChanged(index, newIndex);
        }
    } else {
        QAbstractItemDelegate::setEditorData(editor, index);
    }
}

void AnalysisItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(QComboBox *cb = qobject_cast<QComboBox *>(editor)) {
        // save the current text of the combo box as the current value of the item
        model->setData(index, cb->currentText(), Qt::DisplayRole);
    }
    else {
        QAbstractItemDelegate::setModelData(editor, model, index);
    }
}
