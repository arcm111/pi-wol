#include <stdio.h>
#include "utils.h"

int isValidMagicPacket (unsigned char *packet, int len)
{
	int i, n;

	if (len < 102) return 0;
	for (i = 0; i < 6; i++) if (packet[i] != 0xff) return 0;
	for (i = 6; i < 12; i ++)
	{
		for (n = 0; n < 16; n++) if (packet[i + 6 * n] != packet[i]) return 0;
	}
	return 1;
}

void getMacAddress (unsigned char *packet, int packetLength, unsigned char *mac)
{
	int i, n;
	if (isValidMagicPacket (packet, packetLength))
	{
		for (i = 6, n = 0; i < 12; i++, n++) mac[n] = packet[i];
	}
	else printf ("Invalid magic packet\n");
}

void printBytes (unsigned char *bytes, int count, char *title)
{
	int i;
	printf ("%s:\n", title);
	for (i = 0; i < count; i++) printf ("%02X ", bytes[i]);
	printf ("\n");
}

void printMacAdress (unsigned char *mac, int len, char *title)
{
	int i;
	printf ("%s:\n", title);
	for (i = 0; i < len - 1; i++) printf ("%02X:", mac[i]);
	printf ("%02X\n", mac[len - 1]);
}
