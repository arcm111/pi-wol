#ifndef __WOL_UTILS_H_
#define __WOL_UTILS_H_

int isValidMagicPacket (unsigned char *packet, int len);
void getMacAddress (unsigned char *packet, int packetLen, unsigned char *mac);
void printBytes (unsigned char *bytes, int count, char *title);
void printMacAdress (unsigned char *mac, int len, char *title);

#endif
