#include "file_encode_check.h"
#include "ui_file_encode_check.h"
#include "compact_enc_det/compact_enc_det.h"


#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QTextCodec>
#include <QTemporaryFile>

// 定义40MB的字节数（1MB = 1024KB，1KB = 1024字节）
#define MAX_FILE_SIZE  (40 * 1024 * 1024) // 41943040 字节

//提醒当前处理文件的时间间隔,单位毫秒
#define INTERVAL_CURRENT_ITEN (500)

//////////////////////////////////////////thread_access_dir//////////////////////////////////////////////
thread_access_dir::thread_access_dir(FileAccessAction nAction, const QString &qstrDir,  QStringList listStrFileExt, const QString &qstrEncodeName, QObject *parent)
   : m_qstrDir(qstrDir)
   , m_listStrFileExt(listStrFileExt)
   , m_qstrEncodeName(qstrEncodeName)
   , m_nAction(nAction)
{
    m_privTime = QTime::currentTime();
}

void thread_access_dir::run() {
    //清空数据
    m_qstrPrivFindCheckResult = "";
    //处理
    ProcessFiles(m_qstrDir, std::bind(&thread_access_dir::ProcessFileCB, this, std::placeholders::_1, std::placeholders::_2));
    //发送结果
    SendDealOverSignal(m_qstrPrivFindCheckResult);
}

//发送信号
void thread_access_dir::SendDealFileItemSignal(const QString &qstrFilePath) {
    //记录上次发送时间，避免处理
    QTime qtimeCurrent = QTime::currentTime();
    qint64 i64Time = m_privTime.msecsTo(qtimeCurrent);
    if (i64Time >= INTERVAL_CURRENT_ITEN) {
        emit OnDealFileItemSignal(qstrFilePath);
        m_privTime = qtimeCurrent;
    }
}

//发送处理完成的结果信号
void thread_access_dir::SendDealOverSignal(const QString &qstrResult) {
    emit OnDealOverSignal(qstrResult);
}

//发送处理过程的提示信息
void thread_access_dir::SendFileAccessMsgBoxSignal(const QString &qstrMsg, sched_mngt_message_box::MsgBoxType nType) {
    emit OnFileAccessMsgBoxSignal(qstrMsg, nType);
}

/**
 * @brief 递归处理文件/目录
 * @param path 目标路径（文件/目录）
 * @param callback 文件处理回调函数
 * @return 整体处理是否成功
 */
bool thread_access_dir::ProcessFiles(const QString& path, const FileProcessCallback& callback)
{
    QFileInfo fileInfo(path);

    // 检查路径是否存在
    if (!fileInfo.exists()) {
        return false;
    }

    // 场景1：是文件 → 先判断大小，再读取并调用回调
    if (fileInfo.isFile()) {
        // 核心改动1：判断文件大小是否超过40MB，超过则跳过
        qint64 fileSize = fileInfo.size();
        if (fileSize > MAX_FILE_SIZE) {
            return true; // 跳过不影响整体流程，返回true
        }
        //过滤后缀
        if (m_listStrFileExt.size() > 0) {
            bool bFind = false;
            for (auto iter = m_listStrFileExt.begin(); iter != m_listStrFileExt.end(); iter++) {
                if (path.endsWith(*iter)) {
                    bFind = true;
                    break;
                }
            }
            if (!bFind) {
                return true;
            }
        }


        QFile file(path);
        // 以只读、二进制模式打开（兼容文本/二进制文件）
        if (!file.open(QIODevice::ReadOnly)) {
            return false;
        }

        // 读取文件全部内容
        QByteArray content = file.readAll();
        file.close();

        // 调用回调函数处理
        if (!callback(path, content)) {
            return false;
        }
        return true;
    }

    // 场景2：是目录 → 递归遍历所有子项
    if (fileInfo.isDir()) {
        QDir dir(path);
        // 配置遍历规则：包含所有文件/目录、递归子目录、不显示隐藏文件（可按需调整）
        QDir::Filters filters = QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot;
        QDir::SortFlags sortFlags = QDir::Name | QDir::DirsFirst;

        // 获取目录下所有项的文件信息
        QFileInfoList fileInfoList = dir.entryInfoList(filters, sortFlags);
        for (const QFileInfo& info : fileInfoList) {
            // 递归处理子项（文件/子目录）
            if (!ProcessFiles(info.absoluteFilePath(), callback)) {
                return false;
            }
        }
        return true;
    }

    return false;
}


