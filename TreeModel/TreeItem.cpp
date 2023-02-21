//
// Created by user on 23.01.2023.
//

#include "TreeItem.h"
#include "LogItem/LogItem.h"

template <typename T>
TreeItem<T>::TreeItem (const QVector<QVariant> &data, T *parent):
    m_parentItem(parent)
    ,m_columnData(data)
{
}

template <typename T>
TreeItem<T>::~TreeItem() {
    qDeleteAll(m_childItems);
}

template <typename T>
void TreeItem<T>::appendChild(T *item) {
    m_childItems.append(item);
}

template <typename T>
T* TreeItem<T>::child (int row) {
    return m_childItems.value(row);
}

template <typename T>
int TreeItem<T>::childCount() const {
    return m_childItems.count();
}

template <typename T>
int TreeItem<T>::columnCount() const {
    return m_columnData.count();
}

template <typename T>
T* TreeItem<T>::parentItem() {
    return m_parentItem;
}

template <typename T>
void TreeItem<T>::setParentItem(T* parent) {
    m_parentItem = parent;
}

template <typename T>
int TreeItem<T>::childNumber() {
    if(m_parentItem) return m_parentItem->m_childItems.indexOf(static_cast<T*>(this));
    return 0;
}

template <typename T>
bool TreeItem<T>::insertChildren(int position, int count, int columns) {
    if (position < 0 || position > m_childItems.size()) return false;
    for (int row = 0; row < count; ++row) {
        QVector<QVariant> newItemData(columns);
        auto *item = new T(newItemData, static_cast<T*>(this));
        m_childItems.insert(position, item);
    }
    return true;
}

template <typename T>
bool TreeItem<T>::insertChildren(int position, T* item) {
    if (position < 0 || position > m_childItems.size()) return false;
    m_childItems.insert(position, item);
    return true;
}

template <typename T>
bool TreeItem<T>::insertColumns(int position, int columns) {
    if (position < 0 || position > m_columnData.size()) return false;
    for (int column = 0; column < columns; ++column) m_columnData.insert(position, QVariant());
            foreach (T *child, m_childItems) child->insertColumns(position, columns);
    return true;
}

template <typename T>
bool TreeItem<T>::removeChildren(int position, int count) {
    if (position < 0 || position + count > m_childItems.size()) return false;
    for (int row = 0; row < count; ++row) delete m_childItems.takeAt(position);
    return true;
}

template <typename T>
void TreeItem<T>::removeChildren(int position) {
    if (position < 0 || position > m_childItems.size()) return;
    delete m_childItems.takeAt(position);
}

template <typename T>
T* TreeItem<T>::moveChildren(int position, T* newParent) {
    if (position < 0 || position > m_childItems.size()) return nullptr;
    auto* movingChild = m_childItems.takeAt(position);
    newParent->appendChild(movingChild);
    movingChild->setParentItem(newParent);
    return movingChild;
}

template <typename T>
bool TreeItem<T>::removeColumns(int position, int columns) {
    if (position < 0 || position + columns > m_columnData.size()) return false;
    for (int column = 0; column < columns; ++column) m_columnData.removeAt(position);
            foreach (TreeItem *child, m_childItems) child->removeColumns(position, columns);
    return true;
}

template <typename T>
bool TreeItem<T>::setData(int column, const QVariant &value) {
    if (column < 0 || column >= m_columnData.size()) return false;
    m_columnData[column] = value;
    return true;
}

template <typename T>
QVariant TreeItem<T>::data(int column, int role) const{
    if(role == Qt::DisplayRole)
        return m_columnData.value(column);
    return QVariant();
}

template<typename T>
const QList<T*>& TreeItem<T>::getMChildItems() const{
    return m_childItems;
}

template<typename T>
void TreeItem<T>::removeAllChildren() {
    while(childCount())
        delete m_childItems.takeAt(0);
}

template class TreeItem<LogItem>;