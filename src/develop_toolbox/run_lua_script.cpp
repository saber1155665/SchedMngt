#include "run_lua_script.h"
#include "ui_run_lua_script.h"
#include "sched_mngt_utils.h"
extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}
#include <mutex>

run_lua_script::run_lua_script(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::run_lua_script)
{
    ui->setupUi(this);
    //绑定
    connect(ui->btn_run, &QPushButton::clicked, this, &run_lua_script::OnRunLua);

    ui->text_edit_lua_src->setPlaceholderText(QStringLiteral("-- 定义两个数字并直接在print中计算加法\r\n"
                                                  "local num1 = 10\r\n"
                                                  "local num2 = 20\r\n"
                                                  "\r\n"
                                                  "-- 打印结果，同时进行加法计算\r\n"
                                                  "print(\"The sum is:\", num1 + num2)\r\n"));
}

run_lua_script::~run_lua_script()
{
    delete ui;
}

// 全局缓冲区（或封装为类成员，避免全局变量）
static std::string luaPrintBuffer;
static std::mutex printMutex;
// 自定义Lua print函数（C函数）
static int luaCustomPrint(lua_State* L) {
    int n = lua_gettop(L); // 获取参数个数
    lua_getglobal(L, "tostring"); // 调用Lua的tostring转换参数为字符串

    for (int i = 1; i <= n; ++i) {
        lua_pushvalue(L, -1); // 复制tostring函数到栈顶
        lua_pushvalue(L, i);  // 压入第i个参数
        lua_call(L, 1, 1);    // 调用tostring(arg)，返回字符串

        const char* s = lua_tostring(L, -1); // 获取转换后的字符串
        if (s) {
            if (i > 1) luaPrintBuffer += "\t"; // 参数间加制表符
            luaPrintBuffer += s;
        }
        lua_pop(L, 1); // 弹出tostring的返回值
    }
    luaPrintBuffer += "\n"; // 换行（模拟原生print）
    return 0; // 自定义函数返回值个数
}

//执行Lua脚本
void run_lua_script::OnRunLua() {
    //先清空数据
    luaPrintBuffer.clear();
    //获取输入
    QString qstrLuaScript = ui->text_edit_lua_src->toPlainText();
    //执行
    lua_State* L = luaL_newstate();  // 创建新的 Lua 状态机
    luaL_openlibs(L); // 打开Lua标准库（含io、string等
    // 替换全局print函数为自定义函数
    lua_register(L, "print", luaCustomPrint);
    int status = luaL_dostring(L, GetStdUtf8String(qstrLuaScript).c_str());  // 执行脚本
    lua_close(L);  // 关闭并释放 Lua 状态机资源
    //设置结果
    QString qstrResult = QString::fromUtf8(luaPrintBuffer.c_str());
    ui->text_edit_lua_result->setText(qstrResult);
}
