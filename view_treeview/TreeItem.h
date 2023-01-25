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

class TreeItem {
public:
    explicit TreeItem (const QVector<QVariant> &data, TreeItem *parentItem = 0);
    ~TreeItem();

    void appendChild(TreeItem *child);
    TreeItem *child(int row);
    QVariant data(int column) const;

    virtual bool setData(int column, const QVariant &value);

    int childCount() const;
    int columnCount() const;
    int childNumber() const;
    TreeItem *parentItem();
    void setParentItem(TreeItem *parent);

    bool insertChildren(int position, int count, int columns);
    bool insertChildren(int position, TreeItem* item);
    bool insertColumns(int position, int columns);
    bool removeChildren(int position, int count);
    TreeItem* removeChildren(int position);
    bool moveChildren(int position, TreeItem *newParent);
    bool removeColumns(int position, int columns);

protected:
    QList<TreeItem*> m_childItems;
    QVector<QVariant> m_columnData;
    TreeItem *m_parentItem;
};

#endif //PROTOSLOGVIEWER_TREEITEM_H
