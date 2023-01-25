//
// Created by user on 23.01.2023.
//

#include "TreeItem.h"

TreeItem::TreeItem (const QVector<QVariant> &data, TreeItem *parent):
    m_parentItem(parent)
    ,m_columnData(data)
{
}

TreeItem::~TreeItem() { qDeleteAll(m_childItems); }

void TreeItem::appendChild(TreeItem *item) {
    m_childItems.append(item);
}

TreeItem *TreeItem::child (int row) {
    return m_childItems.value(row);
}

int TreeItem::childCount() const {
    return m_childItems.count();
}

int TreeItem::columnCount() const {
    return m_columnData.count();
    //Количество столбцов в узле = длине списка данных узла
}

QVariant TreeItem::data (int column) const {
    return m_columnData.value(column);
}

TreeItem *TreeItem::parentItem() {
    return m_parentItem;
}

void TreeItem::setParentItem(TreeItem* parent) {
    m_parentItem = parent;
}

int TreeItem::childNumber() const {
    if (m_parentItem) return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));
    return 0;
}

bool TreeItem::insertChildren(int position, int count, int columns) {
    if (position < 0 || position > m_childItems.size()) return false;
    for (int row = 0; row < count; ++row) {
        QVector<QVariant> data(columns);
        auto *item = new TreeItem(data, this);
        m_childItems.insert(position, item);
    }
    return true;
}

bool TreeItem::insertChildren(int position, TreeItem* item) {
    if (position < 0 || position > m_childItems.size()) return false;
    m_childItems.insert(position, item);
    return true;
}

bool TreeItem::insertColumns(int position, int columns) {
    if (position < 0 || position > m_columnData.size()) return false;
    for (int column = 0; column < columns; ++column) m_columnData.insert(position, QVariant());
            foreach (TreeItem *child, m_childItems) child->insertColumns(position, columns);
    return true;
}

bool TreeItem::removeChildren(int position, int count) {
    if (position < 0 || position + count > m_childItems.size()) return false;
    for (int row = 0; row < count; ++row) delete m_childItems.takeAt(position);
    return true;
}

TreeItem* TreeItem::removeChildren(int position) {
    if (position < 0 || position > m_childItems.size()) return nullptr;
    auto* retVal = m_childItems.takeAt(position);
    return retVal;
}

bool TreeItem::moveChildren(int position, TreeItem* newParent) {
    if (position < 0 || position > m_childItems.size()) return false;
    auto* movingChild = m_childItems.takeAt(position);
    newParent->appendChild(movingChild);
    movingChild->setParentItem(newParent);
    return true;
}

bool TreeItem::removeColumns(int position, int columns) {
    if (position < 0 || position + columns > m_columnData.size()) return false;
    for (int column = 0; column < columns; ++column) m_columnData.removeAt(position);
            foreach (TreeItem *child, m_childItems) child->removeColumns(position, columns);
    return true;
}

bool TreeItem::setData(int column, const QVariant &value) {
    if (column < 0 || column >= m_columnData.size()) return false;
    m_columnData[column] = value;
    return true;
}