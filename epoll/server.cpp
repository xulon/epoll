/*************************************************************************
	> File Name: server.cpp
	> Author:xuelong 
	> Mail: yousuchen@126.com
	> Created Time: 三 11/ 7 11:11:24 2018
 ************************************************************************/

#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

using namespace std;

#define MAXEVENTS 10

int create_socket_server()
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

    return sock;
}

int accept_socket(int server_socket)
{
    cout<<"start accept"<<endl;
    int client_socket = accept(server_socket,NULL,NULL);
    if(client_socket <= 0) 
    {
        cout<<"accept fail "<<endl;
        return -1;
    }
    cout<<"accept success"<<endl;
    
    int ret = send(client_socket,"i am server",12,0);
    if(ret <= 0)
    {
        cout<<"first send error"<<endl;
        return -1;
    }
    cout<<"first send "<<client_socket<<endl;

    return client_socket;
}

int socket_rdwr(int client_socket)
{
    static const int bufferSize = 1024;
    static char recvBuffer[bufferSize] = {0};
    static char sendBuffer[bufferSize] = "hello, i am server";

    memset(recvBuffer,0,bufferSize);
    int ret = recv(client_socket,recvBuffer,bufferSize,0);
    if(ret <= 0)
    {
        cout<<"recv error "<<client_socket<<endl;
        shutdown(client_socket,SHUT_RDWR);
        close(client_socket);
        return -1;
    }
    cout<<"recv message "<<recvBuffer<<endl;
    
    ret = send(client_socket,sendBuffer,strlen(sendBuffer),0);
    if(ret <= 0)
    {
        cout<<"send message error "<<client_socket<<endl;
        shutdown(client_socket,SHUT_RDWR);
        close(client_socket);
        return -1;
    }
    cout<<"send message "<<sendBuffer<<endl;   

    return 0; 
}

int main()
{
    int sock = create_socket_server();
    
    struct epoll_event ev, events[MAXEVENTS];
    int epollfd = epoll_create(1024);
    if(epollfd == -1)
    {
        cout<<"epoll_create fail "<<endl;
        exit(-1);
    }

    ev.events = EPOLLIN;
    ev.data.fd = sock;

    if(epoll_ctl(epollfd,EPOLL_CTL_ADD,sock,&ev) == -1)
    {
        cout<<"epoll add sock error"<<endl;
        exit(-1);
    }

    while(1)
    {
   	    cout<<"start epoll"<<endl;

        int nfds = epoll_wait(epollfd,events,MAXEVENTS,5000);

        if(nfds == -1 )
        {
            cout<<"epoll error"<<endl;
            exit(-1);
        }

        if(nfds == 0)
        {
            cout<<"timeout"<<endl;
            continue;
        }

        cout<<"epoll return "<<nfds<<endl;

        for(int index = 0; index < nfds; index++)
        {
            if(events[index].data.fd == sock)
            {
                int client_socket = accept_socket(sock);

                if(client_socket < 0)
                    continue;

                ev.data.fd = client_socket;
                ev.events = EPOLLIN | EPOLLET;

                if(epoll_ctl(epollfd,EPOLL_CTL_ADD,client_socket,&ev) == -1)
                {
                    cout<<"epoll ctl add error"<<endl;
                    exit(-1);
                }
            }
            else
            {
                int ret = socket_rdwr(events[index].data.fd);
                if(ret == 0) continue;
                if(epoll_ctl(epollfd,EPOLL_CTL_DEL,events[index].data.fd,&events[index]) == -1)
                {
                    cout<<"epoll delete error"<<endl;
                }  
            }                
        }
    }
    
    cout<<"exit"<<endl;
    return 0;
}

