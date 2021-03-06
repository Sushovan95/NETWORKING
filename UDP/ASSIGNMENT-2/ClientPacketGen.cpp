#include<iostream>
#include<arpa/inet.h>
#include<unistd.h>
#include<netdb.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<stdio.h>
#include<sys/time.h>
#include<string.h>
#include<stdlib.h>
#include<fstream>
#include<sstream>
using namespace std;

#define max_num 90
#define min_num 65

int main(int argc,char* argv[])
{
  if(argc<6)
  {
    cout<<"ERROR"<<endl;
    return 0;
  }

  ofstream myfile;
  myfile.open ("PLOT1.txt");

  struct sockaddr_in serv,client;

  int j = 0;
  char* ip = argv[1];//IP
  int port = atoi(argv[2]);//Port
  int P = atoi(argv[3]);//Payload p byte
  unsigned int TTL = atoi(argv[4]);//TTL 1byte
  int numpack = atoi(argv[5]);//number of packets 2byte

 serv.sin_addr.s_addr = inet_addr(ip);
 serv.sin_family = AF_INET;
 serv.sin_port = htons(port);
 
 socklen_t slen = sizeof(serv);

  int clientsd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);



  int size = (7+P);
  unsigned char packet[size];

  struct timeval start, end1,end2;
  gettimeofday(&start, NULL);
  unsigned long long int timer1 = (( long long int) start.tv_sec) *1000000ll + (long long int) start.tv_usec;
 
 while(j<numpack)
 {
    
  gettimeofday(&end1, NULL);
  unsigned long long int timer2 = (( long long int) end1.tv_sec) *1000000ll + (long long int) end1.tv_usec;
 

  unsigned int timediff = (timer2 - timer1)%4294967296;
  cout<<timediff<<endl;

  packet[0] = (unsigned char) 0;
  packet[1] = (unsigned char) j ;

  unsigned char buff[4];
  
  memcpy(&buff,(unsigned char *)&timediff,sizeof(buff));
  memcpy(&packet[2],&buff,sizeof(buff));

  packet[6] = (unsigned char) TTL;

  for(int i = 7;i<size;i++)
    {
      
      unsigned int num = rand() % (max_num+1-min_num) + min_num;
      packet[i] = (unsigned char) num;
    
    }

    cout<<"MESSAGE SENT:"<<endl;
    int rp = sendto(clientsd,packet,size,0,(struct sockaddr *)&serv,slen);
    if(rp<0)
    { 
       cout<<"Writing datagram error"<<endl;
    }

    cout<<int(packet[0]);
    cout<<int(packet[1]);
    cout<<"--";
    
    unsigned int value1;
    memcpy(&value1,(unsigned int *)&buff,sizeof(int));
    cout<<value1;


    cout<<"--";
    cout<<int(packet[6]);
    cout<<"--";
    for (int i = 7; i < size; i++)
    {
      cout<<packet[i];
    }
    cout<<endl;

    memset(&packet,'\0',sizeof(packet));
    memset(&buff,0,sizeof(buff));
    
    int rc= recvfrom(clientsd,packet,sizeof(packet),0,(struct sockaddr *)&serv,&slen);
    if(rc<0)
     {
        cout<<"Receiving datagram error"<<endl;
     }

  gettimeofday(&end1, NULL);
  unsigned long long int timer3 = (( long long int) end1.tv_sec) *1000000ll + (long long int) end1.tv_usec;

  unsigned int RTT = (timer3 - timer2)%4294967296; //2^32
  cout<<"RTT VALUE:"<<RTT<<endl;

  
    myfile<<RTT<<endl;
   
    cout<<"MESSAGE RECEIVED:"<<endl;

    cout<<int(packet[0]);
    cout<<int(packet[1]);
    cout<<"--";

    for (int i = 2; i < 6; ++i)
    {
      buff[i-2] = packet[i];
    }
    
    int value2;
    memcpy(&value2,(unsigned int*) buff,sizeof(int));
    cout<<value2;

    cout<<"--";
    cout<<int(packet[6]);
    cout<<"--";
    for (int i = 7; i < size; i++)
    {
      cout<<packet[i];
    }
    cout<<endl;

    memset(&packet,'\0',sizeof(packet));


    j++;

    cout<<endl;

  }

 close(clientsd);
 myfile.close();

  return 0;
}
