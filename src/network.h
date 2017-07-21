#include <stdio.h> 
#include <string.h>   

//net includes
#include <unistd.h>  
#include <netinet/in.h> 
#include <stdlib.h> 
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <string>


#ifndef __NETWORK_INCLUDED
#define __NETWORK_INCLUDED 

struct GE_NetworkSocket
{
	//shouldn't be touched except by network.cpp
	int socketfd;
	struct sockaddr_in addr;
};


	
GE_NetworkSocket* GE_CreateNetworkSocket();
int GE_FillNetworkSocket(GE_NetworkSocket* aSocket, unsigned short port);

int GE_BindServer(GE_NetworkSocket* nSocket);

int GE_ConnectServer(GE_NetworkSocket* sSocket, char* hostname);
int GE_ConnectClient(GE_NetworkSocket* cSocket, GE_NetworkSocket* nSocket);

int GE_Read(GE_NetworkSocket* cSocket, char* buffer, size_t buffersize);
int GE_Write(GE_NetworkSocket* cSocket, char* buffer, size_t buffersize);

void GE_FreeNetworkSocket(GE_NetworkSocket* socket);

#endif // __NETWORK_INCLUDED
