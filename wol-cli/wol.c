#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include "wol.h"

int packMacAddr (const char *mac, mac_addr_t *packedMac)
{
	const char delimiter[2] = ":";
	char *tok;
	int total = 0;
	int count = 0;
	unsigned char val;
	char *tmpMac, *p = (char *) mac;

	if (mac != NULL) while (*p != '\0') if (*p++ == ':') total += 1;
	if (total != MAC_ADDR_MAX - 1) return -1;
	if ((tmpMac = malloc ((strlen (mac) + 1) * sizeof (char))) == NULL)
	{
		perror ("Cannot allocate memory for mac address: ");
		return -1;
	}
	strcpy (tmpMac, mac);
	tok = strtok (tmpMac, delimiter);
	while (tok != NULL)
	{
		val = (unsigned char) strtol (tok, NULL, CONVERT_BASE);
		packedMac->mac_addr[count++] = val;
		tok = strtok (NULL, delimiter);
	}
	free (tmpMac);
	strncpy (packedMac->mac_addr_str, mac, MAC_ADDR_STR_MAX);
	return 0;
}

int startupSocket()
{
	int sock;
	int optval = 1;
	char *opt = (char *) &optval;

	if ((sock = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		perror ("Cannot open socket: ");
		return -1;
	}
	if  (setsockopt (sock, SOL_SOCKET, SO_BROADCAST, opt, sizeof (optval)) < 0)
	{
		perror ("Cannot set socket options: ");
		return -1;
	}
	return sock;
}

int sendWOL (const wol_header_t *wol_header, const int sock)
{
	struct sockaddr_in addr;
	unsigned char packet[PACKET_BUF];
	int i, j;
	struct sockaddr *dest;

	addr.sin_family = AF_INET;
	addr.sin_port   = htons (REMOTE_PORT);
	if (inet_aton (wol_header->remote_addr, &addr.sin_addr) == 0)
	{
		fprintf (stderr, "Invalid remote ip address given: ");
		fprintf (stderr, "%s ...!\n", wol_header->remote_addr);
		return -1;
	}
	dest = (struct sockaddr *) &addr; 
	for (i = 0; i < 6; i++) packet[i] = 0xFF;
	for (i = 1; i <= 16; i++) for (j = 0; j < 6; j++)
	{
		packet[i * 6 + j] = wol_header->mac_addr->mac_addr[j];
	}
	if (sendto (sock, packet, sizeof (packet), 0, dest, sizeof (addr)) < 0)
	{
		perror ("Cannot send data: ");
		return -1;
	}
	return 0;
}

int main (int argc, char **argv)
{
	int sock, c, errflg = 1;
	char *mac = NULL;
	char *remote = NULL;
	wol_header_t *WOLHeader = NULL;

	/* Parse arguments */
	if (argc < 3)
	{
		fprintf (stderr, USAGE, *argv);
		exit (EXIT_FAILURE);
	}
	while ((c = getopt (argc, argv, ":r:")) != -1) switch (c)
	{
		case 'r':
			remote = optarg;
			errflg--;
		break;
		case '?':
			if (isprint (optopt))
			{
				fprintf (stderr, "Unrcognized option: -%c ...!\n", optopt);
			}
			errflg++;
		break;
		case ':':
			fprintf (stderr, "Option -%c requires an operand\n", optopt);
			errflg++;
		break;
		default:
			fprintf (stderr, USAGE, *argv);
			exit (EXIT_FAILURE);
		break;
	}
	if (optind == argc) errflg++;
	else mac = argv[optind];
	if (errflg)
	{
		if (mac == NULL) puts ("Missing MAC address");
		if (remote == NULL) puts ("Missing remote address");
		exit (EXIT_FAILURE);
	}
	printf ("Remote address: %s\n", remote);
	printf ("MAC address: %s\n", mac);

	/* Connect to broadcast ip; Log is done in startupSocket() */
	if ((sock = startupSocket()) < 0) exit (EXIT_FAILURE);

	if ((WOLHeader = malloc (sizeof (wol_header_t))) == NULL)
	{
		perror ("memory: ");
		exit (EXIT_FAILURE);
	}
	if ((WOLHeader->mac_addr = malloc (sizeof (mac_addr_t))) == NULL)
	{
		perror ("memory: ");
		free (WOLHeader);
		exit (EXIT_FAILURE);
	}
	strncpy (WOLHeader->remote_addr, remote, ADDR_LEN);
	if (packMacAddr (mac, WOLHeader->mac_addr) == 0)
	{
		if (sendWOL (WOLHeader, sock) == 0)
		{
			printf ("Successful sent WOL magic packet to %s\n", mac);
			free (WOLHeader->mac_addr);
			free (WOLHeader);
			close (sock);
			return EXIT_SUCCESS;
		}
		fprintf (stderr, "Error occured during sending the ");
		fprintf (stderr, "WOL magic packet for mac address: ");
		fprintf (stderr, "%s\n", mac);
	}
	else printf ("Failed to pack mac address: %s\n", mac);
	free (WOLHeader->mac_addr);
	free (WOLHeader);
	close (sock);
	return EXIT_FAILURE;
}
