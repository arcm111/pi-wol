#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include "utils.h"
#include "sender.h"
#include "gpio.h"

#define MAXBUFLEN 1024
#define PORT "5649"
#define BACKLOG 10

int sockfd;
int quit = 0;
char bdcast[16] = "255.255.255.255";
extern int shtdwn;

void signalHandler (int signal)
{
	if (signal == SIGTERM || signal == SIGINT)
	{
		quit = 1;
		if (sockfd != -1) shutdown (sockfd, SHUT_RDWR);
		printf ("Terminated ...\n");
	}
}

void readConfigurations()
{
	char buf[256];
	char ip[256] = {0};
	FILE *conf;

	if ((conf = fopen ("/home/arcm/projects/c/wol-server/wol.conf", "r")) != NULL)
	{
		if (fgets (buf, sizeof buf, conf) != NULL)
		{
			if (sscanf (buf, "broadcast ip = %s\n", ip) == 1)
			{
				if (strlen (ip) > 6 && strlen (ip) < 16)
				{
					strncpy (bdcast, ip, 15);
					printf ("Bdcast IP: %s\n", bdcast);
				}
			}
		}
		fclose (conf);
	}
	else perror ("fopen");
}

void *get_in_addr (struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in *) sa)->sin_addr);
	}
	return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}

int startServer()
{
	int sockfd, rv;
	struct addrinfo hints, *servinfo, *p;

	memset (&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; /* allows both ipv4 and ipv6 */
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; /* use my ip address */
	if ((rv = getaddrinfo (NULL, PORT, &hints, &servinfo)) != 0)
	{
		fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (rv));
		return -1;
	}
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		sockfd = socket (p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sockfd == -1)
		{
			perror ("socket");
			continue;
		}
		if (bind (sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close (sockfd);
			perror ("bind");
			continue;
		}
		break;
	}
	if (p == NULL)
	{
		fprintf (stderr, "failed to bind to socket\n");
		return -1;
	}
	freeaddrinfo (servinfo);
	printf ("Server: Listening for new connections on port %s...\n", PORT);
	return sockfd;
}

void *wolHandler (void *arg)
{
	int sockfd = *((int *) arg);
	struct sockaddr_storage their_addr;
	struct sockaddr *addr = (struct sockaddr *) &their_addr;
	socklen_t len = sizeof their_addr;
	int count;
	unsigned char buf[MAXBUFLEN];
	char ip[INET6_ADDRSTRLEN];
	unsigned char mac[6] = {0};

	while ((count = recvfrom (sockfd, buf, MAXBUFLEN - 1, 0, addr, &len)) > 0)
	{
		inet_ntop (their_addr.ss_family, get_in_addr (addr), ip, sizeof ip);
		printf ("server: got packet from %s\n", ip);
		printf ("server: packet is %d bytes long\n", count);
		printBytes (buf, count, "Packet received");
		getMacAddress (buf, count, mac);
		printMacAdress (mac, 6, "MAC address");
		onCommand();
		if (!sendMagicPacket (buf, count, bdcast))
		{
			fprintf (stderr, "Failed to send packet to %s\n", bdcast);
			onError();
			break;
		}
	}
	if (count == -1)
	{
		perror ("recvfrom");
		onError();
	}
	close (sockfd);
	pthread_exit (NULL);
}

int main (void)
{
	int interval = 10;
	pthread_t gpioThread, wolThread;
	int rv = 0;

	readConfigurations();
	if (initGPIO() == -1)
	{
		fprintf (stderr, "Failed to initiate wiring pi library\n");
		exit (1);
	}
	if ((sockfd = startServer()) == -1)
	{
		fprintf (stderr, "Failed to create UDP server on PORT %s\n", PORT);
		exit (1);
	}
	signal (SIGINT, signalHandler);
	signal (SIGTERM, signalHandler);
	pthread_create (&gpioThread, NULL, gpioHandler, (void *) &interval);
	pthread_create (&wolThread, NULL, wolHandler, (void *) &sockfd);
	pthread_join (wolThread, NULL);
	pthread_join (gpioThread, NULL);
	if (shtdwn == 1) rv = system ("poweroff");
	pthread_exit ((void *) &rv);
}
