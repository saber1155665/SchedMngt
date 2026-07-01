#ifndef CUSTOM_TREE_DELEGATE_H
#define CUSTOM_TREE_DELEGATE_H

#include <QStyledItemDelegate>
#include <QModelIndex>
#include <QPainter>

// 自定义数据角色：存储缩进值（避免与Qt内置角色冲突）
const int CustomIndentRole = Qt::UserRole + 100;

class custom_tree_delegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    custom_tree_delegate(QObject *parent = nullptr);

    // 重写绘制逻辑
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    // 重写尺寸计算（适配缩进宽度）
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

private:
    // 获取Item的自定义缩进值（无则返回0）
    int getCustomIndent(const QModelIndex &index) const;
};

#endif // CUSTOM_TREE_DELEGATE_H
