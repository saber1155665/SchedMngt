// Tcp.cpp: implementation of the CTcp class.
//
//////////////////////////////////////////////////////////////////////

/************************************************************************/
/*作用： 封装用来发送或者数据包的套接字，适用于windows, linux
                                                                     */
/************************************************************************/

#include "sched_mngt_tcp.h"

#if _MSC_VER  
	#include <winsock2.h>
	#include <stdio.h>
	#pragma comment(lib, "ws2_32.lib")
	#pragma warning(disable:4067)
#endif

#if  __linux__ 
	#include <stdio.h>
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <sys/time.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <unistd.h>
	#include <stdlib.h>
	#include <string.h>
	#include <arpa/inet.h>
	#include <errno.h>
	#include <time.h>
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//生成日志时候路径
#if _MSC_VER  
static const char *pLogPath = "tcp.log";
#endif

#if  __linux__ 
static const char *pLogPath = "tcp.log"
#endif


//发送文件时候，一次发送或者接受的套接字长度
#define ONCE_SEND_LENGTH 256

;//这里是因为linux可能报错

CTcp::CTcp()
{
	m_nSocket = 0;
	m_nListenSocket = 0;
	m_nTimeOut = 5000;



#if _MSC_VER  
	WSADATA wsaData;
	int nRet = 0;

	nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (nRet != 0 )
	{
		WriteLog("[WSAStartup] fail");
	}
#endif

}

CTcp::~CTcp()
{
	if (m_nSocket != 0)
	{
#if _MSC_VER  
		closesocket(m_nSocket);
#endif

#if  __linux__ 
		close(m_nSocket);
#endif
	}
	if (m_nListenSocket != 0)
	{
#if _MSC_VER  
		closesocket(m_nListenSocket);
#endif

#if  __linux__ 
		close(m_nListenSocket);
#endif
	}

#if _MSC_VER  
	WSACleanup();
#endif
}

void CTcp::InitSocket(unsigned int unTimeOut)
{
	m_nTimeOut = unTimeOut;
}

SOCKET CTcp::CreateSocket()
{
	SOCKET nSocket = -1;
	char buffer[256];

#if _MSC_VER  
	nSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (INVALID_SOCKET == nSocket)
	{
#if _DEBUG || DEBUG

		int nErrorCode = 0;
		nErrorCode = GetLastError();

		WriteLog("[CreateSocket] fail");

		sprintf(buffer, "%s %d", " GetLastError() : ", nErrorCode);
		WriteLog(buffer);
#endif

		return SOCKET_CREATE_FAIL;
	}
#endif

#if  __linux__ 
	nSocket = socket(AF_INET, SOCK_STREAM, 0);
	
	if (-1 == nSocket)
	{
#if _DEBUG || DEBUG
		WriteLog("[CreateSocket] fail");
		sprintf(buffer, "error number:%d,  error info: %s", errno, strerror(errno));
		WriteLog(buffer);
#endif
		return SOCKET_CREATE_FAIL;
	}
#endif
	
	return nSocket;

}


