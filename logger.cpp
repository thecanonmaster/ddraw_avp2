#include "logger.h"

FILE* g_LogFile = NULL;
char g_szTemp[2048];
static LOG_EX_INFO s_eLogLevel = LTRACE;
static LOG_EX_INFO s_eLogLevelTemp = LTRACE;

/*void logf_old(void * thisptr, char *msg, ...)
{	
	va_list argp;
	
	static int t = -1;
	if (t == -1)
		t = GetTickCount();
	int tn = GetTickCount();
	
	fprintf(g_LogFile,"[%+6dms] (%08x) ", tn-t, thisptr);
	t = tn;
	
	va_start(argp, msg);
	vfprintf(g_LogFile, msg, argp);
	va_end(argp);
	
	fprintf(g_LogFile,"\n");
}*/

void loglevel_override(LOG_EX_INFO eInfo)
{
	s_eLogLevelTemp = s_eLogLevel;
	s_eLogLevel = eInfo;
}

void loglevel_revert()
{
	s_eLogLevel = s_eLogLevelTemp;
}

void logf(void * thisptr, LOG_EX_INFO eInfo, char *msg, ...)
{	
	va_list argp;
		
	switch (eInfo)
	{
		case LTRACE: 
			if (s_eLogLevel > LTRACE) return;
			fprintf(g_LogFile,"(%08X) [TRACE]   ", thisptr);
		break;

		case LDEBUG: 
			if (s_eLogLevel > LDEBUG) return;
			fprintf(g_LogFile,"(%08X) [DEBUG]   ", thisptr);
		break;

		case LERROR: 
			if (s_eLogLevel > LERROR) return;
			fprintf(g_LogFile,"(%08X) [ERROR]   ", thisptr);
		break;

		case LWARN: 
			if (s_eLogLevel > LWARN) return;
			fprintf(g_LogFile,"(%08X) [WARN ]   ", thisptr);
		break;

		case LSTUB:  
			if (s_eLogLevel > LSTUB) return;
			fprintf(g_LogFile,"(%08X) [STUB ]   ", thisptr);
		break;

		case LINFO:	
			if (s_eLogLevel > LINFO) return;
			fprintf(g_LogFile,"(%08X) [INFO ]   ", thisptr);
		break;
	}

	
	va_start(argp, msg);
	vfprintf(g_LogFile, msg, argp);
	va_end(argp);
	
	fprintf(g_LogFile,"\n");

	//fflush(g_LogFile);

}
