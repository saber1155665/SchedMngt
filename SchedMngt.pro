QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets 

CONFIG += c++11
CONFIG += x86_64
CONFIG += qxt


include(third-party/qxtglobalshortcut5/qxt.pri)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += SQLITE_USE_LEGACY_STRUCT
DEFINES += SQLITE_HAS_CODEC #开启数据库加密

# 强制 MSVC 使用 UTF-8 编码解析源码
msvc {
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}

win32:CONFIG(debug, debug|release): {
    LIBS += $$PWD/third-party/ced/debug/ced.lib
    #INCLUDEPATH += $$QMAKE_SKIA_DIR/out/Debug-x64
    #DEPENDPATH += $$QMAKE_SKIA_DIR/out/Debug-x64
    QMAKE_CFLAGS_DEBUG += -MTd
    QMAKE_CXXFLAGS_DEBUG += -MTd
}
else:win32:CONFIG(release, debug|release): {
    LIBS += $$PWD/third-party/ced/release/ced.lib
    #INCLUDEPATH += $$QMAKE_SKIA_DIR/out/Release-x64
    #DEPENDPATH += $$QMAKE_SKIA_DIR/out/Release-x64
    #win32:QMAKE_CXXFLAGS += /MD
    QMAKE_CFLAGS_RELEASE += -MT
    QMAKE_CXXFLAGS_RELEASE += -MT
}

INCLUDEPATH += "C:/Program Files (x86)/Windows Kits/10/Include/10.0.19041.0/ucrt"
INCLUDEPATH += $$PWD/src/
INCLUDEPATH += $$PWD/src/utils
INCLUDEPATH += $$PWD/src/main/
INCLUDEPATH += $$PWD/src/db/
INCLUDEPATH += $$PWD/src/lunar/
INCLUDEPATH += $$PWD/src/sqlite/
INCLUDEPATH += $$PWD/src/sqlitecpp/
INCLUDEPATH += $$PWD/src/common/
INCLUDEPATH += $$PWD/src/messagebox/
INCLUDEPATH += $$PWD/src/timed_task/
INCLUDEPATH += $$PWD/src/popupwindows/
INCLUDEPATH += $$PWD/src/work_log
INCLUDEPATH += $$PWD/src/prompt_log
INCLUDEPATH += $$PWD/src/develop_toolbox
INCLUDEPATH += $$PWD/src/develop_toolbox/network_conn_test
INCLUDEPATH += $$PWD/src/develop_toolbox/lua
INCLUDEPATH += $$PWD/src/develop_toolbox/string_tools
INCLUDEPATH += $$PWD/src/develop_toolbox/code_file_template
INCLUDEPATH += $$PWD/src/develop_toolbox/date_time
INCLUDEPATH += $$PWD/src/develop_toolbox/file_deal
INCLUDEPATH += $$PWD/src/tray_windows
INCLUDEPATH += $$PWD/src/widget
INCLUDEPATH += $$PWD/src/fast_start
INCLUDEPATH += $$PWD/third-party/ced/include

LIBS += "-LC:/Program Files (x86)/Windows Kits/10/Lib/10.0.19041.0/ucrt/x86" "-LF:/qt/qt5.12.12/5.12.12/msvc2017/lib"


