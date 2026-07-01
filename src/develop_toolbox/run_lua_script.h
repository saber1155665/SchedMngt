#ifndef RUN_LUA_SCRIPT_H
#define RUN_LUA_SCRIPT_H

#include <QWidget>

namespace Ui {
class run_lua_script;
}

class run_lua_script : public QWidget
{
    Q_OBJECT

public:
    explicit run_lua_script(QWidget *parent = nullptr);
    ~run_lua_script();

private:
    //执行Lua脚本
    void OnRunLua();
private:
    Ui::run_lua_script *ui;
};

#endif // RUN_LUA_SCRIPT_H
