/*!
 * @file
 * @author Jackson McNeill
 *
 * A small network abstraction layer, using Unix sockets. No windows equivilent available. 
 */
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

/*!
 * A TCP socket abstraction
 */
struct GE_NetworkSocket
{
	//shouldn't be touched except by network.cpp
	int socketfd;
	struct sockaddr_in addr;
};


/*!
 * Creates, but does not populate, a GE_NetworkSocket
 */
GE_NetworkSocket* GE_CreateNetworkSocket();

/*!
 * Fills a GE_NetworkSocket to be a TCP socket
 * @param aSocket The socket to fill
 * @param port The port the socket will be on
 *
 * @return 0 if okay, <0 if not okay
 */
int GE_FillNetworkSocket(GE_NetworkSocket* aSocket, unsigned short port);

/*!
 * Binds a socket to a port
 * @param nSocket The socket to bind
 * @return 0 if okay, <0 if not okay
 */
int GE_BindServer(GE_NetworkSocket* nSocket);

/*!
 * Connects a socket to a server
 * @param sSocket The socket that will be associated with the server
 * @param hostname The hostname of the server
 * @return 0 if okay, <0 if not okay
 */
int GE_ConnectServer(GE_NetworkSocket* sSocket, char* hostname);

/*!
 * Connects (listens for) a client to your server. Blocks. 
 * @param cSocket The socket that will be associated with the client
 * @param nSocket Your bound server socket
 * @return 0 if okay, <0 if not okay, 1 if server was NULL
 */
int GE_ConnectClient(GE_NetworkSocket* cSocket, GE_NetworkSocket* nSocket);

/*!
 * Reads to a socket
 * @param cSocket The socket to read on
 * @param buffer A buffer that will be filled with information
 * @param buffersize The size of buffer
 * @return 0 if okay, <0 if not okay
 */
int GE_Read(GE_NetworkSocket* cSocket, char* buffer, size_t buffersize);

/*!
 * Writes to a socket
 * @param cSocket The socket to write on
 * @param buffer Will be written to the socket
 * @param buffersize The size of buffer
 * @return 0 if okay, <0 if not okay
 */
int GE_Write(GE_NetworkSocket* cSocket, char* buffer, size_t buffersize);

/*!
 * Frees the memory taken by a network socket. Do not use a socket after freeing.
 * @param socket The network socket to free
 */
void GE_FreeNetworkSocket(GE_NetworkSocket* socket);

#endif // __NETWORK_INCLUDED
