#ifndef NULLSOFT_APIH
#define NULLSOFT_APIH

#include <api/service/api_service.h>
#include "../Agave/Language/api_language.h"
#include <api/service/waServiceFactory.h>
#include <api/syscb/callbacks/browsercb.h>
#include <api/syscb/callbacks/syscb.h>
#include <api/syscb/api_syscb.h>
extern api_syscb *WASABI_API_SYSCB;
#define WASABI_API_SYSCB sysCallbackApi
#include <api/application/api_application.h>
extern api_application *applicationApi;
#define WASABI_API_APP applicationApi

#endif