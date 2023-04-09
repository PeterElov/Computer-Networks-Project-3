#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
//link to system socket library

#define MaxClientNum 100
//define the maxium client number to 100
int count;
int cSocket[MaxClientNum];
void* communicate(void* a);
//prototype the communicate function

int main() {
    count = 0;
    int sSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    //AF_INET: IPv4 address family 
    //SOCK_STREAM: for TCP
    //IPPROTO_TCP: TCP
    if (sSocket == -1) {
        perror("Failed to create socket");
        return -2;
    }
    printf("Successfully created socket\n");

    struct sockaddr_in addr = { 0 };
    addr.sin_family = AF_INET;
    //AF_INET: IPv4 address family 
    addr.sin_addr.s_addr = inet_addr("10.35.70.17");
    //raspberry PI server IP address
    addr.sin_port = htons(33333);
    //raspberry PI server port number
    int r = bind(sSocket, (struct sockaddr*)&addr, sizeof addr);
    //bind function to the socket
    if (r == -1) {
        perror("Failed to bind socket");
        close(sSocket);
        return -2;
    }
    printf("Bind Socket Successfully!\n");

    r = listen(sSocket, SOMAXCONN);
    //keep listening from the socket to check if there are any messages
    if (r == -1) {
        perror("Failed to listen socket");
        close(sSocket);
        return -2;
    }
    printf("Successfully listen to socket\n");

    for (int i = 0; i < MaxClientNum; i++) {
    //for loop to keep detecting the clients connected to the server
    //i is the number of the client which has connected to the server
        cSocket[i] = accept(sSocket, NULL, NULL);
        //if all the protoocol are the same, then accept
        if (cSocket[i] == -1) {
            perror("Server crushed");
            close(sSocket);
            return -3;
        }
        printf("Client number %d has successfully connected to server\n", i + 1);
        count++;

        pthread_t thread_id;
        //define a thread to communicate with the clients
        pthread_create(&thread_id, NULL, communicate, (void*)(intptr_t)i);
        //create a thread
    }
    while (1);
    return 0;
}

void* communicate(void* a) {
    int index = (int)(intptr_t)a;
    char buff[128];
    int r;
    char temp[128];
    while (1) {
        r = recv(cSocket[index], buff, 127, 0);
        //receive the date from the clients
        if (r > 0) {
            buff[r] = 0;
            printf(">> Client %d: %s \n", index + 1, buff);
            //output the date we received
            //at the same time send them to all the clients which have connected to the server
            memset(temp, 0, 128);
            //clear temp for each loop
            sprintf(temp, "Client %d: %s", index + 1, buff);
            for (int i = 0; i < count; i++) {
            send(cSocket[i], temp, strlen(temp), 0);
            //send them to all the clients
            }
        }
    }
    return 0;
}
