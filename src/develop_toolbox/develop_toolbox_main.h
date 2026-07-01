#ifndef DEVELOP_TOOLBOX_MAIN_H
#define DEVELOP_TOOLBOX_MAIN_H

#include "fast_start_key_value.h"

#include <QDialog>
#include <QDialog>
#include <QList>
#include <QTreeWidgetItem>

namespace Ui {
class develop_toolbox_main;
}

class develop_toolbox_main : public QDialog
{
    Q_OBJECT
       //关联显示字符和操作类型
    struct TreeWidgetShow2Action {
        QString m_qstrText;
        TreeItemAction m_action;
        QWidget *m_pWidget;
    };
    class TreeWidgetShow2ActionList {
    public:
        TreeWidgetShow2ActionList &operator<<(const TreeWidgetShow2Action &item) {
            m_listItem.push_back(item);
            return *this;
        }
        QList<TreeWidgetShow2Action> m_listItem;
    };

public:
    explicit develop_toolbox_main(QWidget *parent = nullptr);
    ~develop_toolbox_main();
    //快捷输入启动
    void OnFastStartDevelopToolbox(const TreeItemAction action);
private:
    //初始化树形控件
    void InitTreeWidget();
    //初始化stackWidget
    void InitStackWidget();
    //初始化映射关系。方便管理,避免到处添加代码
    void InitTreeActionMap();
    //选中项
    void OnCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    //退出
    void OnExit();
    // 新增：拦截按键事件（核心）
    void keyPressEvent(QKeyEvent *event) override;
private:
    Ui::develop_toolbox_main *ui;
    //映射关系
    QMap<QString, TreeWidgetShow2ActionList> m_qMapTreeItem;
};

#endif // DEVELOP_TOOLBOX_MAIN_H
