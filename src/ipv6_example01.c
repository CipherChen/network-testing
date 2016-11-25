/* -*- c-file-style: "linux" -*- */

/* Simple IPv6 example, originally borrowed from stackoverflow:
    http://stackoverflow.com/questions/2455762/why-cant-i-bind-ipv6-socket-to-a-linklocal-address
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>  /* inet_pton(3) */
#include <unistd.h>     /* write(3) */
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_socket.h"

void error(char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	int sock, length, n;
	socklen_t fromlen;
	struct sockaddr_in6 server;
	struct sockaddr_in6  from;

	int portNr = 5555;
	char buf[1024];
	char ipv6[INET6_ADDRSTRLEN]; /* see man inet_ntop(3) */

	printf("Simple IPv6 UDP example (LISTEN'ing on UDP port %d)\n", portNr);

	length = sizeof (struct sockaddr_in6);

	sock=Socket(AF_INET6, SOCK_DGRAM, 0);

	bzero((char *)&server, length);
	server.sin6_family=AF_INET6;
	server.sin6_addr=in6addr_any;
	server.sin6_port=htons(portNr);

	inet_pton( AF_INET6, "::", (void *)&server.sin6_addr.s6_addr);
	//inet_pton( AF_INET6, "fe80::21f:29ff:feed:2f7e", (void *)&server.sin6_addr.s6_addr);
	//inet_pton( AF_INET6, "::1", (void *)&server.sin6_addr.s6_addr);

	if (bind(sock,(struct sockaddr *)&server,length)<0)
		error("binding");

	fromlen = sizeof(struct sockaddr_in6);
	while (1) {
		printf("- Waiting on recvfrom()\n");
		n = recvfrom(sock,buf,1024,0,(struct sockaddr *)&from,&fromlen);
		if (n < 0) error("recvfrom");
		if (!inet_ntop(AF_INET6, (void*)&from.sin6_addr,
			       ipv6, sizeof(ipv6)))
			error("inet_ntop");
		else
			printf("From (from.sin6_addr) = %s\n", ipv6);
		write(1,"- Received a datagram: ",23);
		write(1,buf,n);
		n = sendto(sock,"Got your message\n",17,
			   0,(struct sockaddr *)&from,fromlen);
		if (n  < 0) error("sendto");
	}
}
