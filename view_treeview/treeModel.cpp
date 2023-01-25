//
// Created by user on 23.01.2023.
//

#include <QtCore/QDataStream>
#include "treeModel.h"
#include "QDebug"
#include "QIcon"

TreeModel::TreeModel(const QStringList &headers, QObject *parent)
    : QAbstractItemModel(parent)
{
    QVector<QVariant> rootData;
    for(const auto& header: headers)
        rootData << header;
    rootItem = new TreeItem(rootData);
    setupModelData(rootItem);
}

TreeModel::~TreeModel() { delete rootItem; }

int TreeModel::columnCount(const QModelIndex &parent) const {
    return rootItem->columnCount();
}

QVariant TreeModel::data (const QModelIndex &index, int role) const {
    if (!index.isValid()) return QVariant();
    TreeItem *item = getItem(index);
    switch (role) {
        case Qt::DisplayRole:
            getDisplayRole(item, index);
            break;
        case Qt::BackgroundRole:
            getBackgroundRole(item,index);
            break;
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
            break;
        case Qt::DecorationRole:
            getDecorationRole(item, index);
            break;
    }
    return QVariant();
}

QVariant TreeModel::getBackgroundRole(TreeItem* item, const QModelIndex &index) const{
    return item->getBrush();
}

QVariant TreeModel::getDisplayRole(TreeItem* item, const QModelIndex &index) const{
    return item->data(index.column());
}

QVariant TreeModel::getDecorationRole(TreeItem* item, const QModelIndex &index) const{
    return item->getColor();
    return QVariant();
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if(role != Qt::EditRole) return false;
    TreeItem *item = getItem(index);
    bool result = item->setData(index.column(), value);
    if (result) {
        emit dataChanged(index, index);
    }
    return result;
}

TreeItem* TreeModel::getItem(const QModelIndex &index) const {
    if (index.isValid()){
        auto *item = static_cast<TreeItem*>(index.internalPointer());
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
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) return rootItem->data(section);
    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const {
    if (parent.isValid() && parent.column() != 0) return QModelIndex();
    TreeItem *parentItem = getItem(parent);
    TreeItem *childItem = parentItem->child(row);
    if (childItem) return createIndex(row, column, childItem);
    else return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const {
    if (!index.isValid()) return QModelIndex();
    TreeItem *childItem = getItem(index);
    TreeItem *parentItem = childItem->parentItem();
    if (parentItem == rootItem) return QModelIndex();
    return createIndex(parentItem->childNumber(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const {
    TreeItem *parentItem = getItem(parent);
    return parentItem->childCount();
}

void TreeModel::setupModelData(const QStringList &lines, TreeItem *parent) {
//    QList<TreeItem*> parents;
//    QList<int> indentations;
//    parents << parent;
//    indentations << 0;
//    int number = 0;
//    //Ищем первый непробельный символ с номером position
//    while (number < lines.count()) {
//        int position = 0;
//        while (position < lines[number].length()) {
//            if (lines[number].at(position) != ' ') break;
//            position++;
//        }
//        //Отрезаем пробельное начало строки
//        QString lineData = lines[number].mid(position).trimmed();
//        if (!lineData.isEmpty()) {
//            //Читаем остальную часть строки, если она есть
//            QStringList columnStrings = lineData.split(":::", Qt::SplitBehaviorFlags::SkipEmptyParts);
//            //Учитываем разделитель столбцов
//            QVector <QVariant> columnData; //Список данных столбцов
//            for (int column = 0; column < columnStrings.count(); ++column)
//                columnData << columnStrings[column];
//            if (position > indentations.last()) {
//                //Последний потомок текущего родителя теперь будет новым родителем,
//                //пока у текущего родителя нет потомков
//                if (parents.last()->childCount() > 0) {
//                    parents << parents.last()->child(parents.last()->childCount()-1);
//                    indentations << position;
//                }
//            }
//            else {
//                while (position < indentations.last() && parents.count() > 0) {
//                    parents.pop_back();
//                    indentations.pop_back();
//                }
//            }
//            //Добавить новый узел в список потомков текущего родителя
//            parents.last()->appendChild(new TreeItem(columnData, parents.last()));
//        }
//        ++number;
//    }
}

void TreeModel::setupModelData(TreeItem *parent) {
    QVector <QVariant> columnData;
    columnData << "PWM";
    auto* newItem = new TreeItem(columnData, parent);
    parent->appendChild(newItem);
    columnData.clear();
    columnData << "RPM";
    newItem->appendChild(new TreeItem(columnData, newItem));
    columnData.clear();
    columnData << "IDC";
    rootItem->appendChild(new TreeItem(columnData, rootItem));
    columnData.clear();
    columnData << "ECT";
    newItem->appendChild(new TreeItem(columnData, newItem));
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
    TreeItem *parentItem = getItem(parent);
    bool success;
    beginInsertRows(parent, position, position + rows - 1);
    success = parentItem->insertChildren(position, rows, rootItem->columnCount());
    endInsertRows();
    return success;
}

bool TreeModel::removeRows(int position, int rows, const QModelIndex &parent) {
    TreeItem *parentItem = getItem(parent);
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
    bool success = oldParentItem->moveChildren(oldRow, newParentItem);
    endMoveRows();
    return success;
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
bool TreeModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    if(data->data("hasChild") == "true")
        return false;
    TreeItem* item = getItem(parent);
    return item->parentItem() == rootItem;
}
