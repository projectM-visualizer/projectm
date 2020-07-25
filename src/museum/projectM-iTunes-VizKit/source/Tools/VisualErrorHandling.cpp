/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualErrorHandling.cpp
 *
 */

/***************************************************************************
 
 Copyright (c) 2004-2009 Heiko Wichmann (http://www.imagomat.de/vizkit)
 
 
 This software is provided 'as-is', without any expressed or implied warranty. 
 In no event will the authors be held liable for any damages
 arising from the use of this software.
 
 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it
 freely, subject to the following restrictions:
 
 1. The origin of this software must not be misrepresented; 
 you must not claim that you wrote the original software. 
 If you use this software in a product, an acknowledgment 
 in the product documentation would be appreciated 
 but is not required.
 
 2. Altered source versions must be plainly marked as such, 
 and must not be misrepresented as being the original software.
 
 3. This notice may not be removed or altered from any source distribution.
 
 ***************************************************************************/

#include "VisualErrorHandling.h"
#include "VisualConfiguration.h"
#include "VisualFile.h"
#include "VisualString.h"
#include "VisualPlayerState.h" 

#include <stdio.h>

#if TARGET_OS_MAC
#include <dlfcn.h>
#include <cxxabi.h>
#endif


#if TARGET_OS_WIN
#include <windows.h>
#endif


#if TARGET_OS_WIN
#define PERR(bSuccess, api) {if(!(bSuccess)) printf("%s: Error %d from %s on line %d\n", __FILE__, GetLastError(), api, __LINE__);}
#endif


// private declaration of internally used function
#if TARGET_OS_MAC
void printframeinfo(unsigned int level, void *fp, void *ra);
#endif


void writeLog(const char* const infoStr) {

	bool textFile = false;
#if DEBUG
	textFile = true;
#endif
	
	if (textFile) {
		VizKit::VisualFile logFile;
		bool success = logFile.initWithUserDesktopDirectory();
		if (!success) {
			return;
		}
		VizKit::VisualString logFileName = VizKit::VisualString(VizKit::VisualConfiguration::visualizerPluginIdentifierName);
		logFileName = logFileName + ".log.txt";
		success = logFile.appendFileName(logFileName);
		if (!success) {
			return;
		}
		VizKit::VisualString logFilePathString;
		logFile.getFilePath(logFilePathString);
		const char* const logFilePath = logFilePathString.getUtf8Representation();
		
		FILE* aFile = fopen(logFilePath, "a");
		if (aFile != NULL) {
			fprintf(aFile, "%s\n", infoStr);
			fclose(aFile);
		}
		
	} else {
#if TARGET_OS_MAC
		printf("%s\n", infoStr);
#endif
#if TARGET_OS_WIN
		HANDLE hSource;
		char* szList[1];
		szList[0] = const_cast<char*>(infoStr);
		hSource = RegisterEventSource(NULL, VizKit::VisualConfiguration::visualizerPluginIdentifierName);
		if (hSource != NULL)  {
			ReportEvent(hSource, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, (LPCSTR*)szList, NULL);
			DeregisterEventSource(hSource);
		}
#endif
	}
}


void writeLogW(const wchar_t* const infoStr) {
#if TARGET_OS_MAC
	printf("%ls\n", infoStr);
#endif
#if TARGET_OS_WIN
	HANDLE hSource;
	wchar_t* szList[1];
	szList[0] = const_cast<wchar_t*>(infoStr);
	hSource = RegisterEventSource(NULL, VizKit::VisualConfiguration::visualizerPluginIdentifierName);
	if (hSource != NULL)  {
		ReportEventW(hSource, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, (LPCWSTR*)szList, NULL);
		DeregisterEventSource(hSource);
	}
#endif
}