SOURCES += \
    main.cpp \
    src/develop_toolbox/code_file_template/code_file_template.cpp \
    src/develop_toolbox/custom_tree_delegate.cpp \
    src/develop_toolbox/develop_toolbox_main.cpp \
    src/develop_toolbox/file_deal/count_file_extensions.cpp \
    src/develop_toolbox/file_deal/file_encode_check.cpp \
    src/develop_toolbox/file_deal/file_to_array.cpp \
    src/develop_toolbox/string_tools/format_json_2_show.cpp \
    src/develop_toolbox/network_conn_test/network_conn_icmp.cpp \
    src/develop_toolbox/network_conn_test/network_conn_tcp.cpp \
    src/develop_toolbox/network_conn_test/network_conn_udp.cpp \
    src/develop_toolbox/network_conn_test/network_conn_test.cpp \
    src/develop_toolbox/network_conn_test/sched_mngt_tcp.cpp \
    src/develop_toolbox/run_lua_script.cpp \
    src/develop_toolbox/string_tools/string_2_array.cpp \
    src/develop_toolbox/string_tools/string_random_dec.cpp \
    src/develop_toolbox/date_time/timestamp_2_datetime.cpp \
    src/fast_start/fast_start_develop_toolbox.cpp \
    src/fast_start/fast_start_key_value.cpp \
    src/main/main_update_bottom.cpp \
    src/prompt_log/add_log_info.cpp \
    src/prompt_log/prompt_log_info_mngt.cpp \
    src/messagebox/sched_mngt_message_box.cpp \
    src/popupwindows/right_bottom_popupwindow.cpp \
    src/timed_task/run_timed_task.cpp \
    src/timed_task/show_hide_calender.cpp \
    src/db/sqlite_tools.cpp \
    src/main/mainwindow.cpp \
    src/main/sched_mngt_table_item.cpp\
    src/sqlite/codec.c\
    src/sqlite/codecext.c\
    src/sqlite/extensionfunctions.c\
    src/sqlite/rijndael.c\
    src/sqlite/sha2.c\
    src/sqlite/sqlite3.c\
    src/sqlite/sqlite3secure.c\
    src/sqlite/userauth.c\
    src/sqlitecpp/Backup.cpp\
    src/sqlitecpp/Column.cpp\
    src/sqlitecpp/Database.cpp\
    src/sqlitecpp/Exception.cpp\
    src/sqlitecpp/Savepoint.cpp\
    src/sqlitecpp/Statement.cpp\
    src/sqlitecpp/Transaction.cpp\
    src/lunar/calendar.cpp \
    src/timed_task/show_hide_time.cpp \
    src/timed_task/timed_task.cpp \
    src/timed_task/timed_task_mngt.cpp \
    src/tray_windows/custom_tray_icon.cpp \
    src/tray_windows/tray_menu_widget.cpp \
    src/develop_toolbox/lua/lapi.c\
    src/develop_toolbox/lua/lauxlib.c\
    src/develop_toolbox/lua/lbaselib.c\
    src/develop_toolbox/lua/lcode.c\
    src/develop_toolbox/lua/lcorolib.c\
    src/develop_toolbox/lua/lctype.c\
    src/develop_toolbox/lua/ldblib.c\
    src/develop_toolbox/lua/ldebug.c\
    src/develop_toolbox/lua/ldo.c\
    src/develop_toolbox/lua/ldump.c\
    src/develop_toolbox/lua/lfunc.c\
    src/develop_toolbox/lua/lgc.c\
    src/develop_toolbox/lua/linit.c\
    src/develop_toolbox/lua/liolib.c\
    src/develop_toolbox/lua/llex.c\
    src/develop_toolbox/lua/lmathlib.c\
    src/develop_toolbox/lua/lmem.c\
    src/develop_toolbox/lua/loadlib.c\
    src/develop_toolbox/lua/lobject.c\
    src/develop_toolbox/lua/lopcodes.c\
    src/develop_toolbox/lua/loslib.c\
    src/develop_toolbox/lua/lparser.c\
    src/develop_toolbox/lua/lstate.c\
    src/develop_toolbox/lua/lstring.c\
    src/develop_toolbox/lua/lstrlib.c\
    src/develop_toolbox/lua/ltable.c\
    src/develop_toolbox/lua/ltablib.c\
    src/develop_toolbox/lua/ltests.c\
    src/develop_toolbox/lua/ltm.c\
    src/develop_toolbox/lua/lundump.c\
    src/develop_toolbox/lua/lutf8lib.c\
    src/develop_toolbox/lua/lvm.c\
    src/develop_toolbox/lua/lzio.c\
    src/develop_toolbox/lua/onelua.c\
    src/utils/self_define_enc_utils.cpp \
    src/work_log/work_log.cpp \
    src/work_log/work_log_mngt.cpp

