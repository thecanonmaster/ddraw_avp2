#include <windows.h>
#include <stdio.h>


enum LOG_EX_INFO
{
	LTRACE = 0,
	LDEBUG,
	LINFO,
	LWARN,
	LERROR,
	LSTUB,
	LNONE,
	LMAX,
};

void logf(void * thisptr, LOG_EX_INFO eInfo, char *msg, ...);
void loglevel_override(LOG_EX_INFO eInfo);
void loglevel_revert();