void writeDebugLog(const char* const infoStr) {
	VizKit::VisualPlayerState* state = VizKit::VisualPlayerState::getInstance();
	if (state->isInDebugMode() == true) {
#if TARGET_OS_MAC
		printf("%s\n", infoStr);
#endif
#if TARGET_OS_WIN
		HANDLE hSource;
		char* szList[1];
		szList[0] = const_cast<char*>(infoStr);
		hSource = RegisterEventSource(NULL, VizKit::VisualConfiguration::visualizerPluginIdentifierName);
		if (hSource != NULL)  {
			ReportEvent(hSource, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, (LPCSTR*)szList, NULL);
			DeregisterEventSource(hSource);
		}
#endif
	}
}



#if TARGET_OS_WIN

void writeLastErrorLog(char* lpszFunction) {
	TCHAR szBuf[1024];
	LPVOID lpMsgBuf;
	DWORD dw = GetLastError();
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				  NULL,
				  dw,
				  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				  (LPSTR) &lpMsgBuf,
				  0, 
				  NULL);
	
	wsprintf(szBuf, "%s failed with error %d: %s", lpszFunction, dw, lpMsgBuf);
	
	writeLog(szBuf);
	LocalFree(lpMsgBuf);
}

#endif



#if TARGET_OS_MAC

void printframeinfo(unsigned int level, void *fp, void *ra) {
    int     ret;
    Dl_info info;
	
    // Find the image containing the given address
    ret = dladdr(ra, &info);
	
	//char demangledName[1024];
	//size_t length;
	//int status;
	//char* demangledNameRetChar = abi::__cxa_demangle(info.dli_sname, demangledName, &length, &status);
	//printf("demangled: %s\n", demangledName);
    
	printf("#%u %s%s in %s, fp = %p, pc = %p\n",
           level,
           (ret) ? info.dli_sname : "?",          // symbol name
           (ret) ? "()" : "",                     // show as a function
           (ret) ? info.dli_fname : "?", fp, ra); // shared object name
}


void stacktrace() {
    unsigned int level = 0;
    void    *saved_ra  = __builtin_return_address(0);
    void   **fp        = (void **)__builtin_frame_address(0);
    void    *saved_fp  = __builtin_frame_address(1);
	
    printframeinfo(level, saved_fp, saved_ra);
    level++;
    fp = (void**)saved_fp;
    while (fp) {
        saved_fp = *fp;
        fp = (void**)saved_fp;
        if (*fp == NULL)
            break;
        saved_ra = *(fp + 2);
        printframeinfo(level, saved_fp, saved_ra);
        level++;
    }
}

#endif




#if TARGET_OS_WIN

/*********************************************************************
 * FUNCTION: addSourceToRegistry(void)                                *
 *                                                                    *
 * PURPOSE: Add a source name key, message DLL name value, and        *
 *          message type supported value to the registry              *
 *                                                                    *
 * INPUT: source name, path of message DLL                            *
 *                                                                    *
 * RETURNS: none                                                      *
 *********************************************************************/

void addSourceToRegistry(char* pszAppname, char* pszMsgDLL)
{
	HKEY hk;                      /* registry key handle */
	DWORD dwData;
	BOOL bSuccess;
	
	/* When an application uses the RegisterEventSource or OpenEventLog
     function to get a handle of an event log, the event loggging service
     searches for the specified source name in the registry. You can add a
     new source name to the registry by opening a new registry subkey
     under the Application key and adding registry values to the new
     subkey. */
	
	/* Create a new key for our application */
	bSuccess = RegCreateKey(HKEY_LOCAL_MACHINE,
							"SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\NTP", &hk);
	PERR(bSuccess == ERROR_SUCCESS, "RegCreateKey");
	
	/* Add the Event-ID message-file name to the subkey. */
	bSuccess = RegSetValueEx(hk,  /* subkey handle         */
							 "EventMessageFile",       /* value name            */
							 0,                        /* must be zero          */
							 REG_EXPAND_SZ,            /* value type            */
							 (LPBYTE) pszMsgDLL,       /* address of value data */
							 strlen(pszMsgDLL) + 1);   /* length of value data  */
	PERR(bSuccess == ERROR_SUCCESS, "RegSetValueEx");
	
	/* Set the supported types flags and addit to the subkey. */
	dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;
	bSuccess = RegSetValueEx(hk,  /* subkey handle                */
							 "TypesSupported",         /* value name                   */
							 0,                        /* must be zero                 */
							 REG_DWORD,                /* value type                   */
							 (LPBYTE) &dwData,         /* address of value data        */
							 sizeof(DWORD));           /* length of value data         */
	PERR(bSuccess == ERROR_SUCCESS, "RegSetValueEx");
	RegCloseKey(hk);
	return;
}

