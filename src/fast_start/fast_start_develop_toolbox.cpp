#include "fast_start_develop_toolbox.h"
#include "ui_fast_start_develop_toolbox.h"
#include "sched_mngt_message_box.h"

#include <qxtglobalshortcut.h>
#include <QCompleter>

fast_start_develop_toolbox::fast_start_develop_toolbox(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::fast_start_develop_toolbox)
{
    ui->setupUi(this);
    // ========== 1. 窗口透明核心设置 ==========
    setWindowFlags(Qt::FramelessWindowHint); // 去掉窗口标题栏和边框
    setAttribute(Qt::WA_TranslucentBackground, true); // 启用半透明背景
    setAttribute(Qt::WA_NoSystemBackground, true);    // 禁用系统默认背景
    //设置快捷启动键
    QxtGlobalShortcut *shortcut = new QxtGlobalShortcut(this);
    connect(shortcut, &QxtGlobalShortcut::activated, this, &fast_start_develop_toolbox::OnGlobalShortcut);
    shortcut->setShortcut(QKeySequence("Ctrl+Shift+z"));
    //
    connect(ui->lineEdit, &QLineEdit::returnPressed, this, &fast_start_develop_toolbox::OnEditText);
    //设置模糊搜索
    QStringList listSearch = FastStartKV().GetFuzzyQueryAllName();
    QCompleter *pCompleter = new QCompleter(listSearch, this);
    pCompleter->setFilterMode(Qt::MatchContains); // 模糊匹配
    pCompleter->setCaseSensitivity(Qt::CaseInsensitive);//忽略大小写
    ui->lineEdit->setCompleter(pCompleter);
    //默认不显示
    hide();
}

fast_start_develop_toolbox::~fast_start_develop_toolbox()
{
    delete ui;
}

//系统快捷键响应
void fast_start_develop_toolbox::OnGlobalShortcut() {
    //显示
    show();
    //激活窗口
    activateWindow();
}

//编辑框输入响应
void fast_start_develop_toolbox::OnEditText() {
    hide();
    QString qstrText = ui->lineEdit->text();
    TreeItemAction action = FastStartKV().GetFuzzyQuery(qstrText);
    if (TIA_UNKNOWN == action) {
        sched_mngt_message_box::question(QStringLiteral("不支持的类型"), sched_mngt_message_box::MBT_ERROR);
        return;
    }
    emit OnFastStartDevelopToolboxSignals(action);
}

//拦截关闭
void fast_start_develop_toolbox::closeEvent(QCloseEvent *event)  {
    this->hide(); // 隐藏对话框而不是关闭
    event->ignore(); // 忽略关闭事件
}

// 新增：拦截按键事件（核心）
void fast_start_develop_toolbox::keyPressEvent(QKeyEvent *event) {
    // 判断是否是ESC键
    if (event->key() == Qt::Key_Escape) {
        hide(); // 隐藏对话框而不是关闭
        // 忽略ESC事件，不触发任何默认行为（如关闭窗口）
        event->ignore();
        // 可选：如果需要提示，可添加qDebug() << "ESC键已被禁用";
    } else if (event->key() == Qt::Key_Enter) {
        hide(); // 隐藏对话框而不是关闭
        // 忽略ESC事件，不触发任何默认行为（如关闭窗口）
        event->ignore();
    } else {
        // 其他按键正常传递（保留原有功能）
        QDialog::keyPressEvent(event);
    }
}
