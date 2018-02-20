#include "network.h"


#include "GeneralEngineCPP.h"
#ifdef outdatedOS

#define WIN32_LEAN_AND_MEAN 
#include <winsock2.h>
#include <windows.h>

#define read(a,b,c) ReadFile(a,b,c)

#define close(socket) closesocket(socket)

#else

#include <unistd.h>  
#include <netinet/in.h> 
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>


#endif

#include <stdlib.h> 
#include <cstring>   
#include <string>


struct GE_NetworkSocket
{
	int socketfd;
	struct sockaddr_in addr;
};

GE_NetworkSocket* GE_CreateNetworkSocket()
{
	return new GE_NetworkSocket{};
}

int GE_FillNetworkSocket(GE_NetworkSocket* aSocket, unsigned short port)
{
	aSocket->socketfd = socket(AF_INET, SOCK_STREAM, 0);

	if (aSocket->socketfd < 0 )
	{
		return aSocket->socketfd;
	}
	memset(&(aSocket->addr),0,sizeof(aSocket->addr));

	aSocket->addr.sin_family = AF_INET;
	aSocket->addr.sin_addr.s_addr = INADDR_ANY;
	aSocket->addr.sin_port = htons(port);
	return 0;

}
int GE_BindServer(GE_NetworkSocket* nSocket)
{
	int bindError = bind(nSocket->socketfd, (struct sockaddr *) &nSocket->addr, sizeof(nSocket->addr));

	if (bindError < 0)
	{
		return bindError;
	}

	return 0;
}
int GE_ConnectServer(GE_NetworkSocket* sSocket, char* hostname)
{
	printf("hn %s\n",hostname);
	struct hostent *server = gethostbyname(hostname);
	if (server == NULL) 
	{
		return 1;
	}
	memmove((void *)server->h_addr, (void *)&(sSocket->addr).sin_addr.s_addr, server->h_length);
	int connectError = connect(sSocket->socketfd, (struct sockaddr*)&sSocket->addr, sizeof(sSocket->addr));
	if (connectError < 0 )
	{
		return connectError;
	}

	return 0;
}

int GE_ConnectClient(GE_NetworkSocket* cSocket, GE_NetworkSocket* nSocket)
{
	listen(nSocket->socketfd,1);

	socklen_t size = sizeof(cSocket->addr);
	cSocket->socketfd = accept(nSocket->socketfd,(struct sockaddr *)&cSocket->addr, &size); 
	if (nSocket->socketfd == -1 )
	{
		return -1;
	}
	return 0;

}
int GE_Read(GE_NetworkSocket* cSocket, char* buffer, size_t buffersize)
{
	int success = read(cSocket->socketfd, buffer, buffersize-1);
	//buffer[buffersize] = '\0';
	return success; 
}
int GE_Write(GE_NetworkSocket* cSocket, char* buffer, size_t buffersize)
{
	int success = write(cSocket->socketfd,buffer,buffersize-1);
	//buffer[buffersize] = '\0';
	return success;
}

void GE_FreeNetworkSocket(GE_NetworkSocket* socket)
{
	close(socket->socketfd);
	delete socket;
}



