#ifndef WASABI2_FOUNDATION_GUID_H
#define WASABI2_FOUNDATION_GUID_H
#pragma once

#include "types.h"

#if !defined(_WIN32)
#ifdef __cplusplus
#if !defined(GUID_EQUALS_DEFINED) || !defined(_SYS_GUID_OPERATOR_EQ_)
#define GUID_EQUALS_DEFINED
#include <memory.h>
#include <string.h>
static __inline int operator ==(const GUID &a, const GUID &b) {
  return !memcmp(&a, &b, sizeof(GUID));
}
static __inline int operator !=(const GUID &a, const GUID &b) {
  return !!memcmp(&a, &b, sizeof(GUID));
}
#endif	//GUID_EQUALS_DEFINED
#endif	//__cplusplus
#else
#include <guiddef.h>
#endif

#ifdef __cplusplus
static __inline int operator <(const GUID &a, const GUID &b) {
  return memcmp(&a, &b, sizeof(GUID)) < 0;
}
#endif


static const GUID INVALID_GUID = { 0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0} };
static const GUID GENERIC_GUID = { 0xFFFFFFFF, 0xFFFF, 0xFFFF, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} };

#endif
