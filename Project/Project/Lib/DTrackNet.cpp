/* DTrackNet: C/C++ source file, A.R.T. GmbH 3.5.07-16.1.12
 *
 * Functions for receiving and sending UDP/TCP packets
 * Copyright (C) 2007-2012, Advanced Realtime Tracking GmbH
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Version v2.2.0
 *
 * $Id: DTrackNet.cpp,v 1.11 2012/01/19 10:36:57 christoph Exp $
 */

#include "DTrackNet.h"

#include <stdlib.h>
#include <stdio.h>

// internal socket type
struct _ip_socket_struct {
#ifdef OS_UNIX
	int ossock;		// Unix socket
#endif
#ifdef OS_WIN
	SOCKET ossock;	// Windows Socket
#endif
};

namespace DTrackSDK_Net {

/**
 * 	\brief	Initialize network ressources
 */
void net_init(void)
{
#ifdef OS_WIN
	// initialize socket dll (only Windows):
	WORD vreq;
	WSADATA wsa;// internal socket type
	vreq = MAKEWORD(2, 0);
	if (WSAStartup(vreq, &wsa) != 0)
	{
		return;
	}
#endif
}


/**
 * 	\brief	Free network ressources
 */
void net_exit(void)
{
#ifdef OS_WIN
	WSACleanup();
#endif
}


/**
 * 	\brief	Convert string to IP address.
 *
 *	@param[in]	name	IPv4 dotted decimal address or hostname
 *	@return 	IP address, 0 if error occured
 */
unsigned int ip_name2ip(const char* name)
{
	unsigned int ip;
	struct hostent* hent;
	// try if string contains IP address:
	if ((ip = inet_addr(name)) != INADDR_NONE)
	{
		return ntohl(ip);
	}
	// try if string contains hostname:
	hent = gethostbyname(name);
	if (!hent)
	{
		return 0;
	}
	return ntohl(((struct in_addr *)hent->h_addr)->s_addr);
}


// ---------------------------------------------------------------------------------------------------
// Handling UDP data:
// ---------------------------------------------------------------------------------------------------

/**
 * 	\brief	Initialize UDP socket.
 *
 *	@param[out]		sock	socket number
 *	@param[in,out]	port	port number, 0 if to be chosen by the OS
 *	@param[in]		ip		multicast ip to listen
 *	@return 0 if ok, < 0 if error occured
 */
int udp_init(void** sock, unsigned short* port, unsigned int ip)
{
	struct _ip_socket_struct* s;
	struct sockaddr_in addr;
#ifdef OS_UNIX
	socklen_t addrlen;
#endif
#ifdef OS_WIN
	int addrlen;
#endif
	s = (struct _ip_socket_struct *)malloc(sizeof(struct _ip_socket_struct));
	if (s == NULL)
	{
		return -11;
	}
	// create socket:
#ifdef OS_UNIX
	s->ossock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s->ossock < 0)
	{
		free(s);
		return -2;
	}
#endif
#ifdef OS_WIN
	s->ossock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s->ossock == INVALID_SOCKET)
	{
		free(s);
		return -2;
	}
#endif
	if (ip != 0) {
		// set reuse port to on to allow multiple binds per host
		int flag_on = 1;
		if ((setsockopt(s->ossock, SOL_SOCKET, SO_REUSEADDR, (char*)&flag_on, sizeof(flag_on))) < 0)
		{
			perror("setsockopt() failed1");
			udp_exit(s);
			return -4;
		}
	}

	// name socket:
	addr.sin_family = AF_INET;
	addr.sin_port = htons(*port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addrlen = sizeof(addr);
	if (bind(s->ossock, (struct sockaddr *)&addr, addrlen) < 0)
	{
		udp_exit(s);
		return -3;
	}
	if (*port == 0)
	{
		// port number was chosen by the OS
		if (getsockname(s->ossock, (struct sockaddr *)&addr, &addrlen))
		{
			udp_exit(s);
			return -3;
		}
		*port = ntohs(addr.sin_port);
	}

	if (ip != 0) {
		// construct an IGMP join request structure
		struct ip_mreq ipmreq;
		ipmreq.imr_multiaddr.s_addr = htonl(ip);
		ipmreq.imr_interface.s_addr = htonl(INADDR_ANY);
		// send an ADD MEMBERSHIP message via setsockopt
		if ((setsockopt(s->ossock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&ipmreq, sizeof(ipmreq))) < 0)
		{
			perror("setsockopt() failed2");
			udp_exit(s);
			return -5;
		}
	}

	*sock = s;
	return 0;	
}


