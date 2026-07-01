// Tcp.h: interface for the CTcp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCP_H__C1EE53E6_1984_4560_BEAB_FE04B0EE3698__INCLUDED_)
#define AFX_TCP_H__C1EE53E6_1984_4560_BEAB_FE04B0EE3698__INCLUDED_

#ifdef _MSC_VER

#include <winsock2.h>
#include <stdio.h>

#endif


//发送超时
#define ERROR_TIME_OUT -2



//创建套接字失败
#define SOCKET_CREATE_FAIL -1

//连接目标IP失败
#define CONNECT_FAIL -1
//连接目标IP成功
#define CONNECT_SUCCESS 1


//创建服务器失败
#define CREATE_SERVER_BIND_FAIL -1
//侦听服务器失败
#define CREATE_SERVER_LISTEN_FAIL -2
//创建服务器成功
#define CREATE_SERVER_SUCCESS 1

//发送数据包时候，数据包为空
#define SENDDATA_ERROR_NO_INFO -1
//发送数据包失败
#define SENDDATA_ERROR_FAIL -2
//发送数据包成功
#define SENDDATA_SUCCESS 1


//得到数据包失败
#define RECVDATA_ERROR_FAIL -1
//接受到的数据包为空
#define RECVDATA_NO_DATA 0
//发送数据包成功
#define RECVDATA_SUCCESS 1

//服务器接收连接失败
#define SERVER_ACCEPT_FAIL 0
//服务器接收连接成功
#define SERVER_ACCEPT_SUCCESS 1


//发送文件失败
#define SENDFILE_FAIL 0
//发送文件成功
#define SENDFILE_SUCCESS 1


//接收文件失败
#define RECVFILE_FAIL 0
//接收文件成功
#define RECVFILE_SUCCESS 1


#ifndef SOCKET
//typedef unsigned int SOCKET;
#endif

class CTcp
{
public:
	CTcp();
	virtual ~CTcp();

	//设置超时
	void InitSocket(unsigned int unTimeOut);

	//发送数据包
	int Send(char* dataBuffer, int nSize);

	//接受数据包
	int Recv(char* dataBuffer, int nSize);

	//连接
    int ConnectServer(const char* pszIP, int nPort);

	//创建服务器 nPort:绑定的端口 bSync:是否异步侦听 pszBindIP:绑定对应的IP(多个网卡时候，我们可以指定某个网卡)
	int CreateServer(int nPort, bool bSync, char* pszBindIP);

	//创建接收套接字
	int AcceptConnect();

	//作为服务端接受套接字时候，传输数据。
	void SetSocket(SOCKET s);;

	//发送文件
	int SendFile(char* pszFilePath);

	//接收文件
	int RecvFile(char* pszFilePath);



private:

	//打印Tcp通讯的日志
    void WriteLog(const char* pszErrorMsg);

	//创建套接字
	SOCKET CreateSocket();

	//发送数据,封装所有的系统的发送数据
	int SendData(char* dataBuffer, int nSize);

	//接受数据,封装所有的系统的接收数据
	int RecvData(char* dataBuffer, int nSize);


	unsigned int m_nTimeOut;//保存系统超时的时间长短

	SOCKET m_nSocket;//保存得到套接字，用来发送或者接受数据
	SOCKET m_nListenSocket;//用来保存侦听套接字
};

#endif // !defined(AFX_TCP_H__C1EE53E6_1984_4560_BEAB_FE04B0EE3698__INCLUDED_)
