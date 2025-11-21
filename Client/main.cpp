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

short y_position = 1;

VOID Recive(SOCKET connect_socket);
VOID InputMessage(CHAR send_buffer[]);
VOID PrintMessage(CHAR recv_buffer[], INT iResult);


int main()
{
	setlocale(LC_ALL, "");
	cout << "Hello WinSock" << endl;
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
	if (iResult == SOCKET_ERROR)
	{
		dwLastError = WSAGetLastError();
		cout << "Connection error: " << dwLastError << endl;
		closesocket(connect_socket);
		freeaddrinfo(result);
		WSACleanup();
		return dwLastError;
	}
	DWORD dwThreadID = 0;
	HANDLE hRecvThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Recive, (LPVOID)connect_socket, 0, &dwThreadID);

	CHAR send_buffer[BUFFER_LENGTH] = "Hello, WinSock";
	do
	{
		InputMessage(send_buffer);
		iResult = send(connect_socket, send_buffer, strlen(send_buffer), 0);
		///////////////////////////
		//Recive(connect_socket);
		////////////////////////////
		if (iResult == SOCKET_ERROR)
		{
			dwLastError = WSAGetLastError();
			cout << "Send failed with error: " << dwLastError << endl;
			closesocket(connect_socket);
			freeaddrinfo(result);
			WSACleanup();
			return dwLastError;
		}
		//cout <<"\n" << iResult << " Bytes sent" << endl;
	} while (strstr(send_buffer, "exit") == 0 && strstr(send_buffer, "quit") == 0);
	CloseHandle(hRecvThread);
	//} while (strcmp(send_buffer, "exit") != 0 && strcmp(send_buffer, "quit") != 0);
	send(connect_socket, "quit", 4, 0);
	iResult = shutdown(connect_socket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		dwLastError = WSAGetLastError();
		cout << "Shutdown failed with error: " << dwLastError << endl;
	}
	closesocket(connect_socket);
	freeaddrinfo(result);
	WSACleanup();
	return dwLastError;
}
VOID InputMessage(CHAR send_buffer[])
{
	/*HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi = {};
	BOOL ok = GetConsoleScreenBufferInfo(hConsole, &csbi);
	COORD position{0, 25};
	SetConsoleCursorPosition(hConsole, position);
	*/
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	SetConsoleCursorPosition(hConsole, { 1, 25 });
	printf("\x1b[2K");
	ZeroMemory(send_buffer, BUFFER_LENGTH);
	cout << "¬ведите сообщение: ";
	SetConsoleCursorPosition(hConsole, { 20, 25 });
	SetConsoleCP(1251);
	cin.getline(send_buffer, BUFFER_LENGTH);
	SetConsoleCP(866);
	
	//SetConsoleCursorPosition(hConsole, {0, 20});
	//CloseHandle(hConsole);
}

VOID PrintMessage(CHAR recv_buffer[], INT iResult)
{
	recv_buffer[iResult] = '\0';
	 HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hConsole, {0, y_position});
    cout << y_position++ << " " << iResult << " Bytes received, Message: " << recv_buffer << endl;
    SetConsoleCursorPosition(hConsole, { 20, 25 });
}
VOID Recive(SOCKET connect_socket)
{
	INT iResult = 0;
	CHAR recv_buffer[BUFFER_LENGTH] = {};
		do
		{
	iResult = recv(connect_socket, recv_buffer, BUFFER_LENGTH, 0);
	if (iResult > 0)
		PrintMessage(recv_buffer, iResult);
	else
		if (iResult == 0)
			cout << "Connection closed" << endl;
		else
			cout << "Receive failed with error: " << WSAGetLastError() << endl;

	} while (iResult > 0);
}