//回调函数
bool thread_access_dir::ProcessFileCB(const QString& filePath, const QByteArray& content) {
    bool is_reliable;
    int bytes_consumed;
    const char * pText = content.data();
    int nLen = content.length();
    //检测编码
    Encoding encoding = CompactEncDet::DetectEncoding(
            pText, nLen,
            nullptr, nullptr, nullptr,
            UNKNOWN_ENCODING,
            UNKNOWN_LANGUAGE,
            CompactEncDet::WEB_CORPUS,
            false,
            &bytes_consumed,
            &is_reliable);
    const char *pEncodingName = MimeEncodingName(encoding);
    QString qstrEncodingName = QString::fromUtf8(pEncodingName);
    //二进制文件我们不用处理
    if (BINARYENC  == encoding) {
        return true;
    }
    //提示当前处理
    SendDealFileItemSignal(filePath);

    if (FAA_CheckEncode == m_nAction) {//检测编码
        //保存结果
        m_qstrPrivFindCheckResult += filePath + " encode:" +  qstrEncodingName + "\r\n";
    } else if (FAA_EncodeChange == m_nAction) { //转码
        //ascii编码就不用改变了
        if (ASCII_7BIT == encoding) {
            return true;
        }
        return ProcessFileEncode(filePath , qstrEncodingName, m_qstrEncodeName, content);
    } else if (FAA_DeleteZH == m_nAction) {     //去除中文
        //ascii编码就不用改变了
        if (ASCII_7BIT == encoding) {
            return true;
        }
        //去除中文
        if (!DeleteZH(filePath, qstrEncodingName, content)) {
            return false;
        }
    }
    return true;
}


//对文件统一编码进行保存
bool thread_access_dir::ProcessFileEncode(const QString& qstrFilePath
                                          , const QString &qstrOldEncodingName
                                          , const QString &qstrNewEncodingName
                                          , const QByteArray& content) {
    if (qstrOldEncodingName == qstrNewEncodingName) {
        return true;
    }
    // 1. 检查文件是否存在且为普通文件
    QFileInfo fileInfo(qstrFilePath);
    if (!fileInfo.exists()) {
        return false;
    }
    if (!fileInfo.isFile()) {
        return false;
    }
    if (!fileInfo.isWritable()) {
        return false;
    }

    //转为指定编码
    QByteArray byteArrayEncode = ChangeUncode(content
                                              , qstrOldEncodingName
                                              , qstrNewEncodingName);
    if (byteArrayEncode.isEmpty()) {
        m_qstrPrivFindCheckResult += QStringLiteral("!!!!!!!!!!!!!!!") + qstrFilePath + QStringLiteral("文件转码错误\r\n");
        return true;
    }

    // 5. 原子写入（先写临时文件，避免原文件损坏）
    // 创建与原文件同目录的临时文件
    QTemporaryFile tempFile(fileInfo.path() + "/tmp_XXXXXX");
    tempFile.setAutoRemove(false); // 关闭自动删除，手动控制
    if (!tempFile.open()) {
        SendFileAccessMsgBoxSignal(QStringLiteral("创建临时文件失败：") + tempFile.errorString() , sched_mngt_message_box::MBT_ERROR);
        return false;
    }
    // 写入UTF-8数据
    qint64 writeSize = tempFile.write(byteArrayEncode);
    tempFile.close();

    if (writeSize != byteArrayEncode.size()) {
        SendFileAccessMsgBoxSignal(QStringLiteral("写入临时文件不完整：预期") + byteArrayEncode.size() + "字节，实际" + writeSize + "字节" , sched_mngt_message_box::MBT_ERROR);
        QFile::remove(tempFile.fileName()); // 删除无效临时文件
        return false;
    }

    // 6. 替换原文件（删除原文件 → 重命名临时文件）
    QString qstrBakName = qstrFilePath + QStringLiteral("_bak");
    if (!QFile::rename(qstrFilePath, qstrBakName)) {
        SendFileAccessMsgBoxSignal(QStringLiteral("删除原文件失败：") + qstrFilePath , sched_mngt_message_box::MBT_ERROR);
        QFile::remove(tempFile.fileName());
        return false;
    }
    if (!tempFile.rename(qstrFilePath)) {
        SendFileAccessMsgBoxSignal(QStringLiteral("重命名临时文件失败：") + tempFile.fileName() + " → " + qstrFilePath , sched_mngt_message_box::MBT_ERROR);
        QFile::rename(qstrFilePath, qstrFilePath);
        QFile::remove(tempFile.fileName());
        return false;
    }
    //删除备份
    QFile::remove(qstrBakName);
    //保存结果
    m_qstrPrivFindCheckResult += qstrFilePath + " (" +  qstrOldEncodingName + "->" + qstrNewEncodingName + ")\r\n";
    return true;
}


