

#ifndef LAB_FIC_2016_CAR_H
#define LAB_FIC_2016_CAR_H

#include <string.h>
#include<arpa/inet.h> //inet_addr

class Car {

public:

    Car();

    bool connectToCar();

    virtual ~Car();
    void sendCommand(char* cmd);

private:
    struct sockaddr_in server;
    int socket_desc;

};



#endif //LAB_FIC_2016_CAR_H
