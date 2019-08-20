#include<iostream>
#include<arpa/inet.h>
#include<unistd.h>
#include<netdb.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<stdio.h>
#include<sys/time.h>
#include<string.h>
#include<fcntl.h>
#include<stdlib.h>
#include<fstream>
#include<sstream>

using namespace std;

#define RRQ 1
#define WRQ 2
#define DATA 3
#define ACK 4
#define ERROR 5
#define SIZE 516
#define PORT 69

mode_t mode = S_IRUSR | S_IWUSR;

struct sockaddr_in client;
int clientsd;
socklen_t slen;

void upload(char[]);
void download(char[]);


int main(int argc,char* argv[])
{

	if (argc<2)
	{
		cout<<"ERROR"<<endl;
		exit(1);
	}

	
	char* ip = argv[1];//IP

	client.sin_addr.s_addr = inet_addr(ip);
	client.sin_family = AF_INET;
	client.sin_port = htons(PORT);

	slen = sizeof(client);
	clientsd = socket(AF_INET, SOCK_DGRAM,0);

	

	while(1)
	{
		cout<<"1.UPLOAD"<<endl;
		cout<<"2.DOWNLOAD"<<endl;
		cout<<"3.EXIT"<<endl;

		int input;
		cin>>input;

		if (input == 1)
		{
			cout<<"ENTER THE FILE NAME"<<endl;
			string str;
			char filename[SIZE];
			cin>>str;
			strcpy(filename,str.c_str());
			upload(filename);
			memset(filename,0,sizeof(filename));
		}

		else if (input == 2)
		{
			cout<<"ENTER THE FILE NAME"<<endl;
			string str;
			char filename[SIZE];
			cin>>str;
			strcpy(filename,str.c_str());
			download(filename);
			memset(filename,0,sizeof(filename));
		}

		else if (input == 3)
		{
			exit(0);
		}
		else
		{
			cout<<"GIVE CHOICE PROPERLY"<<endl;
		}
			
	}

	return 0;

}

void upload(char filename[]) //send data to server 
{
	FILE* fp;
	fp = fopen(filename,"r");

	unsigned char buffer[SIZE];
	memset(buffer,0,sizeof(buffer));
	unsigned char rcvpbuf[SIZE];

	int len = strlen(filename);

    const char* MODE;
    MODE = "octet";

    int modlen = strlen(MODE);

	buffer[0] =  0;
	buffer[1] =  WRQ;
	memcpy(&buffer[2],filename,len);
	memcpy(&buffer[2+len+1],MODE,modlen);

	int length = 9+len;

	cout<<"SENDING"<<endl;
	int rp = sendto(clientsd,buffer,length,0,(struct sockaddr *)&client,slen);
    if(rp<0)
    { 
       cout<<"Writing datagram error"<<endl;
    }

    struct sockaddr_in server;
    socklen_t srvrlen = sizeof(server);
    
    unsigned char rcvbuf[SIZE];
    unsigned short blockno = 0;
    long int blockcount = 0;
    	
    while(1)
    {
		memset(rcvbuf,0,sizeof(rcvbuf));
		int rc = recvfrom(clientsd,rcvbuf,SIZE,0,(struct sockaddr *)&server,&srvrlen);		
		if (rc<0)
		{
			cout<<"recvfrom error"<<endl;
		}

		int ackno = (int) rcvbuf[1];
		unsigned short rcblockno = ((int) rcvbuf[2])*256 + ((int) rcvbuf[3]);

		cout<<"rcblockno = "<<rcblockno<<" "<<"blockno = "<<blockno<<endl;

		
		if(ackno!=4 || rcblockno!=blockno)
		{
			if (ackno == 5)
			{
				cout<<"ERROR OCCURED"<<endl;
				exit(1);
			}
			cout<<"ACKNOWLEDGE ERROR"<<endl;
			exit(1);
		}

		blockno++;

		unsigned char sendbuffer[SIZE];

		sendbuffer[0] =  0;
		sendbuffer[1] =  DATA;
		sendbuffer[2] =  (blockno/256);
		sendbuffer[3] =  (blockno%256);

		int byteread = fread(sendbuffer+4,1,512,fp);

		int sen = sendto(clientsd,sendbuffer,byteread+4,0,(struct sockaddr *)&server,srvrlen);
		if (sen<0)
		{
			cout<<"ERROR WHILE UPLOADING"<<endl;
			exit(1);
		}

		blockcount++;
		if (byteread<512)
		{
			break;
		}
	}

	cout<<"BLOCKS UPLOADED = "<<blockcount<<endl;
	fclose(fp);
	cout<<"UPLOADING SUCCESSFUL"<<endl;

}

void download(char filename[])
{

	FILE* fp;
	fp = fopen(filename,"w");

	unsigned char buffer[SIZE];
	memset(buffer,0,sizeof(buffer));
	
	int len = strlen(filename);

    const char* MODE;
    MODE = "octet";

    int modlen = strlen(MODE);

	buffer[0] =  0;
	buffer[1] =  RRQ;
	memcpy(&buffer[2],filename,len);
	memcpy(&buffer[2+len+1],MODE,modlen);

	int length = 9+len;

	cout<<"SENDING"<<endl;
	int rp = sendto(clientsd,buffer,length,0,(struct sockaddr *)&client,slen);
    if(rp<0)
    { 
       cout<<"Writing datagram error"<<endl;
    }

    struct sockaddr_in server;
    socklen_t srvrlen = sizeof(server);
 
 	unsigned char rcvbuf[SIZE];
    unsigned short blockno = 1;
    long int blockcount = 0;

    while(1)
    {
		memset(rcvbuf,0,sizeof(rcvbuf));
		int rc = recvfrom(clientsd,rcvbuf,SIZE,0,(struct sockaddr *)&server,&srvrlen);		
		if (rc<0)
		{
			cout<<"recvfrom error"<<endl;
		}

		rc = rc-4;

		int opcode = (int) rcvbuf[1];
		unsigned short rcblockno = ((int) rcvbuf[2])*256 + (int) rcvbuf[3];

		cout<<"rcblockno = "<<rcblockno<<" "<<"blockno = "<<blockno<<endl;

	
		if(opcode!=3 || rcblockno!=blockno)
		{
			if (opcode == 5)
			{
				cout<<"ERROR OCCURED"<<endl;
				exit(1);
			}
			cout<<"ACKNOWLEDGE ERROR"<<endl;
			exit(1);
		}

	
		fwrite(&rcvbuf[4],1,rc,fp);

		unsigned char sendbuffer[SIZE];

		sendbuffer[0] =  0;
		sendbuffer[1] = ACK;
		sendbuffer[2] = (blockno/256);
		sendbuffer[3] = (blockno%256);

		blockno++;

		int sen = sendto(clientsd,sendbuffer,4,0,(struct sockaddr *)&server,srvrlen);
		if (sen<0)
		{
			cout<<"ERROR WHILE DOWNLOADING"<<endl;
			exit(1);
		}

		blockcount++;
		if (rc<512)
		{
			break;
		}

	}

	cout<<"BLOCKS DOWNLOADED = "<<blockcount<<endl;
	fclose(fp);
	cout<<"DOWNLOADING SUCCESSFUL"<<endl;

}