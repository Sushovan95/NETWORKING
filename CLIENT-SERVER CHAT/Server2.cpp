#include <iostream>
#include <string.h>
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
#include <bits/stdc++.h>
#include <pthread.h>
using namespace std;

struct descrip
{
    int newsd;
};

char chat[1200];

void* receive(void* );

int main(int argc, char *argv[])
{
  if(argc != 2)
  {
      cout << "Usage: port" << endl;
      exit(0);
  }

    int port = atoi(argv[1]);
	  sockaddr_in servaddr;


    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    
    int serversd = socket(AF_INET, SOCK_STREAM, 0);

    if(serversd < 0)
    {
        cout << "Socket Error" << endl;
        exit(0);
    }

    int bindsd = bind(serversd, (struct sockaddr*) &servaddr, sizeof(servaddr));
    if(bindsd < 0)
    {
        cout << "Binding Error" << endl;
        exit(0);
    }

    cout << "Waiting" << endl;
    
    listen(serversd, 5);
    

    sockaddr_in newsockaddr;
    socklen_t newsockaddrsize = sizeof(newsockaddr);

    int newsd = accept(serversd, (sockaddr *)&newsockaddr, &newsockaddrsize);
    if(newsd < 0)
    {
        cout << "Error establishing connection" << endl;
        exit(0);
    }
    cout << "Connection Established" << endl;

    pthread_t tid; 
    struct descrip* data;
    data = (struct descrip*) malloc(sizeof(struct descrip));
    data->newsd = newsd;

    int threadread = pthread_create(&tid, NULL, receive, data);
    
    if(threadread!=0)
    {
        cout<<"Error while sending"<<threadread<<endl;;
        return 0;
    }
		while(1)
		{
        memset(&chat,0,sizeof(chat));
		     string s;
         getline(cin,s);
         strcpy(chat,s.c_str());
         send(newsd , (char*)&chat , strlen(chat), 0 );
         
         if(s=="exit")
         {
			        cout<<"End From Server Side"<<endl;
			        exit(0);
			   }
		     
         memset(&chat,0,sizeof(chat));
	 }
	  
    pthread_join(tid,NULL);
    close(newsd);
    close(serversd);
    cout << "********Session********" << endl;
    cout << "Connection closed..." << endl;
    return 0;
}

void* receive(void *arg)
{
  struct descrip* data;
  data = (struct descrip*)arg;
  int newsd = data->newsd;

	while(1)
  {
    memset(&chat,0,sizeof(chat));
    recv(newsd, (char*)&chat, sizeof(chat), 0);
		if(!strcmp(chat,"exit"))
    {
			cout<<"End From Client Side"<<endl;
			exit(0);
		}
		cout<<"Client:"<<chat<<endl;
		memset(&chat,0,sizeof(chat));
	}

}
