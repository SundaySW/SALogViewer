//
// Created by user on 23.01.2023.
//
#ifndef PROTOSLOGVIEWER_TREEITEM_H
#define PROTOSLOGVIEWER_TREEITEM_H

#include <QList>
#include <QVector>
#include <QVariant>
#include <QBrush>
#include "QColor"

template <typename T>
class TreeItem {
public:
    explicit TreeItem(const QVector<QVariant>&data, T *parentItem = 0);
    ~TreeItem();
    void appendChild(T *child);
    T *child(int row);
    virtual QVariant data(int column, int role) const;
    virtual bool setData(int column, const QVariant &value);
    int childCount() const;
    int columnCount() const;
    int childNumber();
    T* parentItem();
    void setParentItem(T *parent);
    bool insertChildren(int position, int count, int columns);
    bool insertChildren(int position, T* item);
    bool insertColumns(int position, int columns);
    bool removeChildren(int position, int count);
    void removeChildren(int position);
    T* moveChildren(int position, T *newParent);
    bool removeColumns(int position, int columns);
    void removeAllChildren();
    const QList<T*>& getMChildItems() const;
protected:
    QList<T*> m_childItems;
    QVector<QVariant> m_columnData;
    T* m_parentItem = nullptr;
};

#endif //PROTOSLOGVIEWER_TREEITEM_H
