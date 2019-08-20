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

char chat[1200];
char chatemp1[1200];
char chatemp2[1200];
char chatemp3[1200];
char chatemp4[1200];


struct descrip
{
    int newsd;
};

vector<int>arr;
void* connection(void* );

int main(int argc,char* argv[])
{
		if(argc != 2)
		{
				cout << "Usage: port" << endl;
				exit(0);
		}


	int port = atoi(argv[1]);

	string str = "Ooopss..! YOUR MESSAGE SHOULD BE IN THE FORM OF:'<TEXTBODY>@<CLIENT_ID>'  ";
	string st = "LOL..! YOU ARE TRYING TO MESSAGE YOURSELF  ";
	string s1 = "YOUR CLIENT IS OFFLINE NOW  ";
	string s4 = "CLIENT EXITED FROM CHAT  ";

	strcpy(chatemp1,str.c_str());
	strcpy(chatemp2,st.c_str());
	strcpy(chatemp3,s1.c_str());
	strcpy(chatemp4,s4.c_str());

	struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

	int serversd = socket(AF_INET, SOCK_STREAM, 0);                               //socket()
    if(serversd < 0)
    {
        cout << "Socket Error" << endl;
        exit(0);
    }

    int bindsd = bind(serversd, (struct sockaddr*) &servaddr, sizeof(servaddr)); //bind()
    if(bindsd < 0)
    {
        cout << "Binding Error" << endl;
        exit(0);
    }
    cout << "Waiting" << endl;
    listen(serversd, 5);                                                          //listen()

	int thread;
	pthread_t tid;
    int newsd;
	sockaddr_in newsockaddr;
    socklen_t newsockaddrsize = sizeof(newsockaddr);

    while((newsd = accept(serversd, (sockaddr *)&newsockaddr, &newsockaddrsize)))  //accept()
    {
    	struct descrip* data;
    	data = (struct descrip*)malloc(sizeof(struct descrip));
    	data->newsd = newsd;
		
		int flag=0;
		int k;

	    for(int i=0;i<arr.size();i++)
	    {
		    if(arr[i]==newsd)
		        {
				    flag=1;
				    break;
			    }
		}

	    if(flag==0)
	    {
            k=arr.size()+1;
			arr.push_back(newsd);
	    }

       cout << "Connection Established with client" <<k<< endl;
       thread=pthread_create(&tid, NULL, connection,data);

   }

   if(newsd < 0)
    {
        cout << "Error establishing connection" << endl;
        exit(0);
    }

    pthread_join(tid,NULL);
    close(newsd);
    close(serversd);
    cout << "********Session********" << endl;
    cout << "Connection closed..." << endl;
    return 0;
}

void* connection(void* arg) //function which is called by the thread
{
	struct descrip* data;
	data = (struct descrip*) arg;

	int newsd = data->newsd;

	while(1)
		{

		int k;
		int flag = 0;
		recv(newsd, (char*)&chat, sizeof(chat), 0);
		int len = strlen(chat);
		if(strcmp(chat,"exit") == 0)
		{
			int t;
			cout<<"End From Client "<<endl;
			for(int i=0;i<arr.size();i++)
			{  
		 	  if(newsd==arr[i])
		      {
		      	t = i;
		       	break;
		      } 
			}

			arr[t] = -1;
			memset(&chat,0,sizeof(chat));
			pthread_exit(NULL);
		}

		for(int i=0;i<arr.size();i++)
		 {  
		 	if(newsd==arr[i])
		      {
		      	flag++;
           		k=i+1;
		       	break;
		      }
		 }


		int c = chat[len-1] - '0'; //converting into int from char
		char c1 = k + '0'; //converting into character from int
		chat[len-1] = c1;

		if (flag == 0 || arr[c-1] == -1)
		{
			send(newsd,(char*)&chatemp4,strlen(chatemp4), 0 );
		}

	 	if(chat[len-2]!= '@')
		{
			send(newsd,(char*)&chatemp1,strlen(chatemp1), 0 );
		}

		else if(arr[c-1] == newsd)
		{
			send(newsd,(char*)&chatemp2,strlen(chatemp2), 0 );
		}

		else if(c>arr.size())
		{
			send(newsd,(char*)&chatemp3,strlen(chatemp3), 0 );
		}

	  	else
     	{
			send(arr[c-1], (char*)&chat , strlen(chat), 0 );
	    }
		
		memset(&chat,0,sizeof(chat));
	 }
}
