#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
using namespace std;
  
int main(int argc, char *argv[])
{
    if(argc != 3)
    
    {
        cout << "Usage: ip_address port" << endl; exit(0);
    }

    char *ip = argv[1]; 
    int port = atoi(argv[2]);

    char msg[1500];
    

    sockaddr_in sendSockAddr;
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr = inet_addr(ip);
    sendSockAddr.sin_port = htons(port);

    int clientSd = socket(AF_INET, SOCK_STREAM, 0);
    
    int status = connect(clientSd,(sockaddr*) &sendSockAddr, sizeof(sendSockAddr));
    if(status < 0)
    {
        cout<<"Error connecting to socket!"<<endl;
    }
    
    cout << "Connected to the server!" << endl;
    
   
    cout << "==>";
    string data;
    getline(cin,data);
       
    strcpy(msg, data.c_str());
    send(clientSd, (char*)&msg, strlen(msg), 0);
    memset(&msg, 0, sizeof(msg));//clear the buffer
        
        
    close(clientSd);
    cout << "******SESSION******" << endl;
    cout << "Connection closed" << endl;
    return 0;  
}
