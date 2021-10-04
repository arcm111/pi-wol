#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT "9"

int initSocket()
{
	int sockfd, opt = 1;
	if ((sockfd = socket (AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror ("socket");
		return -1;
	}
	if (setsockopt (sockfd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof opt) == -1)
	{
		perror ("setsockopt");
		return -1;
	}
	return sockfd;
}

int sendMagicPacket (unsigned char *packet, int len, const char *ip)
{
	struct sockaddr_in addr;
	struct hostent *host;
	struct sockaddr *dest = (struct sockaddr *) &addr;
	int sockfd, count;

	if ((sockfd = initSocket()) == -1)
	{
		fprintf (stderr, "Failed to initialize socket\n");
		return 0;
	}
	if ((host = gethostbyname (ip)) == NULL)
	{
		perror ("gethostbyname");
		return 0;
	}
	memset (addr.sin_zero, '\0', sizeof addr.sin_zero);
	addr.sin_family = AF_INET;
	addr.sin_port = htons (PORT);
	addr.sin_addr = *((struct in_addr *) host->h_addr);
	if ((count = sendto (sockfd, packet, len, 0, dest, sizeof addr)) == -1)
	{
		perror ("sendto");
		return 0;
	}
	printf ("sent %d bytes to %s\n", count, inet_ntoa (addr.sin_addr));
	close (sockfd);
	return 1;
}
