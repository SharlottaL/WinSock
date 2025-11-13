#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN		
#endif

#include<Windows.h>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<iphlpapi.h>
#include<iostream>
using namespace std;

#pragma comment(lib, "WS2_32.lib")

#define DEFAULT_PORT "27015"
#define BUFFER_LENGTH 1460

int main()
{
	setlocale(LC_ALL, "");
	cout << "Hello WinSock" << endl;
	cout << "Клиент запущен" << endl;
	INT iResult = 0;
	DWORD dwLastError = 0;
	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		cout << "WSAStartup failed: " << iResult << endl;
		return iResult;
	}

	addrinfo* result = NULL;
	addrinfo* ptr = NULL;
	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	/*string ipAddress;
	cout << "Enter server IP address (127.0.0.1 for local): ";
	cin >> ipAddress;
	cin.ignore();*/

	iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		cout << "getaddreinfo failed: " << iResult << endl;
		WSACleanup();
		return iResult;
	}

	ptr = result;
	SOCKET connect_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if (connect_socket == INVALID_SOCKET)
	{
		dwLastError = WSAGetLastError();
		cout << "Socket error: " << dwLastError << endl;
		freeaddrinfo(result);
		WSACleanup();
		return dwLastError;
	}
	iResult = connect(connect_socket, ptr->ai_addr, (INT)ptr->ai_addrlen);
	if(iResult == SOCKET_ERROR)
	{
		dwLastError = WSAGetLastError();
		cout << "Connection error: " << dwLastError << endl;
		closesocket(connect_socket);
		freeaddrinfo(result);
		WSACleanup();
		return dwLastError;
	}
	
	cout << "Успешно подключено к серверу!" << endl;

	// Цикл обмена сообщениями
	CHAR recv_buffer[BUFFER_LENGTH] = {};
	CHAR send_buffer[BUFFER_LENGTH] = {};
	do
	{
		// Клиент вводит сообщение
		cout << "Введите сообщение: ";
		cin.getline(send_buffer, BUFFER_LENGTH);

		// Отправка сообщения на сервер
		iResult = send(connect_socket, send_buffer, strlen(send_buffer), 0);
		if (iResult == SOCKET_ERROR)
		{
			dwLastError = WSAGetLastError();
			cout << "Send failed with error: " << dwLastError << endl;
			closesocket(connect_socket);
			freeaddrinfo(result);
			WSACleanup();
			return dwLastError;
		}
		cout << iResult << " Bytes sent" << endl;

		// Проверка на выход
		if (strcmp(send_buffer, "exit") == 0 || strcmp(send_buffer, "quit") == 0)
		{
			cout << "Завершение работы..." << endl;
			break;
		}
	} while (iResult > 0);

	// Завершение работы
	
	/*iResult = send(connect_socket, send_buffer, strlen(send_buffer), 0);
	if (iResult == SOCKET_ERROR)
	{
		dwLastError = WSAGetLastError();
		cout << "Send failed with error: " << dwLastError << endl;
		closesocket(connect_socket);
		freeaddrinfo(result);
		WSACleanup();
		return dwLastError;
	}
	cout << iResult << " Bytes sent" << endl;*/
	//CHAR recv_buffer[BUFFER_LENGTH] = {};
	do
	{
		iResult = recv(connect_socket, recv_buffer, BUFFER_LENGTH, 0);
			if(iResult>0)
				cout << iResult << " Bytes received, Message:\t" << recv_buffer << ".\n";
			else 
				if (iResult == 0)
					cout << "Connection closed" << endl;
			else
					cout << "Receive failed with error: " << WSAGetLastError() << endl;
	} while (iResult > 0);

	iResult = shutdown(connect_socket, SD_SEND);
	if(iResult == SOCKET_ERROR)
	{
		dwLastError = WSAGetLastError();
		cout << "Shutdown failed with error: " << dwLastError << endl;
	}

	closesocket(connect_socket);
	freeaddrinfo(result);
	WSACleanup();
	return dwLastError;

}