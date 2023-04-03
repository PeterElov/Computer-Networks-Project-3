// #include <stdio.h>
// #include <winSock2.h>
// #pragma comment(lib,"ws2_32.lib")
// //link to winsocket library
// #include <graphics.h>
// //link to a library that can give us access for a messages record window, we will set the window later in the main function
// SOCKET sSocket;
// #pragma warning(disable:4996) 
// //this code is just for avoiding the errors in socket2 functions

// DWORD WINAPI scanfAndsend(LPVOID a);
// //function prototype in C++ in front of the main function

// int main() {
// 	initgraph(400, 600, 1);
// 	//initialize the height and width of the windows for clients

// 	//1.set up the protocol version
// 	WSADATA wsaData; 
// 	//The WSADATA structure contains information about the Windows Sockets implementation.
//     //https://learn.microsoft.com/en-us/windows/win32/api/winsock/ns-winsock-wsadata 
// 	WSAStartup(MAKEWORD(2, 2), &wsaData);
// 	//initialize the data definition language that the winsocket process used
// 	//set up the prootocal version here, the protocal version should be 2.2
// 	//MAKEWORD builds a 16 bits words from two 1 bytes word
	
// 	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
// 		printf("WinSocket protocol version is incorrect!\n");
// 		//9.clear the protocol info if the requested protocol version is not 2.2
// 		WSACleanup();
// 		return -1;
// 	}
// 	printf("WinSocket protocol version is 2.2!\n");
// 	//2.creat the socket
// 	sSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
// 	//AF_INET: IPv4 address family 
// 	//SOCK_STREAM: for TCP
// 	//IPPROTO_TCP: TCP
// 	if (SOCKET_ERROR == sSocket) {
// 	//in case any errors happened, then print out failed to creat socket
// 		printf("Failed to create socket:%d\n", GetLastError());
// 		return -2;
// 	}
// 	printf("Created Socket Successfully!\n");
// 	//3.set up the server protocol address family
// 	SOCKADDR_IN addr = { 0 };	
// 	addr.sin_family = AF_INET;
// 	//AF_INET: IPv4 address family, the same with the created socket before
// 	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
// 	//local ip address which can test in different devices easily
// 	addr.sin_port = htons(9671);
// 	//server port number
// 	//4.connect to the server
// 	int r = connect(sSocket, (sockaddr*)&addr, sizeof addr);
// 	if (-1 == r) {
// 		printf("Failed to connect to server:%d\n", GetLastError());
// 		return -2;
// 	}
// 	printf("Connected to Server Successfully \n");

// 	CreateThread(NULL, NULL, scanfAndsend, NULL, NULL, NULL);

// 	char buff[128];
// 	int n = 0;
// 	while (1) {
// 		r = recv(sSocket, buff, 127, NULL);
// 		//infinite loop to receive data
// 		if (r > 0) {
// 			buff[r] = 0;
// 			//when we meet the 0, it means the message ends
// 			outtextxy(1, n * 20, buff);
// 			//output the strings to chat room window
// 			n++;
// 			if (n > 29) {
// 				n = 0;
// 			//because each line has a 20 unit height and the window's height is 600. So the maximum messages could be 29 lines
// 				cleardevice();
// 			}
// 		}
// 	}
// 	return 0;
// }

// DWORD WINAPI scanfAndsend(LPVOID a) {
// 	//5.communicate
// 	char buff[128];
// 	int index = (int) a;
// 	//detect how many of the clients connected to the server
// 	while (1) {
// 		memset(buff, 0, 128);
// 		//clear temp for each loop
// 		printf("You are Client %d, write something to the chat room: ",index+1);
// 		//because the lpParam starts from 0 so normally the first people we call it client 1 so the number should be index+1
// 		scanf("%[^\n]", buff);
// 		//scan the messages we wrote
// 		char a = getchar();
// 		//read the \n into the getchar in case scanf will jump directly for the next loop
// 		send(sSocket, buff, strlen(buff), NULL);//send the messages to the server
// 	}



// }

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>

int sSocket;

void *scanfAndsend(void *a);

int main() {
    initscr();
    timeout(0);
    scrollok(stdscr, TRUE);

    sSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sSocket == -1) {
        perror("Failed to create socket");
        return -2;
    }
    printf("Created Socket Successfully!\n");

    struct sockaddr_in addr = { 0 };
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("10.35.70.17");
    addr.sin_port = htons(33333);

    int r = connect(sSocket, (struct sockaddr *)&addr, sizeof addr);
    if (r == -1) {
        perror("Failed to connect to server");
        return -2;
    }
    printf("Connected to Server Successfully \n");

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, scanfAndsend, NULL);

    char buff[128];
    int n = 0;
    while (1) {
        r = recv(sSocket, buff, 127, 0);
        if (r > 0) {
            buff[r] = 0;
            printw("%s\n", buff);
            refresh();
        }
    }

    endwin();
    return 0;
}

void *scanfAndsend(void *a) {
    char buff[128];
    int index = (int)(intptr_t)a;
    while (1) {
        
	memset(buff, 0, 128);
        //echo();
	//rewind(stdin);
        printf("You are Client %d, write something to the chat room: \n", index + 1);
	//char c = getchar();
        //refresh();
	//ile ((buff = getchar())! = '\n')
        //tstr(buff);
	//refresh();
        //noecho();
	scanf("%[^\n]", buff);
        char b = getchar();
        send(sSocket, buff, strlen(buff), MSG_NOSIGNAL);
	//refresh();
    }
}