/*********************************************************************
 * FUNCTION: reportAnEvent(DWORD dwIdEvent, WORD cStrings,            *
 *                         LPTSTR *ppszStrings);                      *
 *                                                                    *
 * PURPOSE: add the event to the event log                            *
 *                                                                    *
 * INPUT: the event ID to report in the log, the number of insert     *
 *        strings, and an array of null-terminated insert strings     *
 *                                                                    *
 * RETURNS: none                                                      *
 *********************************************************************/

void reportAnIEvent(uint32 dwIdEvent, uint16 cStrings, char* pszStrings)
{
	HANDLE hAppLog;
	BOOL bSuccess;
	
	/* Get a handle to the Application event log */
	hAppLog = RegisterEventSource(NULL,   /* use local machine      */
								  "NTP");                   /* source name                 */
	PERR(hAppLog, "RegisterEventSource");
	
	/* Now report the event, which will add this event to the event log */
	bSuccess = ReportEvent(hAppLog,       /* event-log handle            */
						   EVENTLOG_INFORMATION_TYPE,      /* event type                  */
						   0,                        /* category zero               */
						   dwIdEvent,                /* event ID                    */
						   NULL,                     /* no user SID                 */
						   cStrings,                 /* number of substitution strings     */
						   0,                        /* no binary data              */
						   (LPCSTR*)pszStrings,               /* string array                */
						   NULL);                    /* address of data             */
	PERR(bSuccess, "ReportEvent");
	DeregisterEventSource(hAppLog);
	return;
}

void reportAnWEvent(uint32 dwIdEvent, uint16 cStrings, char* pszStrings)
{
	HANDLE hAppLog;
	BOOL bSuccess;
	
	/* Get a handle to the Application event log */
	hAppLog = RegisterEventSource(NULL,   /* use local machine      */
								  "NTP");                   /* source name                 */
	PERR(hAppLog, "RegisterEventSource");
	
	/* Now report the event, which will add this event to the event log */
	bSuccess = ReportEvent(hAppLog,       /* event-log handle            */
						   EVENTLOG_WARNING_TYPE,      /* event type                  */
						   0,                        /* category zero               */
						   dwIdEvent,                /* event ID                    */
						   NULL,                     /* no user SID                 */
						   cStrings,                 /* number of substitution strings     */
						   0,                        /* no binary data              */
						   (LPCSTR*)pszStrings,               /* string array                */
						   NULL);                    /* address of data             */
	PERR(bSuccess, "ReportEvent");
	DeregisterEventSource(hAppLog);
	return;
}

void reportAnEEvent(uint32 dwIdEvent, uint16 cStrings, char* pszStrings)
{
	HANDLE hAppLog;
	BOOL bSuccess;
	
	/* Get a handle to the Application event log */
	hAppLog = RegisterEventSource(NULL,   /* use local machine      */
								  "NTP");                   /* source name                 */
	PERR(hAppLog, "RegisterEventSource");
	
	/* Now report the event, which will add this event to the event log */
	bSuccess = ReportEvent(hAppLog,       /* event-log handle            */
						   EVENTLOG_ERROR_TYPE,      /* event type                  */
						   0,                        /* category zero               */
						   dwIdEvent,                /* event ID                    */
						   NULL,                     /* no user SID                 */
						   cStrings,                 /* number of substitution strings     */
						   0,                        /* no binary data              */
						   (LPCSTR*)pszStrings,               /* string array                */
						   NULL);                    /* address of data             */
	PERR(bSuccess, "ReportEvent");
	DeregisterEventSource(hAppLog);
	return;
}


#endif



