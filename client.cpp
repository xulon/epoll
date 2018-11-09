/*************************************************************************
	> File Name: client.cpp
	> Author:xuelong 
	> Mail: yousuchen@126.com
	> Created Time: 三 11/ 7 15:09:56 2018
 ************************************************************************/

#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

using namespace std;

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

	int count = 500;
	while(count--)
	{
		pid_t fpid = fork();

		if(fpid < 0)
		{
			cout<<"fork error"<<endl;
			break;
		}

		if(fpid!=0)
			continue;

		if((client_sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
		{
			perror("socket error");
			return 1;
		}

		if(connect(client_sockfd,(struct sockaddr *)&remote_addr,sizeof(struct sockaddr))<0)
		{
			perror("connect error");
			return 1;
		}
		printf("connected to server\n");

		while(1)
		{
			len=recv(client_sockfd,buf,BUFSIZ,0);
			if(len <= 0)
			{
			    cout<<"recv error"<<client_sockfd<<endl;
			    break;
			}
			buf[len]='\0';
			printf("received:%s\n",buf);

			for(int index = 0;index<20; index++)
			{
				buf[index] = 'a'+index;
			}

			sleep(1);

			len=send(client_sockfd,buf,strlen(buf),0);
			if(len <= 0)
			{
			   cout<<"send error"<<client_sockfd<<endl;
			   break;
			}			
		}
		
		shutdown(client_sockfd,SHUT_RDWR);
		close(client_sockfd);
		printf("quit\n");
		break;
	}
  return 0;
}
