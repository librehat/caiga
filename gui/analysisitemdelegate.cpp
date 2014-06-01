#include <QComboBox>
#include <QDebug>
#include "analysisitemdelegate.h"

AnalysisItemDelegate::AnalysisItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
    classesList = NULL;
}

QWidget *AnalysisItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //do not allow edition except in column 3 (Class), starting from 0
    if(index.column() != 2) {
        return QAbstractItemDelegate::createEditor(parent, option, index);
    }

    //create the combobox and populate it
    QComboBox *cb = new QComboBox(parent);
    cb->setEditable(true);
    if (classesList == NULL) {
        qWarning() << "Error. Classes List cannot be NULL.";
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
            currentText = cb->currentText();//in case of caseSensitive checking in classesList
        }
        // otherwise, create a new item
        else {
            cb->addItem(currentText);
            int newIndex = cb->findText(currentText, Qt::MatchFixedString);
            cb->setCurrentIndex(newIndex);
        }
        emit classChanged(index, currentText);
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
