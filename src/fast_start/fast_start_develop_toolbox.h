#ifndef FAST_START_DEVELOP_TOOLBOX_H
#define FAST_START_DEVELOP_TOOLBOX_H

#include "fast_start_key_value.h"

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class fast_start_develop_toolbox;
}

class fast_start_develop_toolbox : public QDialog
{
    Q_OBJECT

public:
    explicit fast_start_develop_toolbox(QWidget *parent = nullptr);
    ~fast_start_develop_toolbox();

signals:
   //槽， 快捷输入启动
    void OnFastStartDevelopToolboxSignals(const TreeItemAction action);

private:
    //系统快捷键响应
    void OnGlobalShortcut();
    //编辑框输入响应
    void OnEditText();
    //拦截关闭
    void closeEvent(QCloseEvent *event) override;
    // 新增：拦截按键事件（核心）
    void keyPressEvent(QKeyEvent *event) override;
private:
    Ui::fast_start_develop_toolbox *ui;
};

#endif // FAST_START_DEVELOP_TOOLBOX_H
