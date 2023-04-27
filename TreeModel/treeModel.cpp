//
// Created by user on 23.01.2023.
//

#include <QtCore/QDataStream>
#include <QtCore/QItemSelectionModel>
#include "treeModel.h"
#include "QDebug"

TreeModel::TreeModel(LogItem* _rootItem, QObject *parent)
    : QAbstractItemModel(parent)
    ,rootItem(_rootItem)
{
}

TreeModel::~TreeModel() { delete rootItem; }

int TreeModel::columnCount(const QModelIndex &parent) const {
    return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return QVariant();
    LogItem *item = getItem(index);
    return item->data(index.column(), role);
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if(role != Qt::EditRole) return false;
    LogItem *item = getItem(index);
    bool result = item->setData(index.column(), value);
    if (result) {
        emit dataChanged(index, index);
    }
    return result;
}

LogItem* TreeModel::getItem(const QModelIndex &index) const {
    if (index.isValid()){
        auto *item = static_cast<LogItem*>(index.internalPointer());
        if(item) return item;
    }
    return rootItem;
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) return Qt::NoItemFlags;
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);
    if (index.isValid())
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEditable | defaultFlags;
    else
        return Qt::ItemIsDropEnabled | defaultFlags;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) return rootItem->data(section, Qt::DisplayRole);
    if (role == Qt::BackgroundRole) return QBrush(QColor(LogVieverColor_dark));
    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const {
    if (parent.isValid() && parent.column() != 0) return QModelIndex();
    LogItem *parentItem = getItem(parent);
    LogItem *childItem = parentItem->child(row);
    if (childItem) return createIndex(row, column, childItem);
    else return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const {
    if (!index.isValid()) return QModelIndex();
    LogItem *childItem = getItem(index);
    LogItem *parentItem = childItem->parentItem();
    if (parentItem == rootItem) return QModelIndex();
    return createIndex(parentItem->childNumber(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const {
    LogItem *parentItem = getItem(parent);
    return parentItem->childCount();
}

bool TreeModel::setHeaderData(int section, Qt::Orientation orientation,
                              const QVariant &value, int role) {
    if (role != Qt::EditRole || orientation != Qt::Horizontal) return false;
    bool result = rootItem->setData(section, value);
    if (result) {
        emit headerDataChanged(orientation, section, section);
    }
    return result;
}

bool TreeModel::insertColumns(int position, int columns, const QModelIndex &parent) {
    bool success;
    beginInsertColumns(parent, position, position + columns - 1);
    success = rootItem->insertColumns(position, columns);
    endInsertColumns();
    return success;
}

bool TreeModel::removeColumns(int position, int columns, const QModelIndex &parent) {
    bool success;
    beginRemoveColumns(parent, position, position + columns - 1);
    success = rootItem->removeColumns(position, columns);
    endRemoveColumns();
    if (rootItem->columnCount() == 0) removeRows(0, rowCount());
    return success;
}

bool TreeModel::insertRows(int position, int rows, const QModelIndex &parent) {
    LogItem*parentItem = getItem(parent);
    bool success;
    beginInsertRows(parent, position, position + rows - 1);
    success = parentItem->insertChildren(position, rows, rootItem->columnCount());
    endInsertRows();
    return success;
}

bool TreeModel::removeRows(int position, int rows, const QModelIndex &parent) {
    LogItem *parentItem = getItem(parent);
    bool success = true;
    beginRemoveRows(parent, position, position + rows - 1);
    success = parentItem->removeChildren(position, rows);
    endRemoveRows();
    return success;
}

Qt::DropActions TreeModel::supportedDropActions() const
{
    return Qt::MoveAction | Qt::LinkAction;
//    return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction | Qt::ActionMask | Qt::TargetMoveAction;
}

Qt::DropActions TreeModel::supportedDragActions() const
{
    return Qt::MoveAction | Qt::LinkAction;
//    return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction | Qt::ActionMask | Qt::TargetMoveAction;
}

bool TreeModel::moveRowDD(int newPosition, const QModelIndex &newParentIdx)
{
    int oldRow = movingData.take("row").toInt();
    QModelIndex oldParentIdx = movingData.take("parentIdx").toModelIndex();
    auto* newParentItem = getItem(newParentIdx);
    auto* oldParentItem = getItem(oldParentIdx)->parentItem();
    beginMoveRows(oldParentIdx, oldRow, oldRow, newParentIdx, newPosition);
    auto* movingChild = oldParentItem->moveChildren(oldRow, newParentItem);
    endMoveRows();
    if(movingChild != nullptr)
        emit ItemMoved(movingChild, newParentItem, oldParentItem);
    return true;
}

QMimeData* TreeModel::mimeData(const QModelIndexList &indexes) const
{
    if (indexes.count() <= 0)
        return nullptr;
    auto* MimeData = QAbstractItemModel::mimeData(indexes);
    auto& idx = indexes.at(0);
    movingData.clear();
    movingData.insert("parentIdx", idx);
    movingData.insert("row", idx.row());
    if(getItem(idx)->childCount()) MimeData->setData("hasChild", "true");
    return MimeData;
}

bool TreeModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent){
    if(!canDropMimeData(data, action, row, column, parent))
        return false;
    switch (action) {
        case Qt::MoveAction:
            moveRowDD(row, parent);
            return true;
        case Qt::IgnoreAction:
        case Qt::CopyAction:
        case Qt::LinkAction:
            moveRowDD(rowCount(), QModelIndex());
            return true;
        case Qt::ActionMask:
        case Qt::TargetMoveAction:
            break;
    }
    return false;
}

bool TreeModel::canFinishDrop(const QModelIndex &parent) const{
    return getItem(parent)->isActive();
}

bool TreeModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    if(data->data("hasChild") == "true")
        return false;
    LogItem* item = getItem(parent);
    return item->parentItem() == rootItem;
}

QVector<LogItem*> TreeModel::getSelections(const QModelIndexList& indexes){
    QVector<LogItem*> retVal;
    retVal.reserve(indexes.size());
    for(const auto& idx: indexes)
        retVal.append(getItem(idx));
    return retVal;
}

void TreeModel::reFresh(){
    emit beginResetModel();
    emit endResetModel();
}

void TreeModel::beginResetMe() {
    emit beginResetModel();
}

void TreeModel::endResetMe() {
    emit endResetModel();
}