HEADERS += \
    src/common/sched_mngt_utils.h \
    src/develop_toolbox/code_file_template/code_file_template.h \
    src/develop_toolbox/custom_tree_delegate.h \
    src/develop_toolbox/develop_toolbox_main.h \
    src/develop_toolbox/file_deal/count_file_extensions.h \
    src/develop_toolbox/file_deal/file_encode_check.h \
    src/develop_toolbox/file_deal/file_to_array.h \
    src/develop_toolbox/string_tools/format_json_2_show.h \
    src/develop_toolbox/network_conn_test/network_conn_icmp.h \
    src/develop_toolbox/network_conn_test/network_conn_tcp.h \
    src/develop_toolbox/network_conn_test/network_conn_udp.h \
    src/develop_toolbox/network_conn_test/network_conn_test.h \
    src/develop_toolbox/network_conn_test/sched_mngt_tcp.h \
    src/develop_toolbox/run_lua_script.h \
    src/develop_toolbox/string_tools/string_2_array.h \
    src/develop_toolbox/string_tools/string_random_dec.h \
    src/develop_toolbox/date_time/timestamp_2_datetime.h \
    src/fast_start/fast_start_develop_toolbox.h \
    src/fast_start/fast_start_key_value.h \
    src/main/main_update_bottom.h \
    src/prompt_log/add_log_info.h \
    src/prompt_log/prompt_log_info_mngt.h \
    src/messagebox/sched_mngt_message_box.h \
    src/popupwindows/right_bottom_popupwindow.h \
    src/timed_task/run_timed_task.h \
    src/timed_task/show_hide_calender.h \
    src/db/sqlite_tools.h \
    src/main/mainwindow.h \
    src/main/sched_mngt_table_item.h\
    src/sqlite/codec.h\
    src/sqlite/rijndael.h\
    src/sqlite/sha2.h\
    src/sqlite/sqlite3.h\
    src/sqlite/sqlite3ext.h\
    src/sqlite/sqlite3userauth.h\
    src/sqlitecpp/Assertion.h\
    src/sqlitecpp/Backup.h\
    src/sqlitecpp/Column.h\
    src/sqlitecpp/Database.h\
    src/sqlitecpp/Exception.h\
    src/sqlitecpp/ExecuteMany.h\
    src/sqlitecpp/Savepoint.h\
    src/sqlitecpp/SQLiteCpp.h\
    src/sqlitecpp/SQLiteCppExport.h\
    src/sqlitecpp/Statement.h\
    src/sqlitecpp/Transaction.h\
    src/sqlitecpp/Utils.h\
    src/sqlitecpp/VariadicBind.h\
    src/lunar/calendar.h \
    src/timed_task/show_hide_time.h \
    src/timed_task/timed_task.h \
    src/timed_task/timed_task_mngt.h \
    src/tray_windows/custom_tray_icon.h \
    src/tray_windows/tray_menu_widget.h \
    src/utils/self_define_enc_utils.h \
    src/widget/breathing_progress_bar.h \
    src/work_log/work_log.h \
    src/work_log/work_log_mngt.h

FORMS += \
    src/develop_toolbox/code_file_template/code_file_template.ui \
    src/develop_toolbox/develop_toolbox_main.ui \
    src/develop_toolbox/file_deal/count_file_extensions.ui \
    src/develop_toolbox/file_deal/file_encode_check.ui \
    src/develop_toolbox/file_deal/file_to_array.ui \
    src/develop_toolbox/string_tools/format_json_2_show.ui \
    src/develop_toolbox/network_conn_test/network_conn_icmp.ui \
    src/develop_toolbox/network_conn_test/network_conn_tcp.ui \
    src/develop_toolbox/network_conn_test/network_conn_udp.ui \
    src/develop_toolbox/network_conn_test/network_conn_test.ui \
    src/develop_toolbox/run_lua_script.ui \
    src/develop_toolbox/string_tools/string_2_array.ui \
    src/develop_toolbox/string_tools/string_random_dec.ui \
    src/develop_toolbox/date_time/timestamp_2_datetime.ui \
    src/fast_start/fast_start_develop_toolbox.ui\
    src/prompt_log/prompt_log_info_mngt.ui \
    src/main/mainwindow.ui \
    src/messagebox/sched_mngt_message_box.ui \
    src/popupwindows/right_bottom_popupwindow.ui \
    src/main/sched_mngt_table_item.ui \
    src/timed_task/show_hide_calender.ui \
    src/timed_task/show_hide_time.ui \
    src/timed_task/timed_task.ui \
    src/timed_task/timed_task_mngt.ui \
    src/tray_windows/tray_menu_widget.ui \
    src/work_log/work_log.ui \
    src/work_log/work_log_mngt.ui
	
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource/root.qrc

RC_ICONS=resource/icon/tray_windows.ico
