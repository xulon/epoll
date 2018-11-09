/*************************************************************************
	> File Name: client.cpp
	> Author:xuelong 
	> Mail: yousuchen@126.com
	> Created Time: 三 11/ 7 15:09:56 2018
 ************************************************************************/

#include<iostream>
using namespace std;

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <list>

int main(int argc, char *argv[])
{
	int client_sockfd;
	int len;
	struct sockaddr_in remote_addr; 
	char buf[BUFSIZ];  
	memset(&remote_addr,0,sizeof(remote_addr));
	remote_addr.sin_family=AF_INET;
	remote_addr.sin_addr.s_addr=htons(INADDR_ANY);
	remote_addr.sin_port=htons(10005); 

	int count = 1000;
	int maxFdSet = 0;
	list<int> list_fd;
	while(count--)
	{
		if((client_sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
		{
			cout<<"socket error"<<endl;
			continue;
		}

		if(connect(client_sockfd,(struct sockaddr *)&remote_addr,sizeof(struct sockaddr))<0)
		{
			cout<<"connect error"<<endl;
			continue;
		}

		if(client_sockfd > maxFdSet)
		{
			maxFdSet = client_sockfd;
		}

	
		list_fd.push_back(client_sockfd);
	}

	fd_set fdset;
	struct timeval tv;

	while(1)
	{
		FD_ZERO(&fdset);
		for(list<int>::iterator it = list_fd.begin();it != list_fd.end(); it++)
		{
			FD_SET(*it,&fdset);
		}

		tv.tv_sec = 5;
		tv.tv_usec = 0;

		int ret = select(maxFdSet+1,&fdset,NULL,NULL,&tv);

		if(ret < 0)
		{
			cout<<"select error"<<endl;
			break;
		}

		else if(ret == 0)
		{
			cout<<"timeout"<<endl;
			continue;
		}

		for(list<int>::iterator it = list_fd.begin(); it != list_fd.end(); it++)
		{
			if(FD_ISSET(*it,&fdset))
			{
				ret = recv(*it,buf,BUFSIZ,0);
				if(ret <= 0)
				{
					cout<<"recv error "<<*it<<endl;
					list_fd.erase(it++);
					continue;
				}
				            			
				sleep(1);
				ret = send(*it,"i am client",12,0);

				if(ret <= 0)
				{
					cout<<"send error "<<*it<<endl;
					list_fd.erase(it++);
					continue;
				}

				it++;
			}
		}
	}
	return 0;
}
