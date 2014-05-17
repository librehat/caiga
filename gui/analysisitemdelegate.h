#ifndef ANALYSISITEMDELEGATE_H
#define ANALYSISITEMDELEGATE_H

#include <QStyledItemDelegate>

class AnalysisItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    AnalysisItemDelegate(QObject *parent = 0);
    ~AnalysisItemDelegate() {}

    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    inline void setClassesList(QStringList *list) { classesList = list; }

signals:
    void classChanged(const QModelIndex&, int) const;

private:
    QStringList *classesList;
};

#endif // ANALYSISITEMDELEGATE_H
