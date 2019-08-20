#include<iostream>
#include<arpa/inet.h>
#include<unistd.h>
#include<netdb.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
using namespace std;

int main(int argc,char* argv[])
{
  if(argc!=2)
  {
    cout<<"provide correct port no"<<endl;
    return 0;
  }
 

 struct sockaddr_in serv;
 int port = atoi(argv[1]);
 int sockfd;
 
 serv.sin_family = AF_INET;
 serv.sin_port = htons(port);
 serv.sin_addr.s_addr = htonl(INADDR_ANY);



 sockfd = socket(AF_INET,SOCK_DGRAM,0);


  int bindsd;
  bindsd = bind(sockfd, (struct sockaddr*) &serv, sizeof(serv));
    
    if(bindsd < 0)
    {
        cout << "Binding Error" << endl;
        return 0;
    }
    cout<<"going to recv\n";

    unsigned char packet[1500];
    memset(&packet,'\0',sizeof(packet));
    socklen_t slen=sizeof(serv);
    unsigned char buff[4];
    
    while(1)
    {
      
    
    int rs=recvfrom(sockfd,packet,sizeof(packet),0,(struct sockaddr *)&serv,&slen);
    if(rs==-1)
    {
      cout<<"Receive datagram error"<<endl;
    }
    
    cout<<"MESSAGE RECEIVED:"<<endl;
 

    cout<<int(packet[0]);
    cout<<int(packet[1]);
    cout<<"--";
    
    for (int i = 2; i < 6; i++)
    {
        buff[i-2] = packet[i]; 
    }

    int value1;
    memcpy(&value1,(unsigned int *)&buff,sizeof(int));
    cout<<value1;
    
    cout<<"--";
    cout<<int(packet[6]);
    cout<<"--";
    int k = 0;

    for (int i = 7; i <sizeof(packet); i++)
    {
      if(packet[i]== '\0' )
      {
        break;
      }
      
        cout<<(unsigned char)packet[i];
        k++;
    
    }

    cout<<endl;

    unsigned int ttl = int(packet[6]) - 1;
    packet[6] = (unsigned char)ttl;

    int rp = sendto(sockfd,packet,7+k,0,(struct sockaddr *)&serv,slen);
    if(rp<0)
    { 
       cout<<"Sending datagram error";
    }
    cout<<"MESSAGE SENT:"<<endl;

    cout<<int(packet[0]);
    cout<<int(packet[1]);
    cout<<"--";
    
    int value2;
    memcpy(&value2,(unsigned int*)&buff,sizeof(int));
    cout<<value2;

    cout<<"--";
    cout<< int(packet[6]);
    cout<<"--";
    for (int i = 7; i <7+k; i++)
    {
      cout<<(unsigned char)packet[i];
    }
    cout<<endl;

    memset(&packet,'\0',sizeof(packet));
    memset(&buff,0,sizeof(buff));

    cout<<endl;
      
    }
    return 0;

}
