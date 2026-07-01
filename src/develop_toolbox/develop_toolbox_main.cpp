#include "develop_toolbox_main.h"
#include "ui_develop_toolbox_main.h"
#include "custom_tree_delegate.h"
#include "network_conn_test.h"
#include "timestamp_2_datetime.h"
#include "run_lua_script.h"
#include "string_2_array.h"
#include "string_random_dec.h"
#include "file_encode_check.h"
#include "code_file_template.h"
#include "format_json_2_show.h"
#include "count_file_extensions.h"
#include "file_to_array.h"

#include <QMap>
#include <QString>
#include <QStringList>
#include <QStringLiteral>

#define ACTION_ID_KEY 0



develop_toolbox_main::develop_toolbox_main(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::develop_toolbox_main)
{
    ui->setupUi(this);
    //设置无边框
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    //绑定
    connect(ui->treeWidget, &QTreeWidget::currentItemChanged, this, &develop_toolbox_main::OnCurrentItemChanged);
    connect(ui->btn_close, &QPushButton::clicked, this, &develop_toolbox_main::OnExit);
    //分割线
    ui->splitter->setHandleWidth(2); // 分割线宽度
    ui->splitter->setChildrenCollapsible(false); // 禁止子控件折叠
    //初始化映射关系
    InitTreeActionMap();
    //树形控件
    InitTreeWidget();
    //初始化stackWidget
    InitStackWidget();

    // 设置分割窗口初始比例（左:右 = 1:4）
    QList<int> sizes;
    sizes << 200 << 800;
    ui->splitter->setSizes(sizes);
}

develop_toolbox_main::~develop_toolbox_main()
{
    delete ui;
}


//初始化映射关系。方便管理,避免到处添加代码
void develop_toolbox_main::InitTreeActionMap() {

    //网络功能
    TreeWidgetShow2ActionList qStrListNetwork;
    qStrListNetwork << TreeWidgetShow2Action{FastStartKV().GetName(TIA_NETWORK_CONNECT_TEST), TIA_NETWORK_CONNECT_TEST, new network_conn_test(this)};
    m_qMapTreeItem[QStringLiteral("网络功能")] = qStrListNetwork;
    //时间功能
    TreeWidgetShow2ActionList qStrListTimes;
    qStrListTimes << TreeWidgetShow2Action {FastStartKV().GetName(TIA_TIMESTAMP_CHANGE_DATETIME), TIA_TIMESTAMP_CHANGE_DATETIME, new timestamp_2_datetime(this)};
    m_qMapTreeItem[QStringLiteral("时间功能")] = qStrListTimes;
    //脚本执行
    TreeWidgetShow2ActionList qStrListScript;
    qStrListScript << TreeWidgetShow2Action{FastStartKV().GetName(TIA_LUA_SCRIPT), TIA_LUA_SCRIPT, new run_lua_script(this)};
    m_qMapTreeItem[QStringLiteral("脚本执行")] = qStrListScript;
    //小工具
    TreeWidgetShow2ActionList qStrListSmallTools;
    qStrListSmallTools << TreeWidgetShow2Action{FastStartKV().GetName(TIA_SMALL_STRING_2_ARRAY), TIA_SMALL_STRING_2_ARRAY, new string_2_array(this)};
    qStrListSmallTools << TreeWidgetShow2Action{FastStartKV().GetName(TIA_SMALL_STRING_RANDOM_ENC), TIA_SMALL_STRING_RANDOM_ENC, new string_random_dec(this)};
    qStrListSmallTools << TreeWidgetShow2Action{FastStartKV().GetName(TIA_SMALL_JSON_FORMAT), TIA_SMALL_JSON_FORMAT, new format_json_2_show(this)};
    m_qMapTreeItem[QStringLiteral("小工具")] = qStrListSmallTools;
    //文件操作
    TreeWidgetShow2ActionList qStrListFileCtl;
    qStrListFileCtl << TreeWidgetShow2Action{FastStartKV().GetName(TIA_FILE_ENCODE_CHECK), TIA_FILE_ENCODE_CHECK, new file_encode_check(this)};
    qStrListFileCtl << TreeWidgetShow2Action{FastStartKV().GetName(TIA_FILE_GENERATE_CODE_FILE), TIA_FILE_GENERATE_CODE_FILE, new code_file_template(this)};
    qStrListFileCtl << TreeWidgetShow2Action{FastStartKV().GetName(TIA_FILE_COUNT_FILE_EXTENSION), TIA_FILE_COUNT_FILE_EXTENSION, new count_file_extensions(this)};
    qStrListFileCtl << TreeWidgetShow2Action{FastStartKV().GetName(TIA_FILE_TO_ARRAY), TIA_FILE_TO_ARRAY, new file_to_array(this)};
    m_qMapTreeItem[QStringLiteral("文件")] = qStrListFileCtl;
}