int CTcp::ConnectServer(const char *pszIP, int nPort)
{
	SOCKET nSocket = -1;
	struct sockaddr_in addr;
	int nRet = 0;
	char buffer[256];

	nSocket = CreateSocket();

	if (SOCKET_CREATE_FAIL == (int)nSocket)
	{
		return SOCKET_CREATE_FAIL;
	}
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(nPort);
	addr.sin_addr.s_addr = inet_addr(pszIP);

#if _MSC_VER  
    // 2. 将套接字设为非阻塞模式
    u_long nonBlock = 1;
    if (ioctlsocket(nSocket, FIONBIO, &nonBlock) == SOCKET_ERROR) {
        closesocket(nSocket);
        return INVALID_SOCKET;
    }


	nRet = connect(nSocket, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
    if (0 == nRet) {
        nonBlock = 0;
        ioctlsocket(nSocket, FIONBIO, &nonBlock);
    } else {
        // 5. 非阻塞connect返回WSAEWOULDBLOCK表示正在连接，需用select检测
        int lastErr = WSAGetLastError();
        if (lastErr != WSAEWOULDBLOCK) {
            closesocket(nSocket);
            return INVALID_SOCKET;
        }
        // 6. 用select监听套接字可写事件（检测连接完成）
        fd_set writeSet;
        FD_ZERO(&writeSet);
        FD_SET(nSocket, &writeSet);

        timeval timeout{};
        timeout.tv_sec = m_nTimeOut / 1000;          // 秒
        timeout.tv_usec = (m_nTimeOut % 1000) * 1000; // 微秒

        // select返回值：>0=有事件，0=超时，<0=失败
        nRet = select(0, NULL, &writeSet, NULL, &timeout);
        if (nRet == 0) {
            closesocket(nSocket);
            return CONNECT_FAIL;
        } else if (nRet == SOCKET_ERROR) {
            closesocket(nSocket);
            return CONNECT_FAIL;
        }

        // 7. 检测连接是否真的成功（select返回可写不代表连接成功）
        int errorCode = 0;
        int errorLen = sizeof(errorCode);
        getsockopt(nSocket, SOL_SOCKET, SO_ERROR, (char*)&errorCode, &errorLen);
        if (errorCode != 0) {
            closesocket(nSocket);
            return CONNECT_FAIL;
        }

        // 8. 恢复为阻塞模式（可选，业务若需要非阻塞可跳过）
        nonBlock = 0;
        if (ioctlsocket(nSocket, FIONBIO, &nonBlock) == SOCKET_ERROR) {
            return CONNECT_FAIL;
        }
    }
#endif

#if  __linux__ 
	
	nRet = connect(nSocket, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
	
	if (-1 == nRet)
	{
#if _DEBUG || DEBUG
		sprintf(buffer, "[ConnectServer] fail IP:%s", pszIP);
		WriteLog(buffer);
		sprintf(buffer, "error number:%d,  error info: %s", errno, strerror(errno));
		WriteLog(buffer);
#endif
		return CONNECT_FAIL;
	}
#endif

	m_nSocket = nSocket;
	return CONNECT_SUCCESS;
}



int CTcp::CreateServer(int nPort, bool bSync, char *pszBindIP)
{
	int nRet;
	SOCKET nSocket;
	struct sockaddr_in addr;
	char pBuffer[256];


	nSocket = CreateSocket();
	if (SOCKET_CREATE_FAIL == nSocket)
	{
		return SOCKET_CREATE_FAIL;
	}

	addr.sin_family = AF_INET;
	if (NULL == pszBindIP)
	{
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		addr.sin_addr.s_addr = inet_addr(pszBindIP);
	}
	addr.sin_port = htons(nPort);


	//绑定套接字
#if _MSC_VER  
	
	nRet = bind(nSocket, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
	
	if (SOCKET_ERROR == nRet)
	{
#if _DEBUG || DEBUG
		int nErrorCode = 0;
		nErrorCode = GetLastError();
		sprintf(pBuffer, "[CreateServer] fail on bind!port:%d", nPort);
		WriteLog(pBuffer);

		sprintf(pBuffer, " GetLastError() : %d", nErrorCode);
		WriteLog(pBuffer);
#endif
		return CREATE_SERVER_BIND_FAIL;
	}
#endif


#if  __linux__ 
	
	nRet = bind(nSocket, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
	
	if (-1 == nRet)
	{
#if _DEBUG || DEBUG
		sprintf(pBuffer, "[CreateServer] fail on bind!port:%d", nPort);
		WriteLog(pBuffer);
		sprintf(pBuffer, "error number:%d,  error info: %s", errno, strerror(errno));
		WriteLog(pBuffer);
#endif
		return CREATE_SERVER_BIND_FAIL;
	}
#endif


	//侦听套接字
#if _MSC_VER  
	
	nRet = listen(nSocket, SOMAXCONN);//windows最大的其实也是5
	
	if (SOCKET_ERROR == nRet)
	{
#if _DEBUG || DEBUG
		int nErrorCode = 0;
		nErrorCode = GetLastError();
		sprintf(pBuffer, "[CreateServer] fail on listen! socket:%d", nSocket);
		WriteLog(pBuffer);
		sprintf(pBuffer, " GetLastError() : %d", nErrorCode);
		WriteLog(pBuffer);
#endif
		return CREATE_SERVER_LISTEN_FAIL;
	}
	if (true == bSync)
	{
		int iMode = 1; // 0阻塞， 非0不阻塞
		ioctlsocket(nSocket, FIONBIO, (u_long *)&iMode);
	}
#endif

#if  __linux__ 
	
	nRet = listen(nSocket, 5);//linux最大处理连接数目常用是5
	
	if (-1 == nRet)
	{
#if _DEBUG || DEBUG
		sprintf(pBuffer, "[CreateServer] fail on listen! socket:%d", nSocket);
		WriteLog(pBuffer);
		sprintf(pBuffer, "error number:%d,  error info: %s", errno, strerror(errno));
		WriteLog(pBuffer);
#endif
		return CREATE_SERVER_LISTEN_FAIL;
	}
	if (true == bSync)
	{
		int flags = fcntl(nSocket, F_GETFL, 0);
		fnctl(nSocket, F_SETFL, flags | O_NONBLOCK);
	}

#endif

	//保存侦听套接字
	m_nListenSocket = nSocket;

	return CREATE_SERVER_SUCCESS;
}


int CTcp::AcceptConnect()
{
	char pBuffer[256];
	int nRet = 0;

	//接受连接请求
	struct sockaddr_in clientAddr;
	int nLength = 0;
	memset((void *)&clientAddr, '\0', sizeof(struct sockaddr_in));

	if (m_nListenSocket <= 0)
	{
		WriteLog("[AcceptConnect] on listened Socket fail");
		return SERVER_ACCEPT_FAIL;
	}

	nLength = sizeof(struct sockaddr_in);
_ACCEPT:
#if _MSC_VER  
	
	nRet = accept(m_nListenSocket, (struct sockaddr *)&clientAddr, &nLength);//windows最大的其实也是5
	
	if (INVALID_SOCKET == nRet)
	{
#if _DEBUG || DEBUG
		int nErrorCode  = 0;
		nErrorCode = GetLastError();
		sprintf(pBuffer, "[AcceptConnect] fail on accept! socket:%d", m_nListenSocket);
		WriteLog(pBuffer);
		
		sprintf(pBuffer, "GetLastError() : %d", nErrorCode);
		WriteLog(pBuffer);
#endif
		return SERVER_ACCEPT_FAIL;
	}
	else if (0 == nRet)
	{
		Sleep(500);
		goto _ACCEPT;
	}
	else
	{
#if _DEBUG || DEBUG
		sprintf(pBuffer, "[AcceptConnect] client IP: %d, port: %d", clientAddr.sin_addr.S_un.S_addr,clientAddr.sin_port);
		WriteLog(pBuffer);
#endif
	}
#endif
	
#if  __linux__ 
	
	nRet = accept(m_nListenSocket, (struct sockaddr *)&clientAddr, (socklen_t *)&nLength);
	
	if (-1 == nRet)
	{
#if _DEBUG || DEBUG
		sprintf(pBuffer, "[AcceptConnect] fail on accept! socket:%d", m_nSocket);
		WriteLog(pBuffer);
		sprintf(pBuffer, "error number:%d,  error info: %s", errno, strerror(errno));
		WriteLog(pBuffer);
#endif
		return SERVER_ACCEPT_FAIL;
	}
	else
	{
#if _DEBUG || DEBUG
		sprintf(pBuffer, "[AcceptConnect] SUCCESS! client IP: %s, port: %d", inet_ntoa(clientAddr.sin_addr), clientAddr.sin_port);
		WriteLog(pBuffer);
		sprintf(pBuffer, "error number:%d,  error info: %s", errno, strerror(errno));
		WriteLog(pBuffer);
#endif
	}
#endif

	//保存用来发送数据的套接字
	//m_nSocket = nRet;
	return nRet;
}

void CTcp::SetSocket(SOCKET s)
{
	m_nSocket = s;
}

int CTcp::SendData(char *dataBuffer, int nSize)
{
	int nRet = 0;
	unsigned int byteSendSuccess = 0;
	char pBuffer[256];

	memset(pBuffer, '\0', 256);

	if (NULL == dataBuffer || 0 == nSize)
	{
		return SENDDATA_ERROR_NO_INFO;
	}

	//开始发送数据包
#if _MSC_VER  
	
	nRet = send(m_nSocket, dataBuffer, nSize, 0);
	
	if (SOCKET_ERROR == nRet)
	{
#ifdef _DEBUG || DEBUG
		int nErrorCode = 0;
		nErrorCode = GetLastError();
		sprintf(pBuffer, "Send data fail!\n");
		WriteLog(pBuffer);
		sprintf(pBuffer, "GetLastError() : %d", nErrorCode);
		WriteLog(pBuffer);

#endif
		return SENDDATA_ERROR_FAIL;
	}
#endif

#if  __linux__ 
	
	nRet = write(m_nSocket, dataBuffer, nSize);
	
	if (-1 == nRet)
	{
#if _DEBUG || DEBUG
		sprintf(pBuffer, "Send data fail!\n");
		WriteLog(pBuffer);
		sprintf(pBuffer, "error number:%d,  error info: %s", errno, strerror(errno));
		WriteLog(pBuffer);
#endif
		return SENDDATA_ERROR_FAIL;
	}
#endif
	
	//得到实际发送的长度
	byteSendSuccess = nRet;

#if _DEBUG || DEBUG
	sprintf(pBuffer, "send data success! length: %d", byteSendSuccess);
#endif

	return byteSendSuccess;
}

int CTcp::RecvData(char *dataBuffer, int nSize)
{
	unsigned int nReturnDataLength = -1;
	char pBuffer[256];
	
#if _MSC_VER  
	
	nReturnDataLength = recv(m_nSocket, dataBuffer, nSize, 0);
	
	if (SOCKET_ERROR == nReturnDataLength)
	{
#if _DEBUG || DEBUG
		sprintf(pBuffer, "Recv data fail!\n");
		WriteLog(pBuffer);
#endif
		return RECVDATA_ERROR_FAIL;
	}

	if (0 == nReturnDataLength)
	{
		return RECVDATA_NO_DATA;
	}
#endif

	
#if  __linux__ 
	
	nReturnDataLength = read(m_nSocket, dataBuffer, nSize);
	
	if (-1 == nReturnDataLength)
	{
#if _DEBUG || DEBUG
		sprintf(pBuffer, "Recv data fail!\n");
		WriteLog(pBuffer);
		sprintf(pBuffer, "error number:%d,  error info: %s", errno, strerror(errno));
		WriteLog(pBuffer);
#endif
		return RECVDATA_ERROR_FAIL;
	}
	
	if (0 == nReturnDataLength)
	{
		return RECVDATA_NO_DATA;
	}
#endif

	return nReturnDataLength;
}


int CTcp::SendFile(char *pszFilePath)
{
	FILE *pFile = NULL;
	char buffer[256];
	int nLength = 0;//读取到的文件的长度
	int bSendComplete = 0;
	int nRet = 0;
	int nFailNumber = 0;
	int nSuccessNumber = 0;
	int nBeginTimeSend = 0;
	int nEndTimeSend = 0;
	unsigned int nTimeSend = 0;//已经发送的时间
	int nSentLength = 0;//已经发送了的长度

		//因为在发送过程中，可能存在修改了时间情况，为了统一，我们使用开机到现在的时间
#if _MSC_VER  
		;
	nBeginTimeSend = GetTickCount();
#endif
	
#if  __linux__ 
	;
	struct timeval te;
	gettimeofday(&te, NULL);
	nBeginTimeSend = te.tv_usec;
#endif
		
	if (NULL == pszFilePath)
	{
		return SENDFILE_FAIL;
	}

	pFile = fopen(pszFilePath, "r");
	if (NULL == pFile)
	{
#if _DEBUG || DEBUG
		WriteLog("[SendFile] Open File fail");
		sprintf(buffer, "%s", pszFilePath);
		WriteLog(buffer);
#endif
		return SENDFILE_FAIL;
	}
	
	while (1)
	{
		//从文件中读取1 * 256个字节
		nLength = fread(buffer, 1, 256, pFile);
		if (0 == nLength)
		{		
			break;
		}
		
		bSendComplete = 0;
		while (!bSendComplete)
		{
			
			nRet = SendData(buffer + nSentLength, nLength);
			nSentLength += nRet;//保存已经发送了的长度

			//没有发送完全
			if (nLength != nSentLength)
			{
#if DEBUG || _DEBUG
				WriteLog("SendData fail!");
				nFailNumber++;//用来统计不能一次发送256字节的原因
#endif

#if _MSC_VER  
				;
				nEndTimeSend = GetTickCount();
#endif
				
#if  __linux__ 
				struct timeval te;
				gettimeofday(&te, NULL);
				nEndTimeSend = te.tv_usec;
#endif
				nTimeSend = nEndTimeSend - nBeginTimeSend;
				if (nTimeSend >= m_nTimeOut)//发送超时
				{
					char errorBuffer[256];

					memset(errorBuffer, '\0', 256);
#if DEBUG || _DEBUG
					WriteLog("[SendFile] Send file time out");
#if _MSC_VER  
					sprintf(errorBuffer," Info:%d", GetLastError());
					WriteLog(errorBuffer);
#endif
#if  __linux__ 
					sprintf(errorBuffer, "error number:%d,  error info: %s", errno, strerror(errno));
					WriteLog(errorBuffer);
#endif
#endif
					return ERROR_TIME_OUT;
				}
				else
				{

				}

			}
			else
			{
				bSendComplete = 1;
			}

/*
#if _MSC_VER  
			Sleep(100);
#endif
#if  __linux__ 
			sleep(100);
#endif*/
		}
		nRet = 0;
		bSendComplete = 0;
	}
#if _DEBUG || DEBUG
	sprintf(buffer, "send data fail number:%d", nFailNumber);
	WriteLog(buffer);
	WriteLog("Send file Success");
	sprintf(buffer, "%s", pszFilePath);
	WriteLog(buffer);
#endif

	return SENDFILE_SUCCESS;
}

int CTcp::RecvFile(char *pszFilePath)
{
	FILE *pFile = NULL;
	char buffer[256];
	int nLength = 0;
	int bSendComplete = 0;
	int nRet = 0;
	int nFailNumber = 0;
	int nSuccessNumber = 0;
	int nBeginTimeSend = 0;
	int nEndTimeSend = 0;
	unsigned int nTimeSend = 0//已经发送的时间

#if _MSC_VER  
		;
	nBeginTimeSend = GetTickCount();
#endif
	
#if  __linux__ 
	;
	struct timeval te;
	gettimeofday(&te, NULL);
	nBeginTimeSend = te.tv_usec;

#endif

	if (NULL == pszFilePath)
	{
		return SENDFILE_FAIL;
	}
	
	pFile = fopen(pszFilePath, "ab+");
	if (NULL == pFile)
	{
#if _DEBUG || DEBUG
		WriteLog("[RecvFile] Open or Create File fail");
		sprintf(buffer, "%s", pszFilePath);
		WriteLog(buffer);
#endif
		return RECVFILE_FAIL;
	}
	
	while (1)
	{
		nRet = RecvData(buffer, ONCE_SEND_LENGTH);

		if (0 == nRet)
		{
#if _MSC_VER  
			nEndTimeSend = GetTickCount();
#endif
			
#if  __linux__ 
			;
			struct timeval te;
			gettimeofday(&te, NULL);
			nEndTimeSend = te.tv_usec;	
#endif
			nTimeSend = (nEndTimeSend - nBeginTimeSend);
			if (nTimeSend >= m_nTimeOut)//接受文件超时
			{
#if DEBUG || _DEBUG
				char errorBuffer[256];
				memset(errorBuffer, '\0', 256);
				WriteLog("[RecvFile] Recv file time out");
#if _MSC_VER  
				sprintf(errorBuffer," Info:%d", GetLastError());
				WriteLog(errorBuffer);
#endif
#if  __linux__ 
				sprintf(errorBuffer, "error number:%d,  error info: %s", errno, strerror(errno));
				WriteLog(errorBuffer);
#endif
#endif
				break;
			}
		}

		fwrite(buffer, 1, nRet, pFile);
	}
#if _DEBUG || DEBUG
	sprintf(buffer, "Receive File Success");
	WriteLog(buffer);
	sprintf(buffer, "%s", pszFilePath);
	WriteLog(buffer);
#endif
	
	return RECVFILE_SUCCESS;
}

int CTcp::Send(char *dataBuffer, int nSize)
{
	int nBeginTimeSend = 0;
	int nEndTimeSend = 0;
	unsigned int nTimeSend = 0;//已经发送的时间
	int nRet = 0;	
	int nSentLength = 0;

	if (NULL == dataBuffer)
	{
		return SENDDATA_ERROR_NO_INFO;
	}

//开始发送时间
#if _MSC_VER  
		;
	nBeginTimeSend = GetTickCount();
#endif
	
#if  __linux__ 
	;
	struct timeval te;
	gettimeofday(&te, NULL);
	nBeginTimeSend = te.tv_usec;
	
#endif

	while (1)
	{
		nRet = this->SendData(dataBuffer + nSentLength, nSize);
		nSentLength += nRet;
		if (nSentLength == nSize)
		{
			return nRet;
		}
		else
		{

			//现在时间
#if _MSC_VER  
			;
			nEndTimeSend = GetTickCount();
#endif
			
#if  __linux__ 
			;
			struct timeval te;
			gettimeofday(&te, NULL);
			nEndTimeSend = te.tv_usec;
			
#endif
			nTimeSend = nEndTimeSend - nBeginTimeSend;
			if (nTimeSend >= m_nTimeOut)
			{

#if DEBUG || _DEBUG
				char errorBuffer[256];
				memset(errorBuffer, '\0', 256);
				WriteLog("[Send] Send data time out");
#if _MSC_VER  
				sprintf(errorBuffer," Info:%d", GetLastError());
				WriteLog(errorBuffer);
#endif
#if  __linux__ 
				sprintf(errorBuffer, "error number:%d,  error info: %s", errno, strerror(errno));
				WriteLog(errorBuffer);
#endif
#endif
				return ERROR_TIME_OUT;
			}
		}
	}

}


int CTcp::Recv(char *dataBuffer, int nSize)
{

	unsigned int nTimeSend = 0;//已经接收的时间
	int nRet = 0;	
	int nSentLength = 0;
	int nBeginTimeSend = 0;
	int nEndTimeSend = 0;
	
	if (NULL == dataBuffer)
	{
		return 0;
	}
	
	//开始接收时间
#if _MSC_VER  
	;
	nBeginTimeSend = GetTickCount();
#endif
	
#if  __linux__ 
	;
	struct timeval te;
	gettimeofday(&te, NULL);
	nBeginTimeSend = te.tv_usec;
	
#endif

	while (1)
	{
		nRet = this->RecvData(dataBuffer, nSize);
		
		if (nRet > 0)
		{
			return nRet;
		}
		else if (RECVDATA_NO_DATA == nRet)//接收到的数据包为空
		{
			
			//现在时间
#if _MSC_VER  
			;
			nEndTimeSend = GetTickCount();
#endif
			
#if  __linux__ 
			;
			struct timeval te;
			gettimeofday(&te, NULL);
			nEndTimeSend = te.tv_usec;
			
#endif
			nTimeSend = nEndTimeSend - nBeginTimeSend;
			if (nTimeSend >= m_nTimeOut)
			{
				
#if DEBUG || _DEBUG
				char errorBuffer[256];
				memset(errorBuffer, '\0', 256);
				WriteLog("[Recv] Recv data time out");
#if _MSC_VER  
				sprintf(errorBuffer," Info:%d", GetLastError());
				WriteLog(errorBuffer);
#endif
#if  __linux__ 
				sprintf(errorBuffer, "error number:%d,  error info: %s", errno, strerror(errno));
				WriteLog(errorBuffer);
#endif
#endif
				return ERROR_TIME_OUT;
			}
		}
		else
		{
			WriteLog("[Recv] unknown error");
			return RECVDATA_ERROR_FAIL;
		}
	}
}

void CTcp::WriteLog(const char *pszErrorMsg)
{
	FILE *pFile = NULL;
    char buffer[256] = { 0 };
	int nLength = 0, nRet = 0;
	int nFileIsOpen = 1;
	
	pFile = fopen(pLogPath, "ab+");

	if (NULL == pFile)
	{
		return;
	}

	fseek(pFile, 0, SEEK_END);
    sprintf(buffer, "info: %s \n", pszErrorMsg);

	nLength = strlen(buffer);
	nRet = fwrite(buffer, 1, nLength, pFile); 

	fclose(pFile);
	return;
}




