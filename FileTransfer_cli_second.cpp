// FileTransfer_cli_second.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <stdio.h>
#include <conio.h>
#include <io.h>
#pragma comment (lib, "Ws2_32.lib")
#include <fstream>
#include <iostream>
#include "FileTransfer_cli_second.h"

//The size of filename include the path
const uint8_t PATH_MAX_SIZE = 64;

using namespace System;
using namespace System::IO;
using namespace std;

void concat(char *to, char *from)
{
	for (; *to; ++to);
	while (*to++ = *from++);
}


std::ifstream::pos_type GetMyFileSize(const char* filename)
{
	std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
	return in.tellg();
}

unsigned int SendFile(SOCKET s, FILE *f, unsigned long filesize)
{
	int  size, symbols ;
	const unsigned int buf_size = 1024;
	while (!feof(f)) //read while we cannot see EnfOfFile
	{
		char bufer[buf_size];
		symbols = fread(bufer, 1, buf_size, f);
		size = ftell(f);//Current position of cursor
		printf("read symbols: %d, fullsize: %ld, pos: %ld \n", symbols, filesize, size);
		send(s, bufer, symbols, 0);//Transfer the buffer
	}
	//Receiving answer from the server that file was received.
	char answer[4] = "";
	recv(s, answer, sizeof(answer), 0);
	if (answer=="OK")
	{
	//	printf("OK\n");
		return 0;
	}
	else 
	{
		return size;
	}

}

SOCKET socketInit()
{
	int rc;

	WSADATA wsd;
	//Initialization
	WSAStartup(0X0101, &wsd);
	//Initialization socket int case of client/server TCP
	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);

	//desription of structure
	sockaddr_in peer;
	peer.sin_family = AF_INET;
	peer.sin_port = htons(8080);
	peer.sin_addr.S_un.S_addr = inet_addr("194.58.88.242");

	//Initialization of client
	rc = connect(s, (struct sockaddr*) &peer, sizeof(peer));
	if (rc < 0)
	{
		printf("Socket failed %ld\n", GetLastError());
		exit(2);
	}

	return s;
}

//Desription of our HELLO structure, for transfer the file to the server.
#pragma pack(push,1)
typedef struct HELLO
{
	unsigned short total;
};
#pragma pack(pop)

//Desription of our OPEN structure. It send before the filedata.
#pragma pack(push,1)
typedef struct OPEN
{
	char fileName[PATH_MAX_SIZE];
	unsigned long fileSize;
	unsigned short calc;
	unsigned short cur;
};
#pragma pack(pop)

int main(int argc, char* argv[])
{
	
	unsigned short files; //Counters of files
	unsigned int filesize, fileSizeSent;

	HELLO hello;
	OPEN open;

	if (argc < 1)
	{
		printf("You must use at least one filename in the argument fileds.");
		exit(1);
	}

	//show us all files.
	for (files = 1; files < argc; files++)
	{
		printf(" '%s' \n",argv[files]);
	}

//	printf("sizeof(HELLO): %d \n", sizeof(hello));
	printf("sizeof(OPEN): %d \n", sizeof(open));
//	printf("Size of unsigned long %d \n", sizeof(unsigned long));
//	printf("Size of unsigned short %d \n", sizeof(unsigned short));
//	printf("Size of string %d \n", sizeof(string));

	printf("Press any key to continue\n");
	_getch();

	//Inicialization socket for transfer the files
	SOCKET s = socketInit();
	//Send the 'hello' structure with total numbers of files.
	hello.total = argc-1;
	printf("Total files to be sent: %d \n", argc);

	send(s, (char *)&hello, sizeof(hello), 0);

	for (files = 1; files < argc; files++)
	{
		char *pFileName = argv[files];

		FILE *f = fopen(pFileName, "rb");
		if (f == NULL)
		{
			printf("Cannot open the file '%s'.\n ", pFileName);
			exit(3);
		}

		printf("Prepearing to send the file '%s' \n", pFileName);

		//define the size of each file.
		filesize = GetMyFileSize(pFileName);
		if (filesize <= 0)
		{
			printf("File is empty '%s'.\n ", pFileName);
			exit(4);
		}
	
		strncpy(open.fileName, pFileName,PATH_MAX_SIZE);  //Copy filename into the structure.
		open.fileSize = filesize;  
		open.cur = files;

		//define the buffer for transfer.
		if (filesize < 1024)
		{
			open.calc = filesize;
		}
		else
		{
			open.calc = 1024;
		}


		send(s, (char *)&open, sizeof(open), 0);

		Sleep(1000);
		//Transfer file
		fileSizeSent = SendFile(s, f, filesize);
		if ((fileSizeSent == 0) || (fileSizeSent == filesize))
		{
			printf("File was sent successfully.\n");
		}
		else if (fileSizeSent < filesize)
		{
			printf("ERROR. Sent '%u' bytes\n", fileSizeSent);
		}
		//close the file that was transfered.
		fclose(f);

		Sleep(2000);
	}//cycle for
	
	 // shutdown socket, clear
	shutdown(s, 2);
	WSACleanup();


	printf("Press any key for close the program \n");
	_getch();

    return 0;
}

