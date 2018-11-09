/*************************************************************************
	> File Name: server.cpp
	> Author:xuelong 
	> Mail: yousuchen@126.com
	> Created Time: 三 11/ 7 11:11:24 2018
 ************************************************************************/

#include<iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <list>

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
    
    fd_set fdaddr;
    struct timeval tv;
    int max_socket_fd = sock;
    list<int> socket_fd;

    while(1)
    {
        FD_ZERO(&fdaddr);
        FD_SET(sock,&fdaddr);
        for(list<int>::iterator it = socket_fd.begin(); it != socket_fd.end(); it++)
        {
            FD_SET(*it,&fdaddr);
        }

        tv.tv_sec = 5;
        tv.tv_usec = 0;
        
        int ret = select(max_socket_fd+1,&fdaddr,NULL,NULL,&tv);

        if(ret < 0 )
        {
            cout<<"select error"<<endl;
            exit(-1);
        }

        if(ret == 0)
        {
            cout<<"timeout"<<endl;
            continue;
        }

        cout<<"select return "<<ret<<endl;

        for(list<int>::iterator it = socket_fd.begin(); it != socket_fd.end();)
        {
            if(!FD_ISSET(*it,&fdaddr))
            {
		        it++;
		        continue;
	        }
            
            memset(recvBuffer,0,bufferSize);
            ret = recv(*it,recvBuffer,bufferSize,0);
            if(ret <= 0)
            {
                cout<<"recv error "<<*it<<endl;
                shutdown(*it,SHUT_RDWR);
                close(*it);
                it = socket_fd.erase(it);
                continue;
            }
            cout<<"recv message "<<recvBuffer<<" from "<<*it<<endl;
            
            ret = send(*it,sendBuffer,strlen(sendBuffer),0);
            if(ret <= 0)
            {
                cout<<"send message error "<<*it<<endl;
                shutdown(*it,SHUT_RDWR);
                close(*it);
                it = socket_fd.erase(it);
                continue;
            }
            cout<<"send message "<<sendBuffer<<endl;

            it++;
            
        }

        if(FD_ISSET(sock,&fdaddr))
        {
            cout<<"start accept"<<endl;

	        int new_fd = accept(sock,NULL,NULL);

	        cout<<"accept success "<<new_fd<<endl;

            if(new_fd <= 0) 
            {
                cout<<"accept error "<<new_fd<<endl;
            }

            else
            {
                ret = send(new_fd,sendBuffer,strlen(sendBuffer),0);
                if(ret <= 0)
                {
                    cout<<"first send error"<<endl;
                    continue;
                }

                socket_fd.push_back(new_fd);
                if(new_fd > max_socket_fd)
                {
                    max_socket_fd = new_fd;
                }
            }

        }

    }
    
    cout<<"exit"<<endl;
    return 0;
}

