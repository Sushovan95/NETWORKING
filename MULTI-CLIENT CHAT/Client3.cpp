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
#include <pthread.h>

using namespace std;

char chat[1200];
char chatemp[1200];

struct descrip
{
    int newsd;
};

void* receive(void* );

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        cout << "Proper input is not given" << endl;
        return 0;
    }


    char *ip = argv[1];
    int port = atoi(argv[2]);

    struct sockaddr_in sendsockaddr;
    sendsockaddr.sin_family = AF_INET;
    sendsockaddr.sin_addr.s_addr = inet_addr(ip);
    sendsockaddr.sin_port = htons(port);

    int clientsd = socket(AF_INET, SOCK_STREAM, 0);

    int connectsd = connect(clientsd,(struct sockaddr*) &sendsockaddr, sizeof(sendsockaddr));

    if(connectsd< 0)
    {
        cout<<"Socket Error"<<endl;
        return 0;
    }
    cout << "Connected" << endl;

    int threadread;
    pthread_t tid;
   
   struct descrip* data;
   data = (struct descrip *) malloc(sizeof(struct descrip));
   data->newsd = clientsd;

    threadread = pthread_create(&tid, NULL, receive,data);
    if(threadread!=0)
    {
		cout<<"Error while sending"<<threadread<<endl;
        return 0;
    }

    while(1)
	{
		 
         string s;
         getline(cin,s);
         strcpy(chat,s.c_str());
         send(clientsd , (char*)&chat , strlen(chat), 0 );
         if(s=="exit")
         {
			cout<<"End From Client Side:"<<endl;
			return 0;
		 }
		memset(&chat,0,sizeof(chat));
	}

    pthread_join(tid,NULL);
    close(clientsd);
    cout << "Connection closed" << endl;
    return 0;
}

void* receive(void* arg)
{
    struct descrip* data;
    data = (struct descrip *)arg;

    int clientsd = data->newsd;
  
	while(1)
    {

        recv(clientsd, (char*)&chat, sizeof(chat), 0);
        int len = strlen(chat);

        for(int i=0;i<strlen(chat)-2;i++)
        {
            chatemp[i] = chat[i];
        }
        
    if(chat[len-2]!= '@')
    {
        cout<<chatemp<<endl;
    }

    else
    {
		cout<<"CLIENT"<<chat[len-1]<<": "<<chatemp<<endl;
    }
	
    memset(&chat,0,sizeof(chat));
    memset(&chatemp,0,sizeof(chatemp));

	}

}
