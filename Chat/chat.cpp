#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#pragma comment(lib,"WSock32.lib")
//add
void ChatServer(void) {
	SOCKET listen_s;
	SOCKET s;
	SOCKADDR_IN saddr;
	SOCKADDR_IN from;
	int fromlen;
	u_short uport;

	/*input port*/
	printf("使用するポート番号-->　");
	scanf_s("%hd", &uport);
	fflush(stdin);

	//open listen socket
	listen_s = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_s == INVALID_SOCKET) {
		printf("リスンソケットオープンエラー");
		WSACleanup();
		return;
	}
	printf("リスンソケットをオープンしました\n");

	//add socket name
	memset(&saddr, 0, sizeof(SOCKADDR_IN));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(uport);
	saddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(listen_s, (struct sockaddr*)&saddr, sizeof(saddr)) == SOCKET_ERROR) {
		printf("bindエラー");
		closesocket(listen_s);
		return;
	}
	printf("bind成功です\n");

	//クライアントからの接続持ちの状態にする
	if (listen(listen_s, 0) == SOCKET_ERROR) {
		printf("listenエラー");
		closesocket(listen_s);
		return;
	}
	printf("listen成功です\n");

	//接続を待機する
	printf("acceptで待機します\n");
	fromlen = (int)sizeof(from);
	s = accept(listen_s, (SOCKADDR*)&from, &fromlen);
	if (s == INVALID_SOCKET) {
		printf("acceptエラー");
		closesocket(listen_s);
		return;
	}

	printf("%sが接続してきました\n", inet_ntoa(from.sin_addr));
	printf("accept関数成功です\n");

	//close listen socket
	closesocket(listen_s);

	printf("会話開始\n");

	while (1)
	{
		//送受信バッファ
		int nRcv;
		char szBuf[1024];
		printf("送受信を待っています\n");

		//receive data
		nRcv = recv(s, szBuf, sizeof(szBuf) - 1, 0);
		if (nRcv == SOCKET_ERROR)
		{
			printf("recvエラー\n");
			break;
		}
		szBuf[nRcv] = '\0';

		//接続切断のチェック
		if (strcmp(szBuf, "c_end") == 0)
		{
			printf("クライアントが接続切断しました\n");
			break;
		}

		//receive message
		printf("受信　--> %s\n", szBuf);

		//input message
		printf("送信 --> ");

		scanf_s("%s", szBuf, 1024);
		fflush(stdin);

		//send message
		send(s, szBuf, (int)strlen(szBuf), 0);

		//サーバー側から切断する
		if (strcmp(szBuf, "s_end") == 0)
		{
			break;
		}
	}

	//close socket
	closesocket(s);
}

void ChatClient(void) {
	SOCKET s;
	SOCKADDR_IN saddr;
	u_short uport;
	char szServer[1024];
	HOSTENT* lpHost;
	unsigned int addr;

	/*input port*/
	printf("使用するポート番号-->　");
	scanf_s("%hd", &uport);
	fflush(stdin);

	/*　サーバー名またサーバーのIP入力*/
	printf("サーバー名　-->");
	scanf_s("%s", szServer, 1024);
	fflush(stdin);

	/* open socket */
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)
	{
		printf("ソケットオープンエラー");
		return;
	}

	/* サーバーを名前で取得する　*/
	lpHost = gethostbyname(szServer);
	if (lpHost == NULL)
	{
		//by address
		addr = inet_addr(szServer);
		lpHost = gethostbyaddr((char*)&addr, 4, AF_INET);
	}

	if (lpHost == NULL)
	{
		printf("Hostが見つかりません\n");
		closesocket(s);
		return;
	}

	/* connect */
	memset(&saddr, 0, sizeof(SOCKADDR_IN));
	saddr.sin_family = lpHost->h_addrtype;
	saddr.sin_port = htons(uport);
	saddr.sin_addr.s_addr = *((u_long*)lpHost->h_addr);

	if (connect(s, (SOCKADDR*)&saddr, sizeof(saddr)) == SOCKET_ERROR)
	{
		printf("サーバーと接続できませんでした\n");
		closesocket(s);
		return;
	}
	printf("サーバーと接続できました\n");

	while (1)
	{
		//送受信バッファ
		int nRcv;
		char szBuf[1024];

		/* 送信データの入力 */
		printf("送信 --> ");
		scanf_s("%s", szBuf, 1024);
		fflush(stdin);

		/* データを送信する */
		send(s, szBuf, (int)strlen(szBuf), 0);

		/* 接続切断するか */
		if (strcmp(szBuf, "c_end") == 0)
		{
			break;
		}

		/* サーバーからデータを受信する */
		nRcv = recv(s, szBuf, sizeof(szBuf) - 1, 0);
		if (nRcv == SOCKET_ERROR) {
			printf("受信エラー\n");
			break;
		}
		szBuf[nRcv] = '\0';

		if (strcmp(szBuf, "s_end") == 0) {
			printf("サーバーが接続切断しました\n");
			break;
		}

		/* 受信データの表示 */
		printf("受信 --> %s\n", szBuf);
	}

	//close
	closesocket(s);
}

void main(void) {
	WSADATA wsaData;
	int mode;

	/* WinSockの初期化 */
	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
		//init error
		printf("WinSockの初期化に失敗しました\n");
		return;
	}

	/* サーバーか？　クライアントか？ */
	printf("サーバーなら0を入力　クライアントなら1を入力 --> ");
	scanf_s("%d", &mode);
	fflush(stdin);

	if (mode == 0) {
		ChatServer();
	}
	else {
		ChatClient();
	}

	WSACleanup();
}