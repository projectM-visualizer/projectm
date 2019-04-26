#ifdef _WIN32

	// Including SDKDDKVer.h defines the highest available Windows platform.

	// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
	// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.

	#include <SDKDDKVer.h>

	#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	#endif

	#include <windows.h>
#elif __ANDROID__

	#include <jni.h>
	#include <errno.h>

	#include <string.h>
	#include <unistd.h>
	#include <sys/resource.h>

	#include <android/log.h>

#endif