//删除中文，要求文件必须是utf8编码
bool thread_access_dir::DeleteZH(const QString& qstrFilePath
                                 , const QString &qstrOldEncodingName
                                 , const QByteArray& content) {
    //转为指定编码
    QByteArray byteArrayEncode = ChangeUncode(content
                                              , qstrOldEncodingName
                                              , QStringLiteral("UTF-8"));
    if (byteArrayEncode.isEmpty()) {
        m_qstrPrivFindCheckResult += QStringLiteral("!!!!!!!!!!!!!!!") + qstrFilePath + QStringLiteral("文件转码错误\r\n");
        return true;
    }

    //去除中文
    QString strTmp = QString::fromUtf8(byteArrayEncode);
    // 使用QTextCodec检测并移除中文
    for (int i = 0; i < strTmp.length();) {
        QChar ch = strTmp[i];
        if (ch.unicode() >= 0x4e00 && ch.unicode() <= 0x9fff ||   //简体字，繁体字
            ch.unicode() >= 0xf900 && ch.unicode() <= 0xfaff ||
             ch.unicode() >= 0xFF00 && ch.unicode() <= 0xFFEF   ) {    //此区间包含约512个兼容汉字，主要用于繁体中文、日文汉字和韩文汉字的兼容表示
            strTmp.remove(i, 1);
        } else {
            i++;
        }
    }

    QFileInfo fileInfo(qstrFilePath);
    if (!fileInfo.exists()) {
        return false;
    }
    if (!fileInfo.isFile()) {
        return false;
    }
    if (!fileInfo.isWritable()) {
        return false;
    }
    // 5. 原子写入（先写临时文件，避免原文件损坏）
    // 创建与原文件同目录的临时文件
    QTemporaryFile tempFile(fileInfo.path() + "/tmp_XXXXXX");
    tempFile.setAutoRemove(false); // 关闭自动删除，手动控制
    if (!tempFile.open()) {
        SendFileAccessMsgBoxSignal(QStringLiteral("创建临时文件失败：") + tempFile.errorString() , sched_mngt_message_box::MBT_ERROR);
        return false;
    }
    // 写入UTF-8数据
    QByteArray utf8ByteArray = strTmp.toUtf8();
    qint64 writeSize = tempFile.write(utf8ByteArray);
    tempFile.close();

    if (writeSize != utf8ByteArray.size()) {
        SendFileAccessMsgBoxSignal(QStringLiteral("写入临时文件不完整：预期") + utf8ByteArray.size() + "字节，实际" + writeSize + "字节" , sched_mngt_message_box::MBT_ERROR);
        QFile::remove(tempFile.fileName()); // 删除无效临时文件
        return false;
    }

    // 6. 替换原文件（删除原文件 → 重命名临时文件）
    QString qstrBakName = qstrFilePath + QStringLiteral("_bak");
    if (!QFile::rename(qstrFilePath, qstrBakName)) {
        SendFileAccessMsgBoxSignal(QStringLiteral("删除原文件失败：") + qstrFilePath , sched_mngt_message_box::MBT_ERROR);
        QFile::remove(tempFile.fileName());
        return false;
    }
    if (!tempFile.rename(qstrFilePath)) {
        SendFileAccessMsgBoxSignal(QStringLiteral("重命名临时文件失败：") + tempFile.fileName() + " → " + qstrFilePath , sched_mngt_message_box::MBT_ERROR);
        QFile::rename(qstrFilePath, qstrFilePath);
        QFile::remove(tempFile.fileName());
        return false;
    }
    //删除备份
    QFile::remove(qstrBakName);
    m_qstrPrivFindCheckResult += qstrFilePath + " (" +  qstrOldEncodingName + "->UTF-8 And Delete Chinese"  + ")\r\n";
    return true;
}


