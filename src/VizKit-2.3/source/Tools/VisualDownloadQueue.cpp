/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualDownloadQueue.cpp
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

#include "VisualDownloadQueue.h"
#include "VisualObject.h"
#include "VisualObjectData.h"
#include "VisualErrorHandling.h"
#include "VisualItemIdentifier.h"
#include "VisualNotification.h"
#include "VisualThreading.h"
#include "VisualThreadingManager.h"
#include "VisualTiming.h"
#include "VisualString.h"



using namespace VizKit;



VisualDownloadQueue* VisualDownloadQueue::theVisualDownloadQueue = NULL;


VisualDownloadQueue::VisualDownloadQueue() {
	downloadIsProcessing = false;
	stopDownloadProcess = false;
	downloadAddRequestQueueIsEmpty = true;
}


VisualDownloadQueue::~VisualDownloadQueue() {
	downloadAddRequestQueue.clear();
	downloadStopRequestQueue.clear();
#if TARGET_OS_MAC
	MPCriticalRegionID visualDownloadQueueAccessCriticalRegion = VisualThreadingManager::getCriticalRegionID(VisualThreadingManager::downloadQueueAccessMutex);
	OSStatus status = noErr;
	status = ::MPEnterCriticalRegion(visualDownloadQueueAccessCriticalRegion, kDurationForever);
	if (status == noErr) {
#endif
#if TARGET_OS_WIN
	LPCRITICAL_SECTION visualDownloadQueueAccessCriticalSection = VisualThreadingManager::getCriticalSectionPointer(VisualThreadingManager::downloadQueueAccessMutex);
	::EnterCriticalSection(visualDownloadQueueAccessCriticalSection);
#endif
		downloadQueue.clear();
#if TARGET_OS_MAC
		status = MPExitCriticalRegion(visualDownloadQueueAccessCriticalRegion);
	}
#endif
#if TARGET_OS_WIN
	::LeaveCriticalSection(visualDownloadQueueAccessCriticalSection);
#endif
}


