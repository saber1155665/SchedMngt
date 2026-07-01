#include "custom_tree_delegate.h"

custom_tree_delegate::custom_tree_delegate(QObject *parent)
    : QStyledItemDelegate(parent)
{

}



// 获取自定义缩进值（支持替换/叠加层级缩进，此处为叠加逻辑）
int custom_tree_delegate::getCustomIndent(const QModelIndex &index) const
{
    if (index.isValid() && index.data(CustomIndentRole).isValid()) {
        return index.data(CustomIndentRole).toInt();
    }
    return 0;
}

void custom_tree_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                               const QModelIndex &index) const
{
    // 复制样式选项，避免修改原数据
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // 调整绘制区域的X偏移（实现缩进）
    int indent = this->getCustomIndent(index);
    if (indent != 0) {
        opt.rect.adjust(indent, 0, 0, 0); // 向左偏移=缩进值
    }

    // 调用父类绘制逻辑
    QStyledItemDelegate::paint(painter, opt, index);
}

QSize custom_tree_delegate::sizeHint(const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    // 基础尺寸
    QSize size = QStyledItemDelegate::sizeHint(option, index);

    // 适配缩进宽度，避免文字截断
    int indent = getCustomIndent(index);
    if (indent != 0) {
        size.setWidth(size.width() + indent);
    }
    return size;
}
