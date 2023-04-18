//
// Created by user on 23.01.2023.
//

#ifndef PROTOSLOGVIEWER_TREEMODEL_H
#define PROTOSLOGVIEWER_TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include "QMimeData"
#include "QVariantMap"
#include "QBrush"
#include "TreeItem.h"
#include "LogItem/LogItem.h"

class TreeModel : public QAbstractItemModel {
    Q_OBJECT
public:
    TreeModel(LogItem* rootItem, QObject *parent = nullptr);
    ~TreeModel();
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;
    bool insertColumns(int position, int columns,const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int position, int columns, const QModelIndex &parent = QModelIndex()) override;
    bool insertRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;
    void beginResetMe();
    void endResetMe();
    void reFresh();
    QVector<LogItem*> getSelections(const QModelIndexList& indexes);
    LogItem *getItem(const QModelIndex &index) const;
    //Drag&Drop
    Qt::DropActions supportedDropActions() const override;
    Qt::DropActions supportedDragActions() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
    bool canFinishDrop(const QModelIndex &parent) const;
    bool moveRowDD(int newPosition, const QModelIndex &newParentIdx);
signals:
    void ItemMoved(LogItem* who, LogItem* where, LogItem* from);
private:
    static inline QVariantMap movingData = {};
    LogItem *rootItem;
};
#endif //PROTOSLOGVIEWER_TREEMODEL_H