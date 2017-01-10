

#include "Car.h"

#include<arpa/inet.h> //inet_addr
#include <unistd.h>
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket


#define  CAR_IP "193.226.12.217"
#define  CAR_PORT 20231

Car::Car() : socket_desc(-1){
}

Car::~Car() {
    close(socket_desc);
}

void Car::sendCommand(char* cmd) {

        //printf("Enter message : ");
        //scanf("%s" , message);
        char server_reply[200];

        //Send some data
        if( send(socket_desc , cmd , strlen(cmd) , 0) < 0)
        {
            puts("Send failed");
            return;
        }

        //Receive a reply from the server
        if( recv(socket_desc , server_reply , 200 , 0) < 0)
        {
            puts("recv failed");
        }

        puts("Server reply :");
        puts(server_reply);

}

bool Car::connectToCar(){



    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");



    server.sin_addr.s_addr = inet_addr(CAR_IP);
    server.sin_family = AF_INET;
    server.sin_port = htons( CAR_PORT );

    //Connect to remote server
    if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connectToCar failed. Error");
        return false;
    }

    puts("Connected\n");

}

