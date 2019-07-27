#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

HANDLE handle;
WSADATA wsaData;
SOCKET sock;
sockaddr_in address;

void OnExit(){
	CloseHandle(handle);

	closesocket(sock);
	WSACleanup();
}

void SerialInitialize(char* portName,int baudRate){
	handle = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);

	if (handle == INVALID_HANDLE_VALUE){
		printf("Serial Open Error!\n");
		exit(1);
	}

	DCB dcb;
	GetCommState(handle, &dcb);
	dcb.BaudRate = baudRate;
	SetCommState(handle, &dcb);
}

void UDPInitialize(u_short portnum){
	int res = WSAStartup(MAKEWORD(2, 0), &wsaData);
	if(res != 0){
		printf("UDP Open Error!\n");
		exit(1);
	}

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
	{
		printf("Socket Create Error!!\n");
		exit(1);
	}

	address.sin_family = AF_INET;
	address.sin_port = htons(portnum);
	address.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

}

void SocketSend(char* str,u_char length){
	sendto(sock, str, length, 0, (struct sockaddr *)&address, sizeof(address));
	printf("%s", str);
}

int main(int argc, char *argv[])
{

	atexit(OnExit);

	if(argc < 4){
		printf("Put argument like \"COM4 115200 8001\".");
		return 1;
	}
	
	SerialInitialize(argv[1],atoi(argv[2]));
	UDPInitialize(atoi(argv[3]));


	char buffer[1],str[256];
	u_char counter = 0;

	while (1)
	{
		ReadFile(handle, buffer, 1, NULL, NULL);
		str[counter] = buffer[0];
		if(buffer[0] == '\n'){
			str[counter + 1] = 0;
			SocketSend(str, counter);
			str[0] = 0;
			counter = 0;
		}else counter++;
	}
}
