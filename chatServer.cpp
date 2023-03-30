// #include <stdio.h>
// #include <winSock2.h>
// #pragma comment(lib,"ws2_32.lib")
// //link to winsocket library
// #include <windows.h>
// #pragma warning(disable:4996) 
// //this code is just for avoiding the errors in socket2 functions
// //above are basic steps for Initialising Winsock
// //https://www.binarytides.com/winsock-socket-programming-tutorial/
// #define MaxClientNum 100
// //define the maxium number of the clients
// int count;
// //this count is used in the future for counting the number of clients

// SOCKET cSocket[MaxClientNum];
// //Define a SOCKET type array whose name is cSocket

// DWORD WINAPI communicate(LPVOID a);
// //function prototype in C++ in front of the main function
// //the thread function which can communicate with the clients

// int main() {
// 	count = 0;
// 	//1.set up the protocol version
// 	WSADATA wsaData;
// 	WSAStartup(MAKEWORD(2, 2), &wsaData);
// 	//initialize the data definition language that the winsocket process used
// 	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
// 	//check the version 
// 		printf("The requested winSocket protocol version is not 2.2!\n");
		
// 		WSACleanup();
//         //9.clear the protocol info if the requested protocol version is not 2.2
// 		return -1;
// 	}
// 	printf("The requested winSocket protocol version is 2.2!\n");
// 	//2.creat socket
// 	SOCKET sSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
// 	//AF_INET: IPv4 address family 
//     //SOCK_STREAM: for TCP
//     //IPPROTO_TCP: TCP
// 	if (SOCKET_ERROR == sSocket) {
// 		printf("Failed to create socket:%d\n", GetLastError());
// 		return -2;
// 	}
// 	printf("Successfully created socket\n");
// 	//3.set up the server protocol address family
// 	SOCKADDR_IN addr = { 0 };
// 	addr.sin_family = AF_INET;
// 	//AF_INET: IPv4 address family, the same with the created socket before
// 	addr.sin_addr.S_un.S_addr = inet_addr("10.35.70.37");
// 	//local ip address which can test in different devices easily
// 	addr.sin_port = htons(33333);
// 	//server port number, bind with the client, could be any number(just make sure the port isn't used)
// 	//4.bind
// 	int r = bind(sSocket, (sockaddr*)&addr, sizeof addr);
// 	if (-1 == r) {
// 		printf("Failed to bind socket:%d\n", GetLastError());
// 		//8.close the connection
// 		closesocket(sSocket);
// 		//9.clear the protocol info
// 		WSACleanup();
// 		return -2;
// 	}
// 	printf("Bind Socket Successfully!\n");
	
// 	//5.listen
// 	r = listen(sSocket, SOMAXCONN);
// 	if (-1 == r) {
// 		printf("Failed to listen socket:%d\n", GetLastError());
// 		//8.close the connection
// 		closesocket(sSocket);
// 		//9.clear the protocol info
// 		WSACleanup();
// 		return -2;
// 	}
// 	printf("Successfully listen to socket\n");
// 	//6.wait for the clients for connection
	
// 	for(int i = 0;i < MaxClientNum; i++){
// 	//i is the number of the client which has connected to the server
// 		cSocket[i]=accept(sSocket, NULL, NULL);
// 		//connect the server in the client
// 		if (SOCKET_ERROR == cSocket[i]) {
// 			printf("Server crushed:%d\n", GetLastError());
// 			//8.close the connection
// 			closesocket(sSocket);
// 			//9.clear the protocol info
// 			WSACleanup();
// 			return -3;
// 		}
// 		printf("Client number %d has successfully connected to server\n",i+1);
// 		count++;
// 		//while each one client is added, the count should add one
// 		//creat a thread communicate with client
// 		CreateThread(NULL, NULL,//could be 0 or NULL
// 			communicate,//lpStartAddress
// 			(LPVOID)i,//type conversion
// 			NULL,NULL);//could be 0 or NULL
// 	}
// 	while (1);
// 	return 0;
// }


// //thread which communicate with client
// //the thread function which can communicate with the clients
// DWORD WINAPI communicate(LPVOID a) {
// 	//7.communicate
// 	int index = (int) a;
// 	char buff[128];
// 	int r;
// 	char temp[128];
// 	while (1) {
// 		r = recv(cSocket[index], buff, 127, NULL);
// 		//receive the date from the clients
// 		if (r > 0) {
// 			buff[r] = 0;
// 			// terminate the strings
// 			printf(">> Client %d: %s \n", index+1,buff);
// 			//output the date we received
// 			//at the same time send them to all the clients which have connected to the server
// 			memset(temp, 0, 128);
// 			//clear temp for each loop
// 			sprintf(temp, "Client %d: %s", index+1, buff);
// 			for (int i = 0; i < count; i++) {
// 				send(cSocket[i], temp, strlen(temp),NULL);
// 				//send them to all the clients
// 			}
// 		}
// 	}
// 	return 0;
// }

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#define MaxClientNum 100

int count;
int cSocket[MaxClientNum];

void *communicate(void *a);

int main() {
    count = 0;

    int sSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sSocket == -1) {
        perror("Failed to create socket");
        return -2;
    }
    printf("Successfully created socket\n");

    struct sockaddr_in addr = { 0 };
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("10.35.70.37");
    addr.sin_port = htons(33333);

    int r = bind(sSocket, (struct sockaddr *)&addr, sizeof addr);
    if (r == -1) {
        perror("Failed to bind socket");
        close(sSocket);
        return -2;
    }
    printf("Bind Socket Successfully!\n");

    r = listen(sSocket, SOMAXCONN);
    if (r == -1) {
        perror("Failed to listen socket");
        close(sSocket);
        return -2;
    }
    printf("Successfully listen to socket\n");

    for (int i = 0; i < MaxClientNum; i++) {
        cSocket[i] = accept(sSocket, NULL, NULL);
        if (cSocket[i] == -1) {
            perror("Server crushed");
            close(sSocket);
            return -3;
        }
        printf("Client number %d has successfully connected to server\n", i + 1);
        count++;

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, communicate, (void *)(intptr_t)i);
    }
    while (1);
    return 0;
}

void *communicate(void *a) {
    int index = (int)(intptr_t)a;
    char buff[128];
    int r;
    char temp[128];
    while (1) {
        r = recv(cSocket[index], buff, 127, 0);
        if (r > 0) {
            buff[r] = 0;
            printf(">> Client %d: %s \n", index + 1, buff);

            memset(temp, 0, 128);
            sprintf(temp, "Client %d: %s", index + 1, buff);
            for (int i = 0; i < count; i++) {
                send(cSocket[i], temp, strlen(temp), 0);
            }
        }
    }
    return 0;
}