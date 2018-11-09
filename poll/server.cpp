/*************************************************************************
	> File Name: server.cpp
	> Author:xuelong 
	> Mail: yousuchen@126.com
	> Created Time: 三 11/ 7 11:11:24 2018
 ************************************************************************/

#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>

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
    
    struct pollfd pollfds[1024];
    int max_socket_fd = 0;
    pollfds[0].fd = sock;
    pollfds[0].events = POLLIN; 

    for(int index = 1; index < 1024; index++)
    {
        pollfds[index].fd = -1;
    }

    while(1)
    {
   	    cout<<"start poll"<<endl;
        int ret = poll(pollfds,max_socket_fd+1, 3000);

        if(ret < 0 )
        {
            cout<<"poll error"<<endl;
            break;
        }

        if(ret == 0)
        {
            cout<<"timeout"<<endl;
            continue;
        }

        cout<<"poll return "<<ret<<endl;

        for(int index = 1; index < 1024; index++)
        {
            if(pollfds[index].fd<0)
                continue;
            if(pollfds[index].revents != POLLRDNORM)
                continue;

            memset(recvBuffer,0,bufferSize);
            ret = recv(pollfds[index].fd,recvBuffer,bufferSize,0);
            if(ret <= 0)
            {
                cout<<"recv error "<<pollfds[index].fd<<endl;
                shutdown(pollfds[index].fd,SHUT_RDWR);
                close(pollfds[index].fd);
                pollfds[index].fd = -1;
                continue;
            }
            cout<<"recv message "<<recvBuffer<<endl;
            
            ret = send(pollfds[index].fd,sendBuffer,strlen(sendBuffer),0);
            if(ret <= 0)
            {
                cout<<"send message error "<<pollfds[index].fd<<endl;
                shutdown(pollfds[index].fd,SHUT_RDWR);
                close(pollfds[index].fd);
                pollfds[index].fd = -1;
                continue;
            }
            cout<<"send message "<<sendBuffer<<endl;           
        }

        if(pollfds[0].revents & POLLIN == POLLIN)
        {
            cout<<"start accept"<<endl;
		    int clientFd = accept(pollfds[0].fd,NULL,NULL);
            if(clientFd <= 0) 
            {
                cout<<"accept fail "<<endl;
                continue;
            }
            cout<<"accept success"<<endl;
            ret = send(clientFd,sendBuffer,strlen(sendBuffer),0);
            cout<<"first send"<<ret<<endl;

            for(int index = 1; index< 1024; index++)
            {
                if(pollfds[index].fd < 0)
                {
                    pollfds[index].fd = clientFd;
		            pollfds[index].events = POLLRDNORM;
                    if(index> max_socket_fd) max_socket_fd = index;
                    break;
                }
            }
        }

    }
    
    cout<<"exit"<<endl;
    return 0;
}

