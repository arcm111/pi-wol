extern int quit;
void *gpioHandler (void *arg);
void onCommand();
void onError();
void onFatalError();
int isBoardReady();
int initGPIO();