/**
 * 	\brief	Deinitialize UDP socket.
 *
 *	@param[in]	sock	socket number
 *	@param[in]	ip		multicast ip to drop
 *	@return	0 ok, -1 error
 */
int udp_exit(void* sock, unsigned int ip)
{
	int err;
	struct _ip_socket_struct* s = (struct _ip_socket_struct *)sock;
	if (sock == NULL)
	{
		return 0;
	}

	if (ip != 0) {
		struct ip_mreq ipmreq;
		ipmreq.imr_multiaddr.s_addr = htonl(ip);
		ipmreq.imr_interface.s_addr = htonl(INADDR_ANY);
		// send a DROP MEMBERSHIP message via setsockopt
		if ((setsockopt(s->ossock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&ipmreq, sizeof(ipmreq))) < 0)
		{
			perror("setsockopt() failed3");
		}
	}
#ifdef OS_UNIX
	err = close(s->ossock);
#endif
#ifdef OS_WIN
	err = closesocket(s->ossock);
	WSACleanup();
#endif
	free(sock);
	if(err < 0)
	{
		return -1;
	}
	return 0;
}


/**
 *	\brief	Receive UDP data.
 *
 *	Tries to receive one packet, as long as data is available.
 *	@param[in]	sock	socket number
 *	@param[out] buffer 	buffer for UDP data
 *	@param[in] 	maxlen	length of buffer
 *	@param[in]  tout_us timeout in us (micro sec)
 *	@return	number of received bytes, <0 if error/timeout occured
 */
int udp_receive(const void* sock, void *buffer, int maxlen, int tout_us)
{
	int nbytes, err;
	fd_set set;
	struct timeval tout;
	struct _ip_socket_struct* s = (struct _ip_socket_struct *)sock;
	// waiting for data:
	FD_ZERO(&set);
	FD_SET(s->ossock, &set);
	tout.tv_sec = tout_us / 1000000;
	tout.tv_usec = tout_us % 1000000;
	switch ((err = select(FD_SETSIZE, &set, NULL, NULL, &tout)))
	{
		case 1:
			break;        // data available
		case 0:
			return -1;    // timeout
		default:
	      return -2;    // error
	}
	// receiving packet:
	while (1)
	{	// receive one packet:
		nbytes = recv(s->ossock, (char *)buffer, maxlen, 0);
		if (nbytes < 0)
		{	// receive error
			return -3;
		}
		// check, if more data available: if so, receive another packet
		FD_ZERO(&set);
		FD_SET(s->ossock, &set);

		tout.tv_sec = 0;   // no timeout
		tout.tv_usec = 0;
		if (select(FD_SETSIZE, &set, NULL, NULL, &tout) != 1)
		{
			// no more data available: check length of received packet and return
			if (nbytes >= maxlen)
			{   // buffer overflow
				return -4;
			}
			return nbytes;
		}
	}
}


/**
 *	\brief	Send UDP data.
 *
 *	@param[in] 	sock	socket number
 *	@param[in] 	buffer	buffer for UDP data
 *	@param[in] 	len		length of buffer
 *	@param[in] 	ipaddr	IPv4 address to send data to
 *	@param[in] 	port	port number to send data to
 *	@param[in] 	tout_us	timeout in us (micro sec)
 *	@return	0 if ok, <0 if error/timeout occured
 */
int udp_send(const void* sock, void* buffer, int len, unsigned int ipaddr, unsigned short port, int tout_us)
{
	fd_set set;
	struct timeval tout;
	int nbytes, err;
	struct sockaddr_in addr;
	struct _ip_socket_struct* s = (struct _ip_socket_struct *)sock;
	// building address:
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(ipaddr);
	addr.sin_port = htons(port);
	// waiting to send data:
	FD_ZERO(&set);
	FD_SET(s->ossock, &set);
	tout.tv_sec = tout_us / 1000000;
	tout.tv_usec = tout_us % 1000000;
	switch ((err = select(FD_SETSIZE, NULL, &set, NULL, &tout)))
	{
		case 1:
			break;
		case 0:
			return -1;    // timeout
		default:
	      return -2;    // error
	}
	// sending data:
	nbytes = sendto(s->ossock, (const char* )buffer, len, 0, (struct sockaddr* )&addr,
	                (size_t )sizeof(struct sockaddr_in));
	if(nbytes < len)
	{	// send error
		return -3;
	}
	return 0;
}

// ---------------------------------------------------------------------------------------------------
// Handling TCP data:
// ---------------------------------------------------------------------------------------------------

/**
 *	\brief	Initialize client TCP socket.
 *
 *	@param[out] sock	socket number
 *	@param[in] 	ip		ip address of TCP server
 *	@param[in] 	port	port number of TCP server
 *	@return		0 if ok, <0 if error occured
 */
