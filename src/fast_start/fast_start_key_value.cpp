#include "fast_start_key_value.h"

fast_start_key_value& FastStartKV() {
    static fast_start_key_value kv;
    return kv;
}

fast_start_key_value::fast_start_key_value()
{
    m_mapAction[TIA_NETWORK_CONNECT_TEST] = QStringLiteral("网络连接测试");
    m_mapAction[TIA_NETWORK_CONNECT_TEST_TCP] = QStringLiteral("网络连接测试TCP");
    m_mapAction[TIA_NETWORK_CONNECT_TEST_UDP] = QStringLiteral("网络连接测试UDP");
    m_mapAction[TIA_NETWORK_CONNECT_TEST_ICMP] = QStringLiteral("网络连接测试ICMP");
    m_mapAction[TIA_TIMESTAMP_CHANGE_DATETIME] = QStringLiteral("时间戳和日期转换");
    m_mapAction[TIA_LUA_SCRIPT] = QStringLiteral("Lua脚本执行");
    m_mapAction[TIA_SMALL_STRING_2_ARRAY] = QStringLiteral("字符串转数组");
	m_mapAction[TIA_SMALL_STRING_RANDOM_ENC] = QStringLiteral("字符串随机加密");
    m_mapAction[TIA_FILE_ENCODE_CHECK] = QStringLiteral("文本文件编码检测和转换");
    m_mapAction[TIA_FILE_GENERATE_CODE_FILE] = QStringLiteral("生成代码文件");
    m_mapAction[TIA_SMALL_JSON_FORMAT] = QStringLiteral("格式化JSON");
    m_mapAction[TIA_FILE_COUNT_FILE_EXTENSION] = QStringLiteral("统计文件后缀和文件个数");
    m_mapAction[TIA_FILE_TO_ARRAY] = QStringLiteral("文件转数组");


    //模糊查询
    m_mapFuzzyQuery[QStringLiteral("网络连接TCP测试")] = TIA_NETWORK_CONNECT_TEST_TCP;
    m_mapFuzzyQuery[QStringLiteral("网络连接UDP测试")] = TIA_NETWORK_CONNECT_TEST_UDP;
    m_mapFuzzyQuery[QStringLiteral("网络连接ICMP测试")] = TIA_NETWORK_CONNECT_TEST_ICMP;

    m_mapFuzzyQuery[QStringLiteral("时间戳和日期转换")] = TIA_TIMESTAMP_CHANGE_DATETIME;
    m_mapFuzzyQuery[QStringLiteral("timestamp")] = TIA_TIMESTAMP_CHANGE_DATETIME;
    m_mapFuzzyQuery[QStringLiteral("datetime")] = TIA_TIMESTAMP_CHANGE_DATETIME;

    m_mapFuzzyQuery[QStringLiteral("Lua脚本执行")] = TIA_LUA_SCRIPT;
    m_mapFuzzyQuery[QStringLiteral("Lua")] = TIA_LUA_SCRIPT;

    m_mapFuzzyQuery[QStringLiteral("字符串转数组")] = TIA_SMALL_STRING_2_ARRAY;
    m_mapFuzzyQuery[QStringLiteral("string to array")] = TIA_SMALL_STRING_2_ARRAY;
	
    m_mapFuzzyQuery[QStringLiteral("字符串随机加密")] = TIA_SMALL_STRING_RANDOM_ENC;
    m_mapFuzzyQuery[QStringLiteral("string random encrypt")] = TIA_SMALL_STRING_RANDOM_ENC;

    m_mapFuzzyQuery[QStringLiteral("文本文件编码检测和转换")] = TIA_FILE_ENCODE_CHECK;
    m_mapFuzzyQuery[QStringLiteral("file encode & change")] = TIA_FILE_ENCODE_CHECK;

    m_mapFuzzyQuery[QStringLiteral("原代码模板文件")] = TIA_FILE_GENERATE_CODE_FILE;
    m_mapFuzzyQuery[QStringLiteral("code file generate")] = TIA_FILE_GENERATE_CODE_FILE;

    m_mapFuzzyQuery[QStringLiteral("格式化JSON")] = TIA_SMALL_JSON_FORMAT;
    m_mapFuzzyQuery[QStringLiteral("Format Json")] = TIA_SMALL_JSON_FORMAT;

    m_mapFuzzyQuery[QStringLiteral("统计文件后缀和文件个数")] = TIA_FILE_COUNT_FILE_EXTENSION;
    m_mapFuzzyQuery[QStringLiteral("count file extension")] = TIA_FILE_COUNT_FILE_EXTENSION;

    m_mapFuzzyQuery[QStringLiteral("文件转数组")] = TIA_FILE_TO_ARRAY;
    m_mapFuzzyQuery[QStringLiteral("file to array")] = TIA_FILE_TO_ARRAY;
}

//根据类型得到名字
QString fast_start_key_value::GetName(TreeItemAction action) {
    auto find = m_mapAction.find(action);
    if (m_mapAction.end() == find) {
        return QString();
    }
    return m_mapAction[action];
}

//模糊查询,根据字符串获取类型
TreeItemAction fast_start_key_value::GetFuzzyQuery(QString qstrName) {
    auto find = m_mapFuzzyQuery.find(qstrName);
    if (m_mapFuzzyQuery.end() == find) {
        return TIA_UNKNOWN;
    }
    return m_mapFuzzyQuery[qstrName];
}

//得到所有的模糊查询名
QStringList fast_start_key_value::GetFuzzyQueryAllName() {
    QStringList listFuzzyQuery;
    for (auto iter = m_mapFuzzyQuery.begin(); iter != m_mapFuzzyQuery.end(); iter++) {
        listFuzzyQuery.push_back(iter.key());
    }
    return listFuzzyQuery;
}