//初始化树形控件
void develop_toolbox_main::InitTreeWidget() {
    // 设置树形控件列数（仅3列）
    //ui->treeWidget->setColumnCount(3);
    // 隐藏表头
    ui->treeWidget->header()->hide();
    ui->treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents); // 根据内容调整大小
    //设置自定义委托
    custom_tree_delegate *delegate = new custom_tree_delegate(this);
    ui->treeWidget->setItemDelegate(delegate);
    //根节点
    QTreeWidgetItem *pRootWidgetItem = new QTreeWidgetItem(ui->treeWidget);
    //第一行不要缩进
    pRootWidgetItem->setData(0, CustomIndentRole, 0);
    pRootWidgetItem->setText(0, QString());
    pRootWidgetItem->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
    for (auto iter = m_qMapTreeItem.begin(); iter != m_qMapTreeItem.end(); iter++) {
        //二级节点
        QTreeWidgetItem *pFirstWidgetItem = new QTreeWidgetItem(pRootWidgetItem);
        pFirstWidgetItem->setText(0, iter.key());
        //目录没有action
        pFirstWidgetItem->setData(ACTION_ID_KEY, Qt::UserRole, -1);
        //第一行不要缩进
        pFirstWidgetItem->setData(0, CustomIndentRole, 8);
        pFirstWidgetItem->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
        const QList<TreeWidgetShow2Action>  &qstrListTmp = iter->m_listItem;
        for (auto iterSecond =  qstrListTmp.begin();iterSecond != qstrListTmp.end(); iterSecond++) {
            //三级节点
            QTreeWidgetItem *pSecondWidgetItem = new QTreeWidgetItem(pFirstWidgetItem);
            pSecondWidgetItem->setText(0, iterSecond->m_qstrText);
            //设置action
            pSecondWidgetItem->setData(ACTION_ID_KEY, Qt::UserRole, iterSecond->m_action); // 绑定索引（2）
            //第二行缩进两个字符
            pSecondWidgetItem->setData(0, CustomIndentRole, 16);
            pSecondWidgetItem->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
        }
    }
    // 展开所有节点
    ui->treeWidget->expandAll();
}


//初始化stackWidget
void develop_toolbox_main::InitStackWidget() {
    for (auto iter = m_qMapTreeItem.begin(); iter != m_qMapTreeItem.end(); iter++) {
        const QList<TreeWidgetShow2Action>  &qstrListTmp = iter->m_listItem;
        for (auto iterSecond =  qstrListTmp.begin();iterSecond != qstrListTmp.end(); iterSecond++) {
            iterSecond->m_pWidget->show();
            int xx = ui->stackedWidget->addWidget(iterSecond->m_pWidget);
            continue;
        }
    }
    //默认第一个
    ui->stackedWidget->setCurrentIndex(TIA_NETWORK_CONNECT_TEST);
    ui->stackedWidget->show();
}

//选中项
void develop_toolbox_main::OnCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous) {
    //得到action
    TreeItemAction nAction = static_cast<TreeItemAction>(current->data(ACTION_ID_KEY, Qt::UserRole).toInt());
    if (nAction < 0) {
        return;
    }
    //找到widget
    QWidget *widget = nullptr;
    for (auto iter = m_qMapTreeItem.begin(); iter != m_qMapTreeItem.end(); iter++) {
        const QList<TreeWidgetShow2Action>  &qstrListTmp = iter->m_listItem;
        for (auto iterSecond =  qstrListTmp.begin();iterSecond != qstrListTmp.end(); iterSecond++) {
            if (iterSecond->m_action == nAction) {
                widget = iterSecond->m_pWidget;
            }
        }
    }
    if (nullptr == widget) {
        return;
    }
    //选中
    ui->stackedWidget->setCurrentWidget(widget);
}

//退出
void develop_toolbox_main::OnExit() {
     hide();
    //close();
}

//快捷输入启动
void develop_toolbox_main::OnFastStartDevelopToolbox(const TreeItemAction nAction) {
    TreeItemAction nTmpAction = nAction;
    //网络测试时候，内部还有一层StackWidget,因此需要再次转换
    if ( (TIA_NETWORK_CONNECT_TEST_TCP == nAction)
         || (TIA_NETWORK_CONNECT_TEST_UDP == nAction)
         || (TIA_NETWORK_CONNECT_TEST_ICMP == nAction)
        ) {
        nTmpAction = TIA_NETWORK_CONNECT_TEST;
    }
    //找到widget
    QWidget *widget = nullptr;
    for (auto iter = m_qMapTreeItem.begin(); iter != m_qMapTreeItem.end(); iter++) {
        const QList<TreeWidgetShow2Action>  &qstrListTmp = iter->m_listItem;
        for (auto iterSecond =  qstrListTmp.begin();iterSecond != qstrListTmp.end(); iterSecond++) {
            if (iterSecond->m_action == nTmpAction) {
                widget = iterSecond->m_pWidget;
            }
        }
    }
    //网络测试的项目因为共用一个action
    if (TIA_NETWORK_CONNECT_TEST == nTmpAction) {
        network_conn_test *pConnTest = dynamic_cast<network_conn_test *>(widget);
        pConnTest->ChangeItem(nAction);
    }
    ui->stackedWidget->setCurrentWidget(widget);
}

// 新增：拦截按键事件（核心）
void develop_toolbox_main::keyPressEvent(QKeyEvent *event) {
    // 判断是否是ESC键
    if (event->key() == Qt::Key_Escape) {
        hide(); // 隐藏对话框而不是关闭
        // 忽略ESC事件，不触发任何默认行为（如关闭窗口）
        event->ignore();
        // 可选：如果需要提示，可添加qDebug() << "ESC键已被禁用";
    } else {
        // 其他按键正常传递（保留原有功能）
        QDialog::keyPressEvent(event);
    }
}