//将文件从old编码，转为指定编码
QByteArray thread_access_dir::ChangeUncode(const QByteArray& content
                        , const QString &qstrOldEncodingName
                        , const QString &qstrNewEncodingName) {
    QByteArray byteArrayResult;
    // 3. EncodingName → Unicode（QString）
    QTextCodec* textCodecSrc = QTextCodec::codecForName(qstrOldEncodingName.toUtf8());
    if (!textCodecSrc) {
        //SendFileAccessMsgBoxSignal(QStringLiteral("不支持的原文件%1编码（系统缺少对应编码库").arg(qstrOldEncodingName) , sched_mngt_message_box::MBT_ERROR);
        return byteArrayResult;
    }
    QString unicodeContent = textCodecSrc->toUnicode(content);

    // 4. Unicode → qstrNewEncodingName
    QTextCodec* textCodecDst = QTextCodec::codecForName(qstrNewEncodingName.toUtf8());
    if (!textCodecDst) {
        //(QStringLiteral("不支持的目标文件%1编码（系统缺少对应编码库").arg(qstrNewEncodingName) , sched_mngt_message_box::MBT_ERROR);
        return byteArrayResult;
    }
    byteArrayResult = textCodecDst->fromUnicode(unicodeContent);
    return byteArrayResult;
}

/////////////////////////////////////////////file_encode_check///////////////////////////////////////////////


file_encode_check::file_encode_check(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::file_encode_check)
{
    ui->setupUi(this);
    //绑定
    connect(ui->btn_start_check, &QPushButton::clicked, this, &file_encode_check::StartCheck);
    connect(ui->btn_encode, &QPushButton::clicked, this, &file_encode_check::StartEncode);
    connect(ui->btn_delete_zh, &QPushButton::clicked, this, &file_encode_check::StartDeleteZh);
    //支持的转码格式
    QStringList listStrCodecs;
    QList<QByteArray> codecs = QTextCodec::availableCodecs();
    foreach (const QByteArray &codec, codecs) {
        listStrCodecs.push_back(QString(codec));
    }
    ui->combo_box_convert->addItems(listStrCodecs);
    //默认转码格式是utf8
    ui->combo_box_convert->setCurrentText("UTF-8");
    //
    ui->line_edit_file_ext->setPlaceholderText(QStringLiteral("多个文件用;分割。例 .txt;.h;.cpp"));
}

file_encode_check::~file_encode_check()
{
    delete ui;
}

//开始进行文件的编码检测
void file_encode_check::StartCheck() {
    //先清空数据
    m_listStrFileExt.clear();
    //获取文件后缀
    QString qstrExt = ui->line_edit_file_ext->text();
    if (!qstrExt.isEmpty()) {
        m_listStrFileExt = qstrExt.split(";");
    }
    //格式化
    for (auto iter = m_listStrFileExt.begin(); iter != m_listStrFileExt.end();) {
        if (iter->isEmpty()) {
            iter = m_listStrFileExt.erase(iter);
        } else {
            *iter= iter->trimmed();//去掉两边的空格
            iter++;
        }
    }
    //得到路径
    QString qstrPath = ui->line_edit_path->text();
    if (qstrPath.isEmpty()) {
         sched_mngt_message_box::question(QStringLiteral("处理路径为空"), sched_mngt_message_box::MBT_INFO);
         return;
    }
    //启动处理线程
    m_pThreadAccessDir = new thread_access_dir(FAA_CheckEncode, qstrPath, m_listStrFileExt, "", this);
    connect(m_pThreadAccessDir, &thread_access_dir::OnDealFileItemSignal, this, &file_encode_check::OnDealFileItem, Qt::BlockingQueuedConnection);
    connect(m_pThreadAccessDir, &thread_access_dir::OnDealOverSignal, this, &file_encode_check::OnDealOver, Qt::BlockingQueuedConnection);
    connect(m_pThreadAccessDir, &thread_access_dir::OnFileAccessMsgBoxSignal, this, &file_encode_check::OnFileAccessMsgBox, Qt::BlockingQueuedConnection);
    m_pThreadAccessDir->start();
}

