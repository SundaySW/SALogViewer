//
// Created by 79162 on 15.01.2023.
//

#ifndef PROTOSLOGVIEWER_ITEMMODEL_H
#define PROTOSLOGVIEWER_ITEMMODEL_H


#include <QtCore/QAbstractItemModel>
#include <LogItem/LogItem.h>

class ItemModel: public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit ItemModel(QList<LogItem>* inListPtr, QObject* parent = nullptr);
    QModelIndex index(int row, int column, const QModelIndex &parent) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &child) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex&) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex& parent) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    bool hasChildren(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
private:
    QList<LogItem>* itemListPtr;
    int findRow(const LogItem *nodeInfo) const;
};

#endif //PROTOSLOGVIEWER_ITEMMODEL_H