int tcp_client_init(void** sock, unsigned int ip, unsigned short port)
{
	struct _ip_socket_struct* s;
	struct sockaddr_in addr;
	s = (struct _ip_socket_struct *)malloc(sizeof(struct _ip_socket_struct));
	if(s == NULL)
	{
		return -11;
	}
	// initialize socket dll (only Windows):
#ifdef OS_WIN
	{
		WORD vreq;
		WSADATA wsa;
		vreq = MAKEWORD(2, 0);
		if (WSAStartup(vreq, &wsa) != 0)
		{
			free(s);
			return -1;
		}
	}
#endif
	// create socket:
#ifdef OS_UNIX
	s->ossock = socket(PF_INET, SOCK_STREAM, 0);
	if (s->ossock < 0)
	{
		free(s);
		return -2;
	}
#endif
#ifdef OS_WIN
	s->ossock = socket(PF_INET, SOCK_STREAM, 0);
	if (s->ossock == INVALID_SOCKET)
	{
		WSACleanup();
		free(s);
		return -2;
	}
#endif
	// connect with server:
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(ip);
	addr.sin_port = htons(port);
	if ((connect(s->ossock, (struct sockaddr *)&addr, (size_t )sizeof(addr))))
	{
		tcp_exit(s);
		return -3;
	}
	*sock = s;
	return 0;
}


/**
 * 	\brief	Deinitialize TCP socket
 *
 *	@param[in]	sock	socket number
 *	@return		0 ok, -1 error
 */
int tcp_exit(void* sock)
{
	int err;
	struct _ip_socket_struct* s = (struct _ip_socket_struct *)sock;
	if (sock == NULL)
	{
		return 0;
	}
#ifdef OS_UNIX
	err = close(s->ossock);
#endif
#ifdef OS_WIN
	err = closesocket(s->ossock);
	WSACleanup();
#endif
	free(sock);
	if (err < 0)
	{
		return -1;
	}
	return 0;
}


/**
 * 	\brief	Receive TCP data.
 *
 *	@param[in] 	sock	socket number
 *	@param[out] buffer	buffer for TCP data
 *	@param[in]	maxlen	length of buffer
 *	@param[in]	tout_us	timeout in us (micro sec)
 *	@return		number of received bytes, <0 if error/timeout occured, -9 broken connection
 */
int tcp_receive(const void* sock, void *buffer, int maxlen, int tout_us)
{
	int nbytes, err;
	fd_set set;
	struct timeval tout;
	struct _ip_socket_struct* s = (struct _ip_socket_struct *)sock;
	// waiting for data:
	FD_ZERO(&set);
	FD_SET(s->ossock, &set);
	tout.tv_sec = tout_us / 1000000;
	tout.tv_usec = tout_us % 1000000;
	switch ((err = select(FD_SETSIZE, &set, NULL, NULL, &tout)))
	{
		case 1:
			break;        // data available
		case 0:
			return -1;    // timeout
		default:
	      return -2;    // error
	}
	// receiving packet:
	nbytes = recv(s->ossock, (char *)buffer, maxlen, 0);
	if (nbytes == 0)
	{	// broken connection
		return -9;
	}
	if (nbytes < 0)
	{	// receive error
		return -3;
	}
	if (nbytes >= maxlen)
	{	// buffer overflow
		return -4;
	}
	return nbytes;
}


/**
 * 	\brief	Send TCP data.
 *
 *	@param[in] 	sock	socket number
 *	@param[in] 	buffer	buffer for TCP data
 *	@param[in] 	len		length of buffer
 *	@param[in] 	tout_us	timeout in us (micro sec)
 *	@return	0 if ok, <0 if error/timeout occured
 */
int tcp_send(const void* sock, const void* buffer, int len, int tout_us)
{
	fd_set set;
	struct timeval tout;
	int nbytes, err;
	struct _ip_socket_struct* s = (struct _ip_socket_struct *)sock;
	// waiting to send data:
	FD_ZERO(&set);
	FD_SET(s->ossock, &set);
	tout.tv_sec = tout_us / 1000000;
	tout.tv_usec = tout_us % 1000000;
	switch ((err = select(FD_SETSIZE, NULL, &set, NULL, &tout)))
	{
		case 1:
			break;
		case 0:
			return -1;    // timeout
		default:
	      return -2;    // error
	}
	// sending data:
	nbytes = send(s->ossock, (const char* )buffer, len, 0);
	if (nbytes < len)
	{	// send error
		return -3;
	}
	return 0;
}

} // end namespace
