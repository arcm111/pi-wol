#ifndef __SENDER_H_
#define __SENDER_H_

int initSocket();
int sendMagicPacket (unsigned char *packet, int len, const char *ip);

#endif
