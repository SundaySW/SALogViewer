//
// Created by 79162 on 15.01.2023.
//

#include "ItemModel.h"

#define MAX_COLUMNS 1

ItemModel::ItemModel(QList<LogItem> *inListPtr, QObject* parent):
    itemListPtr(inListPtr),
    QAbstractItemModel(parent)
{
}

QModelIndex ItemModel::parent(const QModelIndex &child) const {
    if (!child.isValid())
        return QModelIndex();

    auto* childItem = static_cast<LogItem*>(child.internalPointer());
    LogItem* parentItem = childItem->parent;
    if (parentItem != nullptr)
        return createIndex(findRow(parentItem), 0, parentItem);
    else
        return QModelIndex();
}

int ItemModel::findRow(const LogItem *logItemData) const
{
//    const auto& parentInfoChildren = logItemData->parent != nullptr ? logItemData->parent->children: *itemListPtr;
//    auto position = std::find(parentInfoChildren.begin(), parentInfoChildren.end(), *logItemData);
//    int retval = std::distance(parentInfoChildren.begin(), position);
//    return retval;
}

int ItemModel::rowCount(const QModelIndex& parent) const {
    if (!parent.isValid())
        return itemListPtr->size();

    auto* parentInfo = static_cast<const LogItem*>(parent.internalPointer());
    return parentInfo->children.size();
}

int ItemModel::columnCount(const QModelIndex &parent) const{
    return MAX_COLUMNS;
}

QVariant ItemModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();
    const auto* logItemData = static_cast<LogItem*>(index.internalPointer());

    switch (role) {
        case Qt::DisplayRole:
            return logItemData->getName();
        default:
            return QVariant();
    }
}

QVariant ItemModel::headerData(int section, Qt::Orientation orientation, int role) const {
    QVariant retVal;
    if (role == Qt::DisplayRole){
        switch (section) {
            case 0: retVal = "Name"; break;
            default: retVal = "Unknown"; break;
        }
    }
    return retVal;
}

bool ItemModel::hasChildren(const QModelIndex &parent) const{
    if (parent.isValid()) {
        const auto* parentInfo = static_cast<const LogItem*>(parent.internalPointer());
        Q_ASSERT(parentInfo != nullptr);
        if (!parentInfo->mapped)
            return true;
    }
    return QAbstractItemModel::hasChildren(parent);
}

QModelIndex ItemModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    if (!parent.isValid())
        return createIndex(row, column, &itemListPtr[row]);

    auto* parentInfo = static_cast<LogItem*>(parent.internalPointer());
    return createIndex(row, column, &parentInfo->children[row]);
}