/*************************************************************************
	> File Name: server.cpp
	> Author:chenxuelong 
	> Mail: chenxuelong@100tal.com
	> Created Time: 三 11/ 7 11:11:24 2018
 ************************************************************************/

#include<iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
using namespace std;

int main()
{
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock <0 )
    {
        cout<<"create socket fail";
        exit(-1);
    }
    
    cout<<"create socket success"<<endl;

    struct sockaddr_in socketAddr;
    memset(&socketAddr,0,sizeof(socketAddr));

    socketAddr.sin_family = AF_INET;
    socketAddr.sin_port = htons(10005);
    socketAddr.sin_addr.s_addr = htons(INADDR_ANY);

    if(bind(sock,(struct sockaddr*)&socketAddr,sizeof(struct sockaddr)) == -1)
    {
        cout<<"bind error";
        exit(-1);
    }

    cout<<"bind success"<<endl;

    if(listen(sock,10) == -1)
    {
        cout<<"listen error";
        exit(-1);
    }

    cout<<"listen success"<<endl;

    const int bufferSize = 1024;
    char recvBuffer[bufferSize] = {0};
    char sendBuffer[bufferSize] = "hello, i am server";

    while(1)
    {
	cout<<"start accept ppid:"<<getppid()<<" pid:"<<getpid()<<endl;
        int clientSock = accept(sock,NULL,NULL);
	cout<<"accept sock:"<<clientSock<<endl;
	pid_t fpid = fork();	
	if(fpid<0)
	{
	    cout<<"fork error"<<endl;		
	} 
       
	if(fpid !=0) continue;

	while(1)
        {
	    int ret = send(clientSock,sendBuffer,bufferSize,0);
            if(ret <= 0)
            {
                cout<<"send error:"<<ret<<endl;
                break;
            }
            cout<<"send message:"<<sendBuffer<<endl;

	    ret = recv(clientSock,recvBuffer,bufferSize,0);
            if(ret <= 0)
            {
                cout<<"recv error:"<<ret<<endl;
                break;
            }
            cout<<"recv message:"<<recvBuffer<<" sock:"<<clientSock<<endl;
        }
	shutdown(clientSock,SHUT_RDWR);
	close(clientSock);
	cout<<"close socket"<<endl;
	break;
    }
    cout<<"exit ppid:"<<getppid()<<"pid:"<<getpid()<<endl;
    return 0;
}

