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
	printf("�g�p����|�[�g�ԍ�-->�@");
	scanf_s("%hd", &uport);
	fflush(stdin);

	//open listen socket
	listen_s = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_s == INVALID_SOCKET) {
		printf("���X���\�P�b�g�I�[�v���G���[");
		WSACleanup();
		return;
	}
	printf("���X���\�P�b�g���I�[�v�����܂���\n");

	//add socket name
	memset(&saddr, 0, sizeof(SOCKADDR_IN));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(uport);
	saddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(listen_s, (struct sockaddr*)&saddr, sizeof(saddr)) == SOCKET_ERROR) {
		printf("bind�G���[");
		closesocket(listen_s);
		return;
	}
	printf("bind�����ł�\n");

	//�N���C�A���g����̐ڑ������̏�Ԃɂ���
	if (listen(listen_s, 0) == SOCKET_ERROR) {
		printf("listen�G���[");
		closesocket(listen_s);
		return;
	}
	printf("listen�����ł�\n");

	//�ڑ���ҋ@����
	printf("accept�őҋ@���܂�\n");
	fromlen = (int)sizeof(from);
	s = accept(listen_s, (SOCKADDR*)&from, &fromlen);
	if (s == INVALID_SOCKET) {
		printf("accept�G���[");
		closesocket(listen_s);
		return;
	}

	printf("%s���ڑ����Ă��܂���\n", inet_ntoa(from.sin_addr));
	printf("accept�֐������ł�\n");

	//close listen socket
	closesocket(listen_s);

	printf("��b�J�n\n");

	while (1)
	{
		//����M�o�b�t�@
		int nRcv;
		char szBuf[1024];
		printf("����M��҂��Ă��܂�\n");

		//receive data
		nRcv = recv(s, szBuf, sizeof(szBuf) - 1, 0);
		if (nRcv == SOCKET_ERROR)
		{
			printf("recv�G���[\n");
			break;
		}
		szBuf[nRcv] = '\0';

		//�ڑ��ؒf�̃`�F�b�N
		if (strcmp(szBuf, "c_end") == 0)
		{
			printf("�N���C�A���g���ڑ��ؒf���܂���\n");
			break;
		}

		//receive message
		printf("��M�@--> %s\n", szBuf);

		//input message
		printf("���M --> ");

		scanf_s("%s", szBuf, 1024);
		fflush(stdin);

		//send message
		send(s, szBuf, (int)strlen(szBuf), 0);

		//�T�[�o�[������ؒf����
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
	printf("�g�p����|�[�g�ԍ�-->�@");
	scanf_s("%hd", &uport);
	fflush(stdin);

	/*�@�T�[�o�[���܂��T�[�o�[��IP����*/
	printf("�T�[�o�[���@-->");
	scanf_s("%s", szServer, 1024);
	fflush(stdin);

	/* open socket */
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)
	{
		printf("�\�P�b�g�I�[�v���G���[");
		return;
	}

	/* �T�[�o�[�𖼑O�Ŏ擾����@*/
	lpHost = gethostbyname(szServer);
	if (lpHost == NULL)
	{
		//by address
		addr = inet_addr(szServer);
		lpHost = gethostbyaddr((char*)&addr, 4, AF_INET);
	}

	if (lpHost == NULL)
	{
		printf("Host��������܂���\n");
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
		printf("�T�[�o�[�Ɛڑ��ł��܂���ł���\n");
		closesocket(s);
		return;
	}
	printf("�T�[�o�[�Ɛڑ��ł��܂���\n");

	while (1)
	{
		//����M�o�b�t�@
		int nRcv;
		char szBuf[1024];

		/* ���M�f�[�^�̓��� */
		printf("���M --> ");
		scanf_s("%s", szBuf, 1024);
		fflush(stdin);

		/* �f�[�^�𑗐M���� */
		send(s, szBuf, (int)strlen(szBuf), 0);

		/* �ڑ��ؒf���邩 */
		if (strcmp(szBuf, "c_end") == 0)
		{
			break;
		}

		/* �T�[�o�[����f�[�^����M���� */
		nRcv = recv(s, szBuf, sizeof(szBuf) - 1, 0);
		if (nRcv == SOCKET_ERROR) {
			printf("��M�G���[\n");
			break;
		}
		szBuf[nRcv] = '\0';

		if (strcmp(szBuf, "s_end") == 0) {
			printf("�T�[�o�[���ڑ��ؒf���܂���\n");
			break;
		}

		/* ��M�f�[�^�̕\�� */
		printf("��M --> %s\n", szBuf);
	}

	//close
	closesocket(s);
}

void main(void) {
	WSADATA wsaData;
	int mode;

	/* WinSock�̏����� */
	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
		//init error
		printf("WinSock�̏������Ɏ��s���܂���\n");
		return;
	}

	/* �T�[�o�[���H�@�N���C�A���g���H */
	printf("�T�[�o�[�Ȃ�0����́@�N���C�A���g�Ȃ�1����� --> ");
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