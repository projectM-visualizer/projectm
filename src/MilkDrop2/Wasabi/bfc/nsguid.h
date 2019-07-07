#ifndef _NSGUID_H
#define _NSGUID_H
#include "platform/guid.h"
#include <bfc/platform/guid.h>
//#include <bfc/common.h>

// Some conversion functions to allow
// us to have GUIDs translatable to and from other data types.
class  nsGUID {
public:
  // To the "Human Readable" character format.
  // {1B3CA60C-DA98-4826-B4A9-D79748A5FD73}
  static char *toChar(const GUID &guid, char *target);
	static wchar_t *toCharW(const GUID &guid, wchar_t *target);
	static GUID fromCharW(const wchar_t *source);
  // To the "C Structure" character format.
  //  { 0x1b3ca60c, 0xda98, 0x4826, { 0xb4, 0xa9, 0xd7, 0x97, 0x48, 0xa5, 0xfd, 0x73 } };
  static char *toCode(const GUID &guid, char *target);
  static GUID fromCode(const char *source);

  // Compare function, returns -1, 0, 1
  static int compare(const GUID &a, const GUID &b);

  // strlen("{xx xxx xxx-xxxx-xxxx-xxxx-xxx xxx xxx xxx}"
  enum { GUID_STRLEN = 38 };

#ifdef WASABI_COMPILE_CREATEGUID
  static void createGuid(GUID *g);
#endif
};


#endif  //_NSGUID_H
