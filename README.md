#Transferring files in C ++
In this project I've done moving some files between client and server. Implemented the struct on TCP/IP. Byte alignment structure on server and client side.

    #pragma pack(push, 1)
    struct OPEN
    {
        char fileName[64];
        uint32_t fileSize;
        uint16_t calc;
        uint16_t cur;
    };
    #pragma pack(pop)
    
    strncpy(open.fileName, pFileName,PATH_MAX_SIZE);  //Copy filename into the structure.


more details in the sorce code.
If you have some question, you can ask me. 
Thank you :)

Output from the client side:
C:\Debug>FileTransfer_cli_second.exe textfile.004.md5 textfile.004
 'textfile.004.md5'
 'textfile.004'
sizeof(OPEN): 72 
Press any key 
Total files to be sent: 3 
Prepearing to send the file 'textfile.004.md5' 
read symbols: 48, fullsize: 48, pos: 48 
File was sent successfully. 
Prepearing to send the file 'textfile.004'

Output from the server side.
s08:~/c/server$ ./server
Size of (HELLO)  2
Size of (OPEN)  72
Listening port 8080
Всего файлов: 4
Запсь о файле # 0
Имя файла textfile.004.md5
Размер файла 48
Buffer: 48
Cur: 1
File created textfile.004.md5
Запсь о файле # 1
Имя файла textfile.004
Размер файла 2017593
Buffer: 1024
Cur: 2
File created textfile.004
Запсь о файле # 2
Имя файла textfile.003.md5
Размер файла 48
Buffer: 48
Cur: 3
File created textfile.003.md5
RecvSize: 48     Received 48
Запсь о файле # 3
Имя файла textfile.003
Размер файла 10485760
Buffer: 1024
Cur: 4
File created textfile.003
