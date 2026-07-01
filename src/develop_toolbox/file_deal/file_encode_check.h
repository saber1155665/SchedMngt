#ifndef FILE_ENCODE_CHECK_H
#define FILE_ENCODE_CHECK_H

#include "sched_mngt_message_box.h"

#include <QWidget>
#include <functional>
#include <QTime>
#include <QThread>

namespace Ui {
class file_encode_check;
}


enum FileAccessAction {
    FAA_CheckEncode, //检测编码
    FAA_EncodeChange, //进行转码
    FAA_DeleteZH,       //删除中文

    FAA_COUNT
};

class thread_access_dir : public QThread {
    Q_OBJECT

public:
    thread_access_dir(FileAccessAction nAction, const QString &qstrDir,  QStringList listStrFileExt, const QString &qstrEncodeName, QObject *parent = nullptr);
    //线程函数
    void run() override;
    //发送当前正在处理的文件
    void SendDealFileItemSignal(const QString &qstrFilePath);
    //发送处理完成的结果信号
    void SendDealOverSignal(const QString &qstrResult);
    //发送处理过程的提示信息
    void SendFileAccessMsgBoxSignal(const QString &qstrMsg, sched_mngt_message_box::MsgBoxType nType);
    /**
     * @brief 定义文件处理回调函数类型
     * @param filePath 文件绝对路径
     * @param fileContent 文件内容（二进制格式，可按需转为文本）
     * @return 处理是否成功
     */
     typedef std::function<bool(const QString&, const QByteArray&)> FileProcessCallback;

    /**
     * @brief 递归处理文件/目录
     * @param path 目标路径（文件/目录）
     * @param callback 文件处理回调函数
     * @return 整体处理是否成功
     */
    bool ProcessFiles(const QString& qstrFilePath, const FileProcessCallback& callback);
    //回调函数
    bool ProcessFileCB(const QString&, const QByteArray&);
    //对文件统一编码进行保存
    bool ProcessFileEncode(const QString& qstrFilePath
                           , const QString &qstrOldEncodingName
                           , const QString &qstrNewEncodingName
                           , const QByteArray& content);
    //删除中文，要求文件必须是utf8编码
    bool DeleteZH(const QString& qstrFilePath
                  , const QString &qstrOldEncodingName
                  , const QByteArray& content);

private:
    //将文件从old编码，转为指定编码
    QByteArray ChangeUncode(const QByteArray& content
                            , const QString &qstrOldEncodingName
                            , const QString &qstrNewEncodingName);

signals:
   //槽， 收到当前正在处理的文件
    void OnDealFileItemSignal(const QString &qstrFilePath);
    //槽， 发送处理结果
    void OnDealOverSignal(const QString &qstrResult);
    //槽， 处理过程的提示信息
    void OnFileAccessMsgBoxSignal(const QString &qstrMsg, sched_mngt_message_box::MsgBoxType nType);
private:
    //处理模板
    QString m_qstrDir;
    //文件后缀
    QStringList m_listStrFileExt;
    //上次记录处理的时间
    QTime m_privTime;
    //记录结果
    QString m_qstrPrivFindCheckResult;
    //将文件转换成指定格式
    QString m_qstrEncodeName;
    //操作
    FileAccessAction m_nAction;
};

class file_encode_check : public QWidget
{
    Q_OBJECT

public:
    explicit file_encode_check(QWidget *parent = nullptr);
    ~file_encode_check();

private slots:
    //槽， 收到当前正在处理的文件
    void OnDealFileItem(const QString &qstrFilePath);
    //槽， 发送处理结果
    void OnDealOver(const QString &qstrResult);
    //槽， 处理过程的提示信息
    void OnFileAccessMsgBox(const QString &qstrMsg, sched_mngt_message_box::MsgBoxType nType);
private:
    //开始进行文件的编码检测
    void StartCheck();
    //开始转换文件的编码
    void StartEncode();
    //去除中文
    void StartDeleteZh();
private:
    //
    Ui::file_encode_check *ui;
    //文件后缀
    QStringList m_listStrFileExt;
    //处理线程
    thread_access_dir *m_pThreadAccessDir;
};

#endif // FILE_ENCODE_CHECK_H
