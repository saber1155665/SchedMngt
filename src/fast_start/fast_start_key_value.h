#ifndef FAST_START_KEY_VALUE_H
#define FAST_START_KEY_VALUE_H

#include <QMap>

//树形控件，每个功能点的类型值.方便关联功能和显示
enum TreeItemAction {
    TIA_UNKNOWN,
    TIA_NETWORK_CONNECT_TEST,//网络连接测试
    TIA_NETWORK_CONNECT_TEST_TCP,//网络连接测试TCP(不显示在目录树)
    TIA_NETWORK_CONNECT_TEST_UDP,//网络连接测试UDP(不显示在目录树)
    TIA_NETWORK_CONNECT_TEST_ICMP,//网络连接测试ICMP(不显示在目录树)
    TIA_TIMESTAMP_CHANGE_DATETIME, //时间戳和日期的转换
    TIA_LUA_SCRIPT,         //lua脚本执行
    TIA_SMALL_STRING_2_ARRAY,         //小工具,字符串转数组
	TIA_SMALL_STRING_RANDOM_ENC,         //小工具,字符串随机加密
    TIA_SMALL_JSON_FORMAT,         //小工具,格式化JSON
    TIA_FILE_ENCODE_CHECK,  //检测文件编码
    TIA_FILE_GENERATE_CODE_FILE,  //生成代码文件
    TIA_FILE_COUNT_FILE_EXTENSION,  //统计目录下文件后缀
    TIA_FILE_TO_ARRAY,      //文件转C数组

    TIA_COUNT
};

class fast_start_key_value
{
public:
    fast_start_key_value();
    //根据类型得到名字
    QString GetName(TreeItemAction action);

    //模糊查询,根据字符串获取类型
    TreeItemAction GetFuzzyQuery(QString qstrName);
    //得到所有的模糊查询名
    QStringList GetFuzzyQueryAllName();

private:
    //类型到准确名字
    QMap<TreeItemAction, QString> m_mapAction;
    //模糊名到类型
    QMap<QString, TreeItemAction> m_mapFuzzyQuery;
};

fast_start_key_value& FastStartKV();
#endif // FAST_START_KEY_VALUE_H
