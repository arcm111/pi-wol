#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <wiringPi.h>
#include "gpio.h"

#define PIN_INTERNET 7
#define PIN_BTN 2
#define PIN_BTN2 0
#define PIN_BOOT 3
#define PIN_WOL 21
#define PIN_STATE 22
#define PIN_ERROR 25

int state = 0;
int shtdwn = 0;
int disabled = 0;
int interval = 10; /* Milli seconds */
int prevBtnState = HIGH;
int prevBtn2State = HIGH;
long internetLastChecked = 0;

int isConnected()
{
	char *hostname = "google.com";
	return gethostbyname (hostname) != NULL;
}

void setup()
{
	pinMode (PIN_BOOT, OUTPUT);
	pinMode (PIN_INTERNET, OUTPUT);
	pinMode (PIN_WOL, OUTPUT);
	pinMode (PIN_STATE, INPUT);
	pullUpDnControl (PIN_STATE, PUD_UP);
	pinMode (PIN_BTN2, INPUT);
	pullUpDnControl (PIN_BTN2, PUD_UP);
	pinMode (PIN_BTN, INPUT);
	pullUpDnControl (PIN_BTN, PUD_UP);
	pinMode (PIN_ERROR, OUTPUT);

	/* Switch Green LED on to indicate successful boot up */
	digitalWrite (PIN_BOOT, HIGH);
	/* Switch Yellow LED on if internet connection available */
	digitalWrite (PIN_INTERNET, isConnected());
	digitalWrite (PIN_WOL, LOW);
	digitalWrite (PIN_ERROR, LOW);

	internetLastChecked = millis();
	state = 1;
}

void deactivate()
{
	digitalWrite (PIN_BOOT, LOW);
	digitalWrite (PIN_INTERNET, LOW);
	digitalWrite (PIN_WOL, LOW);
	digitalWrite (PIN_ERROR, LOW);
	disabled = 1;
}

void reactivate()
{
	digitalWrite (PIN_BOOT, HIGH);
	digitalWrite (PIN_INTERNET, isConnected());
	disabled = 0;
}

void reset()
{
	onError();
	digitalWrite (PIN_BOOT, LOW);
	digitalWrite (PIN_INTERNET, LOW);
	digitalWrite (PIN_WOL, LOW);
	digitalWrite (PIN_ERROR, LOW);
	pinMode (PIN_BOOT, INPUT);
	pinMode (PIN_INTERNET, INPUT);
	pinMode (PIN_WOL, INPUT);
	pinMode (PIN_ERROR, INPUT);
	pinMode (PIN_STATE, INPUT);
	pinMode (PIN_BTN, INPUT);
	pinMode (PIN_BTN2, INPUT);
	state = 0;
}

int isBoardAttached()
{
	return digitalRead (PIN_STATE) == LOW;
}

int isBoardReady()
{
	return state && !disabled;
}

void onCommand()
{
	int i;
	if (!isBoardReady()) return;
	for (i = 0; i < 6; i++)
	{
		digitalWrite (PIN_WOL, HIGH);
		delay (100);
		digitalWrite (PIN_WOL, LOW);
		delay (100);
	}
}

void onError()
{
	int i;
	if (!isBoardReady()) return;
	for (i = 0; i < 3; i++)
	{
		digitalWrite (PIN_ERROR, HIGH);
		delay (200);
		digitalWrite (PIN_ERROR, LOW);
		delay (200);
	}
}

void onFatalError()
{
	if (isBoardReady()) digitalWrite (PIN_ERROR, HIGH);
}

void *gpioHandler (void *arg)
{
	int t = *((int *) arg);
	int btnState, btn2State, connected;

	while (!quit)
	{
		if (!isBoardAttached())
		{
			printf ("Board is not connected ...\n");
			if (state == 1) reset();
			delay (10 * 1000);
			continue;
		}
		if (state == 0)
		{
			setup();
			printf ("Board is connected.\n");
			onCommand();
		}
		/* Disable/Enable board depending on btn state */
		if ((btnState = digitalRead (PIN_BTN)) == LOW && prevBtnState == HIGH)
		{
			disabled ? reactivate() : deactivate();
			printf ("Powering %s...\n", (disabled ? "off" : "on"));
		}
		prevBtnState = btnState;
		/* Poweroff system on btn2 pressed */
		btn2State = digitalRead (PIN_BTN2);
		if (btn2State == LOW && prevBtnState == HIGH)
		{
			shtdwn = 1;
			raise (SIGINT);
			break;
		}
		prevBtn2State = btn2State;
		/* If board is deactivated wait until reactivated */
		if (disabled)
		{
			delay (t);
			continue;
		}
		/* Check internet connection every 1 second */
		if (millis() - internetLastChecked > 5000)
		{
			connected = isConnected();
			digitalWrite (PIN_INTERNET, connected);
			internetLastChecked = millis();
		}
		delay (t);
	}
	reset();
	pthread_exit (NULL);
}

int initGPIO()
{
	return wiringPiSetup();
}
