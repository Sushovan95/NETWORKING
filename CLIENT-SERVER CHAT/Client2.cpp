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

struct descrip
{
    int newsd;
};

char chat[1200];
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

    sockaddr_in sendsockaddr;
    
    sendsockaddr.sin_family = AF_INET;
    sendsockaddr.sin_addr.s_addr = inet_addr(ip);
    sendsockaddr.sin_port = htons(port);
    
    int clientsd = socket(AF_INET, SOCK_STREAM, 0);

    int connectsd = connect(clientsd,(sockaddr*) &sendsockaddr, sizeof(sendsockaddr));

    if(connectsd< 0)
    {
        cout<<"Socket Error"<<endl;
        return 0;
    }
    cout << "Connected" << endl;

    pthread_t tid;

    struct descrip* data;
    data = (struct descrip*) malloc(sizeof(struct descrip));
    data->newsd = clientsd;
    
    int threadread = pthread_create(&tid, NULL, receive,data);
    if(threadread!=0)
    {
		    cout<<"Error while sending"<<threadread<<endl;
        return 0;
    }

    while(1)
    {
         memset(&chat,0,sizeof(chat));
         string s;
         getline(cin, s);
         strcpy(chat,s.c_str());
         send(clientsd , (char*)&chat , strlen(chat), 0 );
         if(s=="exit")
         {
            cout<<"End From Server Side"<<endl;
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
    data = (struct descrip*)arg;
    int clientsd = data->newsd;

	while(1)
    {
        memset(&chat,0,sizeof(chat));
        recv(clientsd, (char*)&chat, sizeof(chat), 0);
		if(!strcmp(chat, "exit"))
        {
			cout<<"End From Server Side"<<endl;
			exit(0);
		}
		cout<<"Server:"<<chat<<endl;
		memset(&chat,0,sizeof(chat));
	}

}