//开始转换文件的编码
void file_encode_check::StartEncode() {
    //先清空数据
    m_listStrFileExt.clear();
    //获取文件后缀
    QString qstrExt = ui->line_edit_file_ext->text();
    if (!qstrExt.isEmpty()) {
        m_listStrFileExt = qstrExt.split(";");
    }
    //格式化
    for (auto iter = m_listStrFileExt.begin(); iter != m_listStrFileExt.end();) {
        if (iter->isEmpty()) {
            iter = m_listStrFileExt.erase(iter);
        } else {
            *iter= iter->trimmed();//去掉两边的空格
            iter++;
        }
    }
    //得到路径
    QString qstrPath = ui->line_edit_path->text();
    if (qstrPath.isEmpty()) {
         sched_mngt_message_box::question(QStringLiteral("处理路径为空"), sched_mngt_message_box::MBT_INFO);
         return;
    }
    //得到指定的转码格式
    QString qstrEncodeName = ui->combo_box_convert->currentText();
    //检测是否存在此编码
    QTextCodec *qTextCodec = QTextCodec::codecForName(qstrEncodeName.toUtf8());
    if (NULL == qTextCodec) {
        sched_mngt_message_box::question(QStringLiteral("编码不支持") , sched_mngt_message_box::MBT_ERROR);
        return;
    }
    //启动处理线程
    m_pThreadAccessDir = new thread_access_dir(FAA_EncodeChange, qstrPath, m_listStrFileExt, qstrEncodeName, this);
    connect(m_pThreadAccessDir, &thread_access_dir::OnDealFileItemSignal, this, &file_encode_check::OnDealFileItem, Qt::BlockingQueuedConnection);
    connect(m_pThreadAccessDir, &thread_access_dir::OnDealOverSignal, this, &file_encode_check::OnDealOver, Qt::BlockingQueuedConnection);
    connect(m_pThreadAccessDir, &thread_access_dir::OnFileAccessMsgBoxSignal, this, &file_encode_check::OnFileAccessMsgBox, Qt::BlockingQueuedConnection);
    m_pThreadAccessDir->start();
}

//去除中文
void file_encode_check::StartDeleteZh() {
    //先清空数据
    m_listStrFileExt.clear();
    //获取文件后缀
    QString qstrExt = ui->line_edit_file_ext->text();
    if (!qstrExt.isEmpty()) {
        m_listStrFileExt = qstrExt.split(";");
    }
    //格式化
    for (auto iter = m_listStrFileExt.begin(); iter != m_listStrFileExt.end();) {
        if (iter->isEmpty()) {
            iter = m_listStrFileExt.erase(iter);
        } else {
            *iter= iter->trimmed();//去掉两边的空格
            iter++;
        }
    }
    //得到路径
    QString qstrPath = ui->line_edit_path->text();
    if (qstrPath.isEmpty()) {
         sched_mngt_message_box::question(QStringLiteral("处理路径为空"), sched_mngt_message_box::MBT_INFO);
         return;
    }
    //得到指定的转码格式
    QString qstrEncodeName = ui->combo_box_convert->currentText();
    //检测是否存在此编码
    QTextCodec *qTextCodec = QTextCodec::codecForName(qstrEncodeName.toUtf8());
    if (NULL == qTextCodec) {
        sched_mngt_message_box::question(QStringLiteral("编码不支持") , sched_mngt_message_box::MBT_ERROR);
        return;
    }
    //启动处理线程
    m_pThreadAccessDir = new thread_access_dir(FAA_DeleteZH, qstrPath, m_listStrFileExt, qstrEncodeName, this);
    connect(m_pThreadAccessDir, &thread_access_dir::OnDealFileItemSignal, this, &file_encode_check::OnDealFileItem, Qt::BlockingQueuedConnection);
    connect(m_pThreadAccessDir, &thread_access_dir::OnDealOverSignal, this, &file_encode_check::OnDealOver, Qt::BlockingQueuedConnection);
    connect(m_pThreadAccessDir, &thread_access_dir::OnFileAccessMsgBoxSignal, this, &file_encode_check::OnFileAccessMsgBox, Qt::BlockingQueuedConnection);
    m_pThreadAccessDir->start();

}

//槽， 收到当前正在处理的文件
void file_encode_check::OnDealFileItem(const QString &qstrFilePath) {
    ui->line_edit_current_ctl->setText(qstrFilePath);
}

//槽， 发送处理结果
void file_encode_check::OnDealOver(const QString &qstrResult) {
    ui->text_edit_result_log->setText(qstrResult);
    ui->line_edit_current_ctl->setText("");
    //提示
    sched_mngt_message_box::question(QStringLiteral("处理完成"), sched_mngt_message_box::MBT_INFO);
}

//槽， 处理过程的提示信息
void file_encode_check::OnFileAccessMsgBox(const QString &qstrMsg, sched_mngt_message_box::MsgBoxType nType) {
    sched_mngt_message_box::question(qstrMsg, nType);
}