VisualDownloadQueue* VisualDownloadQueue::getInstance() {
	if (theVisualDownloadQueue == NULL) {
#if TARGET_OS_MAC
		MPCriticalRegionID visualDownloadQueueCriticalRegion = VisualThreadingManager::getCriticalRegionID(VisualThreadingManager::downloadQueueSingletonMutex);
		OSStatus status = noErr;
		status = ::MPEnterCriticalRegion(visualDownloadQueueCriticalRegion, kDurationForever);
		if (status != noErr) return NULL;
#endif
#if TARGET_OS_WIN
		LPCRITICAL_SECTION visualDownloadQueueCriticalSection = VisualThreadingManager::getCriticalSectionPointer(VisualThreadingManager::downloadQueueSingletonMutex);
		::EnterCriticalSection(visualDownloadQueueCriticalSection);
#endif
		if (theVisualDownloadQueue == NULL) {
			theVisualDownloadQueue = new VisualDownloadQueue;
			if (theVisualDownloadQueue == NULL) {
				char errLog[256];
				sprintf(errLog, "ERR: init VisualDownloadQueue failed in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
				writeLog(errLog);
			}
		}
#if TARGET_OS_MAC
		status = ::MPExitCriticalRegion(visualDownloadQueueCriticalRegion);
#endif
#if TARGET_OS_WIN
		::LeaveCriticalSection(visualDownloadQueueCriticalSection);
#endif
	}
	return theVisualDownloadQueue;
}


void VisualDownloadQueue::dispose() {
	if (theVisualDownloadQueue != NULL) {
		theVisualDownloadQueue->stopDownloadProcess = true;
		VisualItemIdentifier downloadQueueStopId;
		uint32 elapsedMilliseconds = 0;
		while ((theVisualDownloadQueue->downloadIsProcessing) && elapsedMilliseconds < 3000) {
			elapsedMilliseconds = VisualTiming::getElapsedMilliSecsSinceReset(downloadQueueStopId);
			VisualTiming::update(); // manual update of current timestamp
			// wait
		}
		delete theVisualDownloadQueue;
		theVisualDownloadQueue = NULL;
	}
}


bool VisualDownloadQueue::addToDownloadQueue(const VisualString& aURL, VisualObject* theRequestingObject, const VisualItemIdentifier& identifier) {

	bool debug = false;

	if (debug) {
		writeLog("Enter: VisualDownloadQueue::addToDownloadQueue");
	}

#if TARGET_OS_MAC

	CFURLRef aURLRef = CFURLCreateWithString(kCFAllocatorDefault, aURL.getCharactersPointer(), NULL);

	CFHTTPMessageRef requestMessageRef = CFHTTPMessageCreateRequest(kCFAllocatorDefault, CFSTR("GET"), aURLRef, kCFHTTPVersion1_1);
	if (requestMessageRef == NULL) {
		CFRelease(aURLRef);
		if (debug) {
			writeLog("CFHTTPMessageCreateRequest failed");
		}
		return false;
	}

	CFReadStreamRef readStreamRef = CFReadStreamCreateForHTTPRequest(kCFAllocatorDefault, requestMessageRef);
	if (readStreamRef == NULL) {
		CFRelease(aURLRef);
		CFRelease(requestMessageRef);
		if (debug) {
			writeLog("CFReadStreamCreateForHTTPRequest failed");
		}
		return false;
	}
    
	Boolean success = CFReadStreamOpen(readStreamRef);
	if (!success) {
		CFRelease(aURLRef);
		CFRelease(requestMessageRef);
		CFRelease(readStreamRef);
		if (debug) {
			writeLog("CFReadStreamOpen failed");
		}
		return false;
	}
	
	CFRelease(aURLRef);
	CFRelease(requestMessageRef);

#endif

#if TARGET_OS_WIN

    HINTERNET hRequest = NULL;

	LPCTSTR lpszAgent = "VizKit Remote Access/1.0";
	DWORD dwAccessType = INTERNET_OPEN_TYPE_PRECONFIG;
	LPCTSTR lpszProxyName = NULL;
	LPCTSTR lpszProxyBypass = NULL;
	DWORD dwFlags = 0;
	HINTERNET hSession = InternetOpen(lpszAgent, dwAccessType, lpszProxyName, lpszProxyBypass, dwFlags);

    if (hSession) {
		wchar_t* lpszHeaders = NULL;
		DWORD dwHeadersLength = 0;
		DWORD dwFlags = INTERNET_FLAG_NO_CACHE_WRITE;
		DWORD_PTR dwContext = NULL;
        hRequest = InternetOpenUrlW(hSession, aURL.getCharactersPointer(), lpszHeaders, dwHeadersLength, dwFlags, dwContext);
	} else {
		if (debug) {
			char logStr[256];
			sprintf(logStr, "Err: InternetOpen failed (%d)", GetLastError());
			writeLog(logStr);
		}
	}

	if (!hRequest) {
		if (debug) {
			char logStr[256];
			sprintf(logStr, "Err: InternetOpenUrl failed (%d)", GetLastError());
			writeLog(logStr);
		}
	}

#endif

	DownloadItem downloadItem;
	VisualItemIdentifier timingIdentifier;
#if TARGET_OS_MAC
	downloadItem.readStreamRef = readStreamRef;
#endif
#if TARGET_OS_WIN
	downloadItem.session = hSession;
	downloadItem.readRequest = hRequest;
#endif
	downloadItem.requestingObject = theRequestingObject;
	downloadItem.requestingObjectData = theRequestingObject->getVisualObjectData();
	downloadItem.requestingObjectData->setIsDownloading(true);
	downloadItem.bytesRead = 0;
	downloadItem.timingId = timingIdentifier;
	downloadItem.requestId = identifier;
	downloadItem.streamDidClose = false;
	downloadItem.isActive = true;
	
	theVisualDownloadQueue = VisualDownloadQueue::getInstance();
#if TARGET_OS_MAC
	MPCriticalRegionID addToDownloadQueueCriticalRegion = VisualThreadingManager::getCriticalRegionID(VisualThreadingManager::addRequestDownloadQueueAccess);
	OSStatus status = noErr;
	status = MPEnterCriticalRegion(addToDownloadQueueCriticalRegion, kDurationForever);
	if (status != noErr) return NULL;
#endif
#if TARGET_OS_WIN
	LPCRITICAL_SECTION addToDownloadQueueCriticalSection = VisualThreadingManager::getCriticalSectionPointer(VisualThreadingManager::addRequestDownloadQueueAccess);
	::EnterCriticalSection(addToDownloadQueueCriticalSection);
#endif
	theVisualDownloadQueue->downloadAddRequestQueue.push_back(downloadItem);
	theVisualDownloadQueue->downloadAddRequestQueueIsEmpty = false;
#if TARGET_OS_MAC
	status = MPExitCriticalRegion(addToDownloadQueueCriticalRegion);
#endif
#if TARGET_OS_WIN
	::LeaveCriticalSection(addToDownloadQueueCriticalSection);
#endif
	
	return true;
}


bool VisualDownloadQueue::removeDownload(const VisualItemIdentifier& identifier) {
	bool found = false;
	theVisualDownloadQueue = VisualDownloadQueue::getInstance();
#if TARGET_OS_MAC
	MPCriticalRegionID visualDownloadQueueAccessCriticalRegion = VisualThreadingManager::getCriticalRegionID(VisualThreadingManager::downloadQueueAccessMutex);
	OSStatus status = noErr;
	status = ::MPEnterCriticalRegion(visualDownloadQueueAccessCriticalRegion, kDurationForever);
	if (status != noErr) return NULL;
#endif
#if TARGET_OS_WIN
	LPCRITICAL_SECTION visualDownloadQueueAccessCriticalSection = VisualThreadingManager::getCriticalSectionPointer(VisualThreadingManager::downloadQueueAccessMutex);
	::EnterCriticalSection(visualDownloadQueueAccessCriticalSection);
#endif
	DownloadQueueIterator it = theVisualDownloadQueue->downloadQueue.begin();
	while ((it != theVisualDownloadQueue->downloadQueue.end()) && (found == false)) {
		if (it->requestId == identifier) {
#if TARGET_OS_MAC
			MPCriticalRegionID removeFromDownloadQueueCriticalRegion = VisualThreadingManager::getCriticalRegionID(VisualThreadingManager::stopRequestDownloadQueueAccess);
			status = MPEnterCriticalRegion(removeFromDownloadQueueCriticalRegion, kDurationForever);
			if (status != noErr) return NULL;
#endif
#if TARGET_OS_WIN
			LPCRITICAL_SECTION removeFromDownloadQueueCriticalSection = VisualThreadingManager::getCriticalSectionPointer(VisualThreadingManager::stopRequestDownloadQueueAccess);
			::EnterCriticalSection(removeFromDownloadQueueCriticalSection);
#endif
			theVisualDownloadQueue->downloadStopRequestQueue.push_back(it->identifier);
#if TARGET_OS_MAC
			status = MPExitCriticalRegion(removeFromDownloadQueueCriticalRegion);
#endif
#if TARGET_OS_WIN
			::LeaveCriticalSection(removeFromDownloadQueueCriticalSection);
#endif
			found = true;
		} else {
			it++;
		}
	}
#if TARGET_OS_MAC
	status = MPExitCriticalRegion(visualDownloadQueueAccessCriticalRegion);
#endif
#if TARGET_OS_WIN
	::LeaveCriticalSection(visualDownloadQueueAccessCriticalSection);
#endif
	return found;
}


bool VisualDownloadQueue::removeDownloadForObject(const VisualItemIdentifier& visualObjectIdentifier) {
	bool found = false;
	theVisualDownloadQueue = VisualDownloadQueue::getInstance();
	
#if TARGET_OS_MAC
	MPCriticalRegionID visualDownloadQueueAccessCriticalRegion = VisualThreadingManager::getCriticalRegionID(VisualThreadingManager::downloadQueueAccessMutex);
	OSStatus status = noErr;
	status = ::MPEnterCriticalRegion(visualDownloadQueueAccessCriticalRegion, kDurationForever);
	if (status != noErr) return NULL;
#endif
#if TARGET_OS_WIN
	LPCRITICAL_SECTION visualDownloadQueueAccessCriticalSection = VisualThreadingManager::getCriticalSectionPointer(VisualThreadingManager::downloadQueueAccessMutex);
	::EnterCriticalSection(visualDownloadQueueAccessCriticalSection);
#endif
	
	DownloadQueueIterator it = theVisualDownloadQueue->downloadQueue.begin();
	while ((it != theVisualDownloadQueue->downloadQueue.end()) && (found == false)) {
		if (it->requestingObjectData->getIdentifier() == visualObjectIdentifier) {
#if TARGET_OS_MAC
			MPCriticalRegionID removeFromDownloadQueueCriticalRegion = VisualThreadingManager::getCriticalRegionID(VisualThreadingManager::stopRequestDownloadQueueAccess);
			status = MPEnterCriticalRegion(removeFromDownloadQueueCriticalRegion, kDurationForever);
			if (status != noErr) return NULL;
#endif
#if TARGET_OS_WIN
			LPCRITICAL_SECTION removeFromDownloadQueueCriticalSection = VisualThreadingManager::getCriticalSectionPointer(VisualThreadingManager::stopRequestDownloadQueueAccess);
			::EnterCriticalSection(removeFromDownloadQueueCriticalSection);
#endif
			theVisualDownloadQueue->downloadStopRequestQueue.push_back(it->identifier);
#if TARGET_OS_MAC
			status = MPExitCriticalRegion(removeFromDownloadQueueCriticalRegion);
#endif
#if TARGET_OS_WIN
			::LeaveCriticalSection(removeFromDownloadQueueCriticalSection);
#endif
			found = true;
		} else {
			it++;
		}
	}
	
#if TARGET_OS_MAC
	status = MPExitCriticalRegion(visualDownloadQueueAccessCriticalRegion);
#endif
#if TARGET_OS_WIN
	::LeaveCriticalSection(visualDownloadQueueAccessCriticalSection);
#endif
	
	return found;
}


void VisualDownloadQueue::setDownloadIsProcessingToFalse() {
	theVisualDownloadQueue = VisualDownloadQueue::getInstance();
	theVisualDownloadQueue->downloadIsProcessing = false;
}


bool VisualDownloadQueue::startDownloadThreadIfNeeded() {
	bool downloadStarted = false;
	static bool isInProgress = false;
	theVisualDownloadQueue = VisualDownloadQueue::getInstance();
	if (isInProgress == true) return false;
	isInProgress = true;
	if (theVisualDownloadQueue->shouldStartDownload()) {
		theVisualDownloadQueue->downloadIsProcessing = true;
		downloadStarted = VisualThreading::createThread((ThreadingFuncPtr)VisualDownloadQueue::processDownloadQueue, NULL, VisualDownloadQueue::setDownloadIsProcessingToFalse);
	}
	isInProgress = false;
	return downloadStarted;
}


#if TARGET_OS_MAC
OSStatus VisualDownloadQueue::processDownloadQueue(void* parameter) {
	OSStatus retVal = noErr;
#endif
#if TARGET_OS_WIN
DWORD WINAPI VisualDownloadQueue::processDownloadQueue(LPVOID lpParam) {
	DWORD retVal = 0;
#endif
	theVisualDownloadQueue = VisualDownloadQueue::getInstance();
	
	bool done = false;
	bool debug = false;

	while (!done) {

		if (debug) {
			//writeLog("processDownloadQueue1");
		}
#if TARGET_OS_MAC
		MPCriticalRegionID visualDownloadQueueAccessCriticalRegion = VisualThreadingManager::getCriticalRegionID(VisualThreadingManager::downloadQueueAccessMutex);
		OSStatus status = noErr;
		status = ::MPEnterCriticalRegion(visualDownloadQueueAccessCriticalRegion, kDurationForever);
		if (status != noErr) return NULL;
#endif
#if TARGET_OS_WIN
		LPCRITICAL_SECTION visualDownloadQueueAccessCriticalSection = VisualThreadingManager::getCriticalSectionPointer(VisualThreadingManager::downloadQueueAccessMutex);
		::EnterCriticalSection(visualDownloadQueueAccessCriticalSection);
#endif
		DownloadQueueIterator it = theVisualDownloadQueue->downloadQueue.begin();
		while (it != theVisualDownloadQueue->downloadQueue.end()) {
			if (debug) {
				//writeLog("processDownloadQueue2");
			}
			if (it->isActive) {
				if (theVisualDownloadQueue->stopDownloadProcess) {
					if (debug) {
						writeLog("theVisualDownloadQueue->stopDownloadProcess");
					}
					it->isActive = false;
				}
				if (!theVisualDownloadQueue->stopDownloadProcess) {
					uint32 elapsedMillisecondsSinceLastSuccessfulCall = VisualTiming::getElapsedMilliSecsSinceReset(it->timingId);
					if (elapsedMillisecondsSinceLastSuccessfulCall > 4000) {
						it->isActive = false; // timed out after 4 sec. of inactivity
					}
				}
#if TARGET_OS_MAC
				if (!it->readStreamRef) {
					if (debug) {
						writeLog("NO readStreamRef");
					}
					it->isActive = false;
				}
#endif
#if TARGET_OS_WIN
				if (!it->readRequest) {
					if (debug) {
						writeLog("NO readRequest");
					}
					it->isActive = false;
				}
#endif
			}
			
			if (it->isActive) {
			
				uint32 bufferSize = 32 * 1024;
				bool endOfStream = false;
				bool success = false;
				if (debug) {
					//writeLog("processDownloadQueue3");
				}

				uint32 elapsedMilliseconds = 0;
				if (!theVisualDownloadQueue->stopDownloadProcess) {
					elapsedMilliseconds = VisualTiming::getElapsedMilliSecsSinceReset(it->identifier);
				}
				if (elapsedMilliseconds > 100) {
					VisualTiming::resetTimestamp(it->identifier);
#if TARGET_OS_MAC
					CFStreamStatus streamStatus = CFReadStreamGetStatus(it->readStreamRef);
					if ((streamStatus != kCFStreamStatusNotOpen) && (streamStatus != kCFStreamStatusOpening) && (streamStatus != kCFStreamStatusClosed) && (streamStatus != kCFStreamStatusError)) {
						Boolean booleanSuccess = CFReadStreamHasBytesAvailable(it->readStreamRef);
						if (booleanSuccess) success = true;
					}
#endif
#if TARGET_OS_WIN
					DWORD dwSize = 0;
					DWORD dwFlags = 0;
					DWORD_PTR dwContext = NULL;
					BOOL boolSuccess = InternetQueryDataAvailable(it->readRequest, &dwSize, dwFlags, dwContext);
					bufferSize = dwSize;
					if (boolSuccess) success = true;
#endif
				}
				if (success) {
					if (debug == true) {
						writeLog("stream has bytes available");
					}
#if TARGET_OS_MAC

					UInt8* buffer = (UInt8*)malloc(bufferSize * sizeof(UInt8));
					CFIndex readChunkSize = CFReadStreamRead(it->readStreamRef, buffer, bufferSize);
					if (readChunkSize > 0) {

						// claim access of object
						bool objectIsAccessible = false;
						bool needToExitCriticalRegion = false;
						if (it->requestingObjectData->hasObject()) {
							it->requestingObjectData->enterCriticalRegion();
							needToExitCriticalRegion = true;
							if (it->requestingObjectData->hasObject()) {
								objectIsAccessible = true;
							}
						}
						if (!objectIsAccessible) {
							it->isActive = false;
						}
						
						if (it->isActive) {
							it->requestingObjectData->addData(buffer, readChunkSize);
						}
						if (needToExitCriticalRegion) {
							it->requestingObjectData->exitCriticalRegion();
						}
						
						it->bytesRead += readChunkSize;
						VisualTiming::resetTimestamp(it->timingId);
					} else if (readChunkSize == 0) {
						endOfStream = true;
					} else if (readChunkSize == -1) {
						// if either the stream is not open or an error occurs
					}
					free(buffer);
					if (debug == true) {
						char logStr[256];
						sprintf(logStr, "bytes read: %ld (%ld)\n", readChunkSize, it->bytesRead);
						writeLog(logStr);
					}

					/*
					if (!(it->requestingObjectData->dataSizeDeclaredHasBeenSet())) {
						// check for content-length value in header data
						CFHTTPMessageRef response = (CFHTTPMessageRef)CFReadStreamCopyProperty(it->readStreamRef, kCFStreamPropertyHTTPResponseHeader);
						if (response) {
							//CFIndex statusCode = CFHTTPMessageGetResponseStatusCode(response);
							success = CFHTTPMessageIsHeaderComplete(response);
							if (success) {
								CFStringRef theContentSizeString = CFHTTPMessageCopyHeaderFieldValue(response, CFSTR("Content-Length"));
								if (theContentSizeString != NULL) {
									VisualString aString = VisualString(theContentSizeString);
									if (!aString.isEmpty()) {
										long declaredSize = atol(aString.getUtf8Representation());
										it->requestingObjectData->setDataSizeDeclared(declaredSize);
									}
									CFRelease(theContentSizeString);
								}
							}
						}
					}
					 */

#endif

#if TARGET_OS_WIN
					uint8* buffer = (uint8*)malloc(bufferSize * sizeof(uint8));
					DWORD bytesRead = 0;
					BOOL readBool = InternetReadFile(it->readRequest, buffer, bufferSize, &bytesRead);
					if (bytesRead > 0) {
						
						// claim access of object
						bool objectIsAccessible = false;
						bool enteredCriticalRegion = false;
						if (it->requestingObjectData->hasObject()) {
							it->requestingObjectData->enterCriticalRegion();
							enteredCriticalRegion = true;
							if (it->requestingObjectData->hasObject()) {
								objectIsAccessible = true;
							}
						}
						if (!objectIsAccessible) {
							it->isActive = false;
						}
						
						if (it->isActive) {
							it->requestingObjectData->addData(buffer, bytesRead);
						}
						if (enteredCriticalRegion) {
							it->requestingObjectData->exitCriticalRegion();
						}
						
						free(buffer);
						it->bytesRead += bytesRead;
						VisualTiming::resetTimestamp(it->timingId);
					} else {
						endOfStream = true;
					}
					if (debug == true) {
						char logStr[256];
						sprintf(logStr, "bytes read: %ld (%d)\n", bytesRead, it->bytesRead);
						writeLog(logStr);
					}

					/*
					if (!(it->requestingObjectData->dataSizeDeclaredHasBeenSet())) {
						// check for content-length value in header data
						char szSizeBuffer[32];
						DWORD dwLengthSizeBuffer = sizeof(szSizeBuffer);
						BOOL success = HttpQueryInfo(it->readRequest, HTTP_QUERY_CONTENT_LENGTH, szSizeBuffer, &dwLengthSizeBuffer, NULL);
						if (success) {
							DWORD dwFileSize = atol(szSizeBuffer);
							it->requestingObjectData->setDataSizeDeclared(dwFileSize);
						}
					}
					 */

#endif
#if TARGET_OS_MAC
				} else { // no success
				
					CFStreamStatus streamStatus = CFReadStreamGetStatus(it->readStreamRef);
					if (streamStatus == kCFStreamStatusAtEnd) {
						endOfStream = true;
					}
					
					//writeLog("stream has no bytes available\n");
					//CFStreamError streamError = CFReadStreamGetError(*it);
#endif
				}
				if (endOfStream) {
					if (debug == true) {
						writeLog("stream is at end");
					}
#if TARGET_OS_MAC
					CFReadStreamClose(it->readStreamRef);
					CFRelease(it->readStreamRef);
#endif
#if TARGET_OS_WIN
					InternetCloseHandle(it->readRequest);
					InternetCloseHandle(it->session);
#endif
					it->streamDidClose = true;
					
					it->isActive = false;
					
				}
			}
			it++;
		}
		
#if TARGET_OS_MAC
		MPCriticalRegionID addToDownloadQueueCriticalRegion = VisualThreadingManager::getCriticalRegionID(VisualThreadingManager::addRequestDownloadQueueAccess);
		status = MPEnterCriticalRegion(addToDownloadQueueCriticalRegion, kDurationForever);
		if (status != noErr) return NULL;
#endif
#if TARGET_OS_WIN
		LPCRITICAL_SECTION addToDownloadQueueCriticalSection = VisualThreadingManager::getCriticalSectionPointer(VisualThreadingManager::addRequestDownloadQueueAccess);
		::EnterCriticalSection(addToDownloadQueueCriticalSection);
#endif
		DownloadAddRequestQueueIterator addRequestIt = theVisualDownloadQueue->downloadAddRequestQueue.begin();
		while (addRequestIt != theVisualDownloadQueue->downloadAddRequestQueue.end()) {
			theVisualDownloadQueue->downloadQueue.push_back(*addRequestIt);
			addRequestIt = theVisualDownloadQueue->downloadAddRequestQueue.erase(addRequestIt);
		}
		theVisualDownloadQueue->downloadAddRequestQueue.clear();
		theVisualDownloadQueue->downloadAddRequestQueueIsEmpty = true;
#if TARGET_OS_MAC
		status = MPExitCriticalRegion(addToDownloadQueueCriticalRegion);
#endif
#if TARGET_OS_WIN
		::LeaveCriticalSection(addToDownloadQueueCriticalSection);
#endif
		
#if TARGET_OS_MAC
		MPCriticalRegionID removeFromDownloadQueueCriticalRegion = VisualThreadingManager::getCriticalRegionID(VisualThreadingManager::stopRequestDownloadQueueAccess);
		status = MPEnterCriticalRegion(removeFromDownloadQueueCriticalRegion, kDurationForever);
		if (status != noErr) return NULL;
#endif
#if TARGET_OS_WIN
		LPCRITICAL_SECTION removeFromDownloadQueueCriticalSection = VisualThreadingManager::getCriticalSectionPointer(VisualThreadingManager::stopRequestDownloadQueueAccess);
		::EnterCriticalSection(removeFromDownloadQueueCriticalSection);
#endif
		DownloadStopRequestQueueIterator stopRequestIt = theVisualDownloadQueue->downloadStopRequestQueue.begin();
		while (stopRequestIt != theVisualDownloadQueue->downloadStopRequestQueue.end()) {
			bool found = false;
			DownloadQueueIterator downloadQueueIt = theVisualDownloadQueue->downloadQueue.begin();
			while (downloadQueueIt != theVisualDownloadQueue->downloadQueue.end() && (found == false)) {
				if (downloadQueueIt->identifier == *stopRequestIt) {
					downloadQueueIt->isActive = false;
					found = true;
				}
			}
			stopRequestIt = theVisualDownloadQueue->downloadStopRequestQueue.erase(stopRequestIt);
		}

		it = theVisualDownloadQueue->downloadQueue.begin();
		while (it != theVisualDownloadQueue->downloadQueue.end()) {
			if (!(it->isActive)) {
				if (it->streamDidClose == false) {
#if TARGET_OS_MAC
					CFReadStreamClose(it->readStreamRef);
					CFRelease(it->readStreamRef);
#endif
#if TARGET_OS_WIN
					InternetCloseHandle(it->readRequest);
					InternetCloseHandle(it->session);
#endif
					it->streamDidClose = true;
				}


				// claim access of object
				bool objectIsAccessible = false;
				bool needToExitCriticalRegion = false;
				if (it->requestingObjectData->hasObject()) {
					it->requestingObjectData->enterCriticalRegion();
					needToExitCriticalRegion = true;
					if (it->requestingObjectData->hasObject()) {
						objectIsAccessible = true;
					}
				}
				if (!objectIsAccessible) {
					it->requestingObjectData->exitCriticalRegion();
					needToExitCriticalRegion = false;
					it->requestingObjectData->release();
					if (it->requestingObjectData->getRefCount() == 0) {
						delete it->requestingObjectData;
						it->requestingObjectData = NULL;
					}
				}

				if (it->requestingObjectData && (it->requestingObjectData->getDataSizeUsed() == it->bytesRead)) {
					it->requestingObject->dataLoadDidEnd(it->requestId);
					it->requestingObjectData->setIsDownloading(false);
				}
				if (it->requestingObjectData && needToExitCriticalRegion) {
					it->requestingObjectData->exitCriticalRegion();
				}
				if (it->requestingObjectData) {
					it->requestingObjectData->release();
					if (it->requestingObjectData->getRefCount() == 0) {
						delete it->requestingObjectData;
					}
				}
				
				it = theVisualDownloadQueue->downloadQueue.erase(it);
			} else {
				it++;
			}
		}
#if TARGET_OS_MAC
		status = MPExitCriticalRegion(removeFromDownloadQueueCriticalRegion);
#endif
#if TARGET_OS_WIN
		::LeaveCriticalSection(removeFromDownloadQueueCriticalSection);
#endif

#if TARGET_OS_MAC
		status = MPExitCriticalRegion(visualDownloadQueueAccessCriticalRegion);
#endif
#if TARGET_OS_WIN
		::LeaveCriticalSection(visualDownloadQueueAccessCriticalSection);
#endif
		
		if (theVisualDownloadQueue->downloadQueue.empty()) {
			done = true;
		}
		
		theVisualDownloadQueue->downloadIsProcessing = false;
		
	}
	
	return retVal;
}


bool VisualDownloadQueue::shouldStartDownload() {
	bool downloadShouldBeStarted = false;
	if ((!this->downloadAddRequestQueueIsEmpty) && (!this->downloadIsProcessing) && (!this->stopDownloadProcess)) {
		// only start download thread if jobs are waiting in queue, download is not yet processing, and download is not supposed to stop
		downloadShouldBeStarted = true;
	}
	return downloadShouldBeStarted;
}
