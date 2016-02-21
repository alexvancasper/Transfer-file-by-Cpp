#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#pragma pack(push, 1)
    struct HELLO
    {
	unsigned short total;
    };
#pragma pack(pop)

#pragma pack(push, 1)
    struct OPEN
    {
	char fileName[64];
	uint32_t fileSize;
	uint16_t calc;
	uint16_t cur;
    };
#pragma pack(pop)

 
int main()
{

    int sock, listener, k;       // дескрипторы сокетов
    struct sockaddr_in addr;  // структура с адресом
    
    int bytes_read;           // кол-во принятых байт
    FILE *file;    

    struct HELLO hello;
    struct OPEN open;

    printf("Size of (HELLO)  %d \n",sizeof(hello));
    printf("Size of (OPEN)  %d \n",sizeof(open));

    printf("Size of unsigned long %d \n", sizeof(unsigned long));
    printf("Size of unsigned short %d \n", sizeof(unsigned short));
    printf("Size of char %d \n", sizeof(char));

    listener = socket(AF_INET, SOCK_STREAM, 0); // создаем сокет для входных подключений
    //listener = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // создаем сокет для входных подключений
    if(listener < 0)
    {
        perror("socket");
        exit(1);
    }
    
    // Указываем параметры сервера
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    //addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_addr.s_addr=inet_addr("194.58.88.242");
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) // связываемся с сетевым устройство. Сейчас это устройство lo - "петля", которое используется для отладки сетевых приложений
    {
        perror("bind");
        exit(2);
    };

    printf ("Listening port 8080\n");
    listen(listener, 1); // очередь входных подключений

    while(1)
    {
        sock = accept(listener, NULL, NULL); // принимаем входные подключение и создаем отделный сокет для каждого нового подключившегося клиента
        if(sock < 0)
        {
            perror("Прием входящих подключений");
            exit(3);
        }

	switch(fork())
	{
	    case -1:
		perror("fork");
		break;
	    case 0:
		close(listener);

        	printf("Ожидаем сообщение... \n");
	        recv(sock, &hello, sizeof(hello),0); // принимаем сообщение от клиента
        	printf("Всего файлов: %d \n", hello.total);
		k=0;
	        while(k<hello.total)
    		{	
		    printf("Запсь о файле # %d\n", k);
		    recv(sock, &open, sizeof(open),0); // принимаем сообщение от клиента
        	    printf("Имя файла %s \n", open.fileName);
		    printf("Размер файла %d \n", open.fileSize);
		    printf("Buffer: %d \n", open.calc);
		    printf("Cur: %d \n", open.cur);
		    
		    if ((file=fopen(open.fileName,"wb"))== NULL)
		    {
			printf("Cannot create the file %s \n", open.fileName);
		    }
		    printf ("File created %s \n", open.fileName);
		    unsigned int RecvSize=0, received=0;
		    char buf[open.calc];	      // буфур для приема
		    while (received<open.fileSize)
		    {
			RecvSize = recv(sock,buf,sizeof(buf),0);
			//fwrite(buf,sizeof(char),sizeof(buf),file);
			fwrite(buf,sizeof(char),RecvSize,file);
			received+=RecvSize;
			printf ("RecvSize: %d \t Received %d \n", RecvSize, received);
		    }
		    send(sock, "OK", sizeof("OK"), 0);
		    memset(buf,0,sizeof(buf));
		    k++;
    		}
		printf("Close the server socket");
    		close(sock); // закрываем сокет
    		//exit(0);
	    default:
		close(sock);
	}   
    }
    
    return 0;
}

