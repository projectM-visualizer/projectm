#pragma once
//#include <bfc/platform/platform.h>
#include <bfc/platform/types.h>
#include <bfc/platform/guid.h>

#ifdef WIN32
#ifndef NOVTABLE
#define NOVTABLE __declspec(novtable)
#endif
#else
#define NOVTABLE
#endif
class DispatchableCallback;

#pragma warning(disable: 4786)
#pragma warning(disable: 4275)
#pragma warning(disable: 4100)

enum
{
	DISPATCH_SUCCESS=0,
	DISPATCH_FAILURE=1,
};


class NOVTABLE Dispatchable {
public:
//  // fake virtual destructor
//  void destruct() { _voidcall(DESTRUCT); }

  // this is virtual so it is visible across modules
  virtual int _dispatch(int msg, void *retval, void **params=0, int nparam=0)=0;
	

  /* added 22 May 2007. these aren't used yet.  To be used in the future
     in the meantime, don't use negative numbers for your msg values */
  size_t AddRef();
  size_t Release();
  int QueryInterface(GUID interface_guid, void **object);
  enum
  {
	  ADDREF=-1,
	  RELEASE=-2,
	  QUERYINTERFACE=-3,
  };
protected:
//  // protected real destructor
//  ~Dispatchable() {}
  // helper templates to implement client-side methods
  int _voidcall(int msg) {
    return _dispatch(msg, 0);
  }

  template<class PARAM1>
  int _voidcall(int msg, PARAM1 param1) {
    void *params[1] = { &param1 };
    return _dispatch(msg, 0, params, 1);
  }

  template<class PARAM1, class PARAM2>
  int _voidcall(int msg, PARAM1 param1, PARAM2 param2) {
    void *params[2] = { &param1, &param2 };
    return _dispatch(msg, 0, params, 2);
  }

  template<class PARAM1, class PARAM2, class PARAM3>
  int _voidcall(int msg, PARAM1 param1, PARAM2 param2, PARAM3 param3) {
    void *params[3] = { &param1, &param2, &param3 };
    return _dispatch(msg, 0, params, 3);
  }

  template<class PARAM1, class PARAM2, class PARAM3, class PARAM4>
  int _voidcall(int msg, PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4) {
    void *params[4] = { &param1, &param2, &param3, &param4 };
    return _dispatch(msg, 0, params, 4);
  }

  template<class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5>
  int _voidcall(int msg, PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4, PARAM5 param5) {
//    void *params[4] = { &param1, &param2, &param3, &param4, &param5 }; // mig found another bug
    void *params[5] = { &param1, &param2, &param3, &param4, &param5 };
    return _dispatch(msg, 0, params, 5);
  }

  template<class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6>
  int _voidcall(int msg, PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4, PARAM5 param5, PARAM6 param6) {
//    void *params[4] = { &param1, &param2, &param3, &param4, &param5, &param6 }; // mig found another bug
    void *params[6] = { &param1, &param2, &param3, &param4, &param5, &param6 };
    return _dispatch(msg, 0, params, 6);
  }

  template<class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7>
  int _voidcall(int msg, PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4, PARAM5 param5, PARAM6 param6, PARAM7 param7) {
    void *params[7] = { &param1, &param2, &param3, &param4, &param5, &param6 , &param7 };
    return _dispatch(msg, 0, params, 7);
  }

  template<class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7, class PARAM8>
  int _voidcall(int msg, PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4, PARAM5 param5, PARAM6 param6, PARAM7 param7, PARAM8 param8) {
    void *params[8] = { &param1, &param2, &param3, &param4, &param5, &param6 , &param7 , &param8 };
    return _dispatch(msg, 0, params, 8);
  }

  template<class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7, class PARAM8, class PARAM9>
  int _voidcall(int msg, PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4, PARAM5 param5, PARAM6 param6, PARAM7 param7, PARAM8 param8, PARAM9 param9) {
    void *params[9] = { &param1, &param2, &param3, &param4, &param5, &param6 , &param7 , &param8 , &param9 };
    return _dispatch(msg, 0, params, 9);
  }

  template<class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7, class PARAM8, class PARAM9, class PARAM10>
  int _voidcall(int msg, PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4, PARAM5 param5, PARAM6 param6, PARAM7 param7, PARAM8 param8, PARAM9 param9, PARAM10 param10) {
    void *params[10] = { &param1, &param2, &param3, &param4, &param5, &param6 , &param7 , &param8 , &param9 , &param10 };
    return _dispatch(msg, 0, params, 10);
  }

  template<class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7, class PARAM8, class PARAM9, class PARAM10, class PARAM11, class PARAM12, class PARAM13, class PARAM14>
  int _voidcall(int msg, PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4, PARAM5 param5, PARAM6 param6, PARAM7 param7, PARAM8 param8, PARAM9 param9, PARAM10 param10, PARAM11 param11, PARAM12 param12, PARAM13 param13, PARAM14 param14) {
    void *params[14] = { &param1, &param2, &param3, &param4, &param5, &param6 , &param7 , &param8 , &param9 , &param10 , &param11 , &param12 , &param13 , &param14 };
    return _dispatch(msg, 0, params, 14);
  }

  template<class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7, class PARAM8, class PARAM9, class PARAM10, class PARAM11, class PARAM12, class PARAM13, class PARAM14, class PARAM15, class PARAM16>
  int _voidcall(int msg, PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4, PARAM5 param5, PARAM6 param6, PARAM7 param7, PARAM8 param8, PARAM9 param9, PARAM10 param10, PARAM11 param11, PARAM12 param12, PARAM13 param13, PARAM14 param14, PARAM15 param15, PARAM16 param16) {
    void *params[16] = { &param1, &param2, &param3, &param4, &param5, &param6 , &param7 , &param8 , &param9 , &param10 , &param11 , &param12 , &param13 , &param14 , &param15 , &param16 };
    return _dispatch(msg, 0, params, 16);
  }

  template<class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7, class PARAM8, class PARAM9, class PARAM10, class PARAM11, class PARAM12, class PARAM13, class PARAM14, class PARAM15, class PARAM16, class PARAM17>
  int _voidcall(int msg, PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4, PARAM5 param5, PARAM6 param6, PARAM7 param7, PARAM8 param8, PARAM9 param9, PARAM10 param10, PARAM11 param11, PARAM12 param12, PARAM13 param13, PARAM14 param14, PARAM15 param15, PARAM16 param16, PARAM17 param17) {
    void *params[17] = { &param1, &param2, &param3, &param4, &param5, &param6 , &param7 , &param8 , &param9 , &param10 , &param11 , &param12 , &param13 , &param14 , &param15 , &param16 , &param17 };
    return _dispatch(msg, 0, params, 17);
  }


  template<class RETURN_TYPE>
  RETURN_TYPE _call(int msg, RETURN_TYPE defval) {
    RETURN_TYPE retval;
    if (_dispatch(msg, &retval)) return retval;
    return defval;
  }

  template<class RETURN_TYPE, class PARAM1>
  RETURN_TYPE _call(int msg, RETURN_TYPE defval, PARAM1 param1) {
    void *params[1] = { &param1 };
    RETURN_TYPE retval;
    if (_dispatch(msg, &retval, params, 1)) return retval;
    return defval;
  }

  template<class RETURN_TYPE, class PARAM1, class PARAM2>
  RETURN_TYPE _call(int msg, RETURN_TYPE defval, PARAM1 param1, PARAM2 param2) {
    void *params[2] = { &param1, &param2 };
    RETURN_TYPE retval;
    if (_dispatch(msg, &retval, params, 2)) return retval;
    return defval;
  }

  template<class RETURN_TYPE, class PARAM1, class PARAM2, class PARAM3>
  RETURN_TYPE _call(int msg, RETURN_TYPE defval, PARAM1 param1, PARAM2 param2, PARAM3 param3) {
    void *params[3] = { &param1, &param2, &param3 };
    RETURN_TYPE retval;
    if (_dispatch(msg, &retval, params, 3)) return retval;
    return defval;
  }

  template<class RETURN_TYPE, class PARAM1, class PARAM2, class PARAM3, class PARAM4>
  RETURN_TYPE _call(int msg, RETURN_TYPE defval, PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4) {
    void *params[4] = { &param1, &param2, &param3, &param4 };
    RETURN_TYPE retval;
    if (_dispatch(msg, &retval, params, 4)) return retval;
    return defval;
  }

  template<class RETURN_TYPE, class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5>
  RETURN_TYPE _call(int msg, RETURN_TYPE defval, PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4, PARAM5 param5) {
    void *params[5] = { &param1, &param2, &param3, &param4, &param5 };
    RETURN_TYPE retval;
    if (_dispatch(msg, &retval, params, 5)) return retval;
    return defval;
  }

  template<class RETURN_TYPE, class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6>
  RETURN_TYPE _call(int msg, RETURN_TYPE defval, PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4, PARAM5 param5, PARAM6 param6) {
    void *params[6] = { &param1, &param2, &param3, &param4, &param5, &param6 };
    RETURN_TYPE retval;
    if (_dispatch(msg, &retval, params, 6)) return retval;
    return defval;
  }

  template<class RETURN_TYPE, class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7>
  RETURN_TYPE _call(int msg, RETURN_TYPE defval, PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4, PARAM5 param5, PARAM6 param6, PARAM7 param7) {
    void *params[7] = { &param1, &param2, &param3, &param4, &param5, &param6, &param7 };
    RETURN_TYPE retval;
    if (_dispatch(msg, &retval, params, 7)) return retval;
    return defval;
  }

  template<class RETURN_TYPE, class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7, class PARAM8>
  RETURN_TYPE _call(int msg, RETURN_TYPE defval, PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4, PARAM5 param5, PARAM6 param6, PARAM7 param7, PARAM8 param8) {
    void *params[8] = { &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8 };
    RETURN_TYPE retval;
    if (_dispatch(msg, &retval, params, 8)) return retval;
    return defval;
  }

  template<class RETURN_TYPE, class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7, class PARAM8, class PARAM9>
  RETURN_TYPE _call(int msg, RETURN_TYPE defval, PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4, PARAM5 param5, PARAM6 param6, PARAM7 param7, PARAM8 param8, PARAM9 param9) {
    void *params[9] = { &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9 };
    RETURN_TYPE retval;
    if (_dispatch(msg, &retval, params, 9)) return retval;
    return defval;
  }

  template<class RETURN_TYPE, class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7, class PARAM8, class PARAM9, class PARAM10>
  RETURN_TYPE _call(int msg, RETURN_TYPE defval, PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4, PARAM5 param5, PARAM6 param6, PARAM7 param7, PARAM8 param8, PARAM9 param9, PARAM10 param10) {
    void *params[10] = { &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10 };
    RETURN_TYPE retval;
    if (_dispatch(msg, &retval, params, 10)) return retval;
    return defval;
  }

  template <class CASTNAME, class CLASSNAME, class RETVAL>
  void cb(RETVAL (CLASSNAME::*fn)(), void *retval, void **params) {
    *static_cast<RETVAL*>(retval) = (static_cast<CLASSNAME *>(static_cast<CASTNAME *>(this))->*fn)();
  }

  template <class CASTNAME, class CLASSNAME>
  void vcb(void (CLASSNAME::*fn)(), void *retval, void **params) {
    (static_cast<CLASSNAME *>(static_cast<CASTNAME *>(this))->*fn)();
  }

  template <class CASTNAME, class CLASSNAME, class RETVAL, class PARAM1>
  void cb(RETVAL (CLASSNAME::*fn)(PARAM1), void *retval, void **params) {
    PARAM1 *p1 = static_cast<PARAM1*>(params[0]);
    *static_cast<RETVAL*>(retval) = (static_cast<CLASSNAME *>(static_cast<CASTNAME *>(this))->*fn)(*p1);
  }

  template <class CASTNAME, class CLASSNAME, class PARAM1>
  void vcb(void (CLASSNAME::*fn)(PARAM1), void *retval, void **params) {
    PARAM1 *p1 = static_cast<PARAM1*>(params[0]);
    (static_cast<CLASSNAME *>(static_cast<CASTNAME *>(this))->*fn)(*p1);
  }

  template <class CASTNAME, class CLASSNAME, class PARAM1, class PARAM2>
  void vcb(void (CLASSNAME::*fn)(PARAM1, PARAM2), void *retval, void **params) {
    PARAM1 *p1 = static_cast<PARAM1*>(params[0]);
    PARAM2 *p2 = static_cast<PARAM2*>(params[1]);
    (static_cast<CLASSNAME *>(static_cast<CASTNAME *>(this))->*fn)(*p1, *p2);
  }

  template <class CASTNAME, class CLASSNAME, class RETVAL, class PARAM1, class PARAM2>
  void cb(RETVAL (CLASSNAME::*fn)(PARAM1, PARAM2), void *retval, void **params) {
    PARAM1 *p1 = static_cast<PARAM1*>(params[0]);
    PARAM2 *p2 = static_cast<PARAM2*>(params[1]);
    *static_cast<RETVAL*>(retval) = (static_cast<CLASSNAME *>(static_cast<CASTNAME *>(this))->*fn)(*p1, *p2);
  }

  // 3 params
  template <class CASTNAME, class CLASSNAME, class PARAM1, class PARAM2, class PARAM3>
  void vcb(void (CLASSNAME::*fn)(PARAM1, PARAM2, PARAM3), void *retval, void **params) {
    PARAM1 *p1 = static_cast<PARAM1*>(params[0]);
    PARAM2 *p2 = static_cast<PARAM2*>(params[1]);
    PARAM3 *p3 = static_cast<PARAM3*>(params[2]);
    (static_cast<CLASSNAME *>(static_cast<CASTNAME *>(this))->*fn)(*p1, *p2, *p3);
  }

  template <class CASTNAME, class CLASSNAME, class RETVAL, class PARAM1, class PARAM2, class PARAM3>
  void cb(RETVAL (CLASSNAME::*fn)(PARAM1, PARAM2, PARAM3), void *retval, void **params) {
    PARAM1 *p1 = static_cast<PARAM1*>(params[0]);
    PARAM2 *p2 = static_cast<PARAM2*>(params[1]);
    PARAM3 *p3 = static_cast<PARAM3*>(params[2]);
    *static_cast<RETVAL*>(retval) = (static_cast<CLASSNAME *>(static_cast<CASTNAME *>(this))->*fn)(*p1, *p2, *p3);
  }

  // 4 params
  template <class CASTNAME, class CLASSNAME, class PARAM1, class PARAM2, class PARAM3, class PARAM4>
  void vcb(void (CLASSNAME::*fn)(PARAM1, PARAM2, PARAM3, PARAM4), void *retval, void **params) {
    PARAM1 *p1 = static_cast<PARAM1*>(params[0]);
    PARAM2 *p2 = static_cast<PARAM2*>(params[1]);
    PARAM3 *p3 = static_cast<PARAM3*>(params[2]);
    PARAM4 *p4 = static_cast<PARAM4*>(params[3]);
    (static_cast<CLASSNAME *>(static_cast<CASTNAME *>(this))->*fn)(*p1, *p2, *p3, *p4);
  }

  template <class CASTNAME, class CLASSNAME, class RETVAL, class PARAM1, class PARAM2, class PARAM3, class PARAM4>
  void cb(RETVAL (CLASSNAME::*fn)(PARAM1, PARAM2, PARAM3, PARAM4), void *retval, void **params) {
    PARAM1 *p1 = static_cast<PARAM1*>(params[0]);
    PARAM2 *p2 = static_cast<PARAM2*>(params[1]);
    PARAM3 *p3 = static_cast<PARAM3*>(params[2]);
    PARAM4 *p4 = static_cast<PARAM4*>(params[3]);
    *static_cast<RETVAL*>(retval) = (static_cast<CLASSNAME *>(static_cast<CASTNAME *>(this))->*fn)(*p1, *p2, *p3, *p4);
  }

  // 5 params
  template <class CASTNAME, class CLASSNAME, class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5>
  void vcb(void (CLASSNAME::*fn)(PARAM1, PARAM2, PARAM3, PARAM4, PARAM5), void *retval, void **params) {
    PARAM1 *p1 = static_cast<PARAM1*>(params[0]);
    PARAM2 *p2 = static_cast<PARAM2*>(params[1]);
    PARAM3 *p3 = static_cast<PARAM3*>(params[2]);
    PARAM4 *p4 = static_cast<PARAM4*>(params[3]);
    PARAM5 *p5 = static_cast<PARAM5*>(params[4]);
    (static_cast<CLASSNAME *>(static_cast<CASTNAME *>(this))->*fn)(*p1, *p2, *p3, *p4, *p5);
  }

  template <class CASTNAME, class CLASSNAME, class RETVAL, class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5>
  void cb(RETVAL (CLASSNAME::*fn)(PARAM1, PARAM2, PARAM3, PARAM4, PARAM5), void *retval, void **params) {
    PARAM1 *p1 = static_cast<PARAM1*>(params[0]);
    PARAM2 *p2 = static_cast<PARAM2*>(params[1]);
    PARAM3 *p3 = static_cast<PARAM3*>(params[2]);
    PARAM4 *p4 = static_cast<PARAM4*>(params[3]);
    PARAM5 *p5 = static_cast<PARAM5*>(params[4]);
    *static_cast<RETVAL*>(retval) = (static_cast<CLASSNAME *>(static_cast<CASTNAME *>(this))->*fn)(*p1, *p2, *p3, *p4, *p5);
  }

  // 6 params
  template <class CASTNAME, class CLASSNAME, class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6>
  void vcb(void (CLASSNAME::*fn)(PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6), void *retval, void **params) {
    PARAM1 *p1 = static_cast<PARAM1*>(params[0]);
    PARAM2 *p2 = static_cast<PARAM2*>(params[1]);
    PARAM3 *p3 = static_cast<PARAM3*>(params[2]);
    PARAM4 *p4 = static_cast<PARAM4*>(params[3]);
    PARAM5 *p5 = static_cast<PARAM5*>(params[4]);
    PARAM6 *p6 = static_cast<PARAM6*>(params[5]);
    (static_cast<CLASSNAME *>(static_cast<CASTNAME *>(this))->*fn)(*p1, *p2, *p3, *p4, *p5, *p6);
  }

  template <class CASTNAME, class CLASSNAME, class RETVAL, class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6>
  void cb(RETVAL (CLASSNAME::*fn)(PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6), void *retval, void **params) {
    PARAM1 *p1 = static_cast<PARAM1*>(params[0]);
    PARAM2 *p2 = static_cast<PARAM2*>(params[1]);
    PARAM3 *p3 = static_cast<PARAM3*>(params[2]);
    PARAM4 *p4 = static_cast<PARAM4*>(params[3]);
    PARAM5 *p5 = static_cast<PARAM5*>(params[4]);
    PARAM6 *p6 = static_cast<PARAM6*>(params[5]);
    *static_cast<RETVAL*>(retval) = (static_cast<CLASSNAME *>(static_cast<CASTNAME *>(this))->*fn)(*p1, *p2, *p3, *p4, *p5, *p6);
  }

  // 7 params
  template <class CASTNAME, class CLASSNAME, class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7>
  void vcb(void (CLASSNAME::*fn)(PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6, PARAM7), void *retval, void **params) {
    PARAM1 *p1 = static_cast<PARAM1*>(params[0]);
    PARAM2 *p2 = static_cast<PARAM2*>(params[1]);
    PARAM3 *p3 = static_cast<PARAM3*>(params[2]);
    PARAM4 *p4 = static_cast<PARAM4*>(params[3]);
    PARAM5 *p5 = static_cast<PARAM5*>(params[4]);
    PARAM6 *p6 = static_cast<PARAM6*>(params[5]);
    PARAM7 *p7 = static_cast<PARAM7*>(params[6]);
    (static_cast<CLASSNAME *>(static_cast<CASTNAME *>(this))->*fn)(*p1, *p2, *p3, *p4, *p5, *p6, *p7);
  }

  template <class CASTNAME, class CLASSNAME, class RETVAL, class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7>
  void cb(RETVAL (CLASSNAME::*fn)(PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6, PARAM7), void *retval, void **params) {
    PARAM1 *p1 = static_cast<PARAM1*>(params[0]);
    PARAM2 *p2 = static_cast<PARAM2*>(params[1]);
    PARAM3 *p3 = static_cast<PARAM3*>(params[2]);
    PARAM4 *p4 = static_cast<PARAM4*>(params[3]);
    PARAM5 *p5 = static_cast<PARAM5*>(params[4]);
    PARAM6 *p6 = static_cast<PARAM6*>(params[5]);
    PARAM7 *p7 = static_cast<PARAM7*>(params[6]);
    *static_cast<RETVAL*>(retval) = (static_cast<CLASSNAME *>(static_cast<CASTNAME *>(this))->*fn)(*p1, *p2, *p3, *p4, *p5, *p6, *p7);
  }

  // 8 params
  template <class CASTNAME, class CLASSNAME, class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7, class PARAM8>
  void vcb(void (CLASSNAME::*fn)(PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6, PARAM7, PARAM8), void *retval, void **params) {
    PARAM1 *p1 = static_cast<PARAM1*>(params[0]);
    PARAM2 *p2 = static_cast<PARAM2*>(params[1]);
    PARAM3 *p3 = static_cast<PARAM3*>(params[2]);
    PARAM4 *p4 = static_cast<PARAM4*>(params[3]);
    PARAM5 *p5 = static_cast<PARAM5*>(params[4]);
    PARAM6 *p6 = static_cast<PARAM6*>(params[5]);
    PARAM7 *p7 = static_cast<PARAM7*>(params[6]);
    PARAM8 *p8 = static_cast<PARAM8*>(params[7]);
    (static_cast<CLASSNAME *>(static_cast<CASTNAME *>(this))->*fn)(*p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8);
  }

  template <class CASTNAME, class CLASSNAME, class RETVAL, class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7, class PARAM8>
  void cb(RETVAL (CLASSNAME::*fn)(PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6, PARAM7, PARAM8), void *retval, void **params) {
    PARAM1 *p1 = static_cast<PARAM1*>(params[0]);
    PARAM2 *p2 = static_cast<PARAM2*>(params[1]);
    PARAM3 *p3 = static_cast<PARAM3*>(params[2]);
    PARAM4 *p4 = static_cast<PARAM4*>(params[3]);
    PARAM5 *p5 = static_cast<PARAM5*>(params[4]);
    PARAM6 *p6 = static_cast<PARAM6*>(params[5]);
    PARAM7 *p7 = static_cast<PARAM7*>(params[6]);
    PARAM8 *p8 = static_cast<PARAM8*>(params[7]);
    *static_cast<RETVAL*>(retval) = (static_cast<CLASSNAME *>(static_cast<CASTNAME *>(this))->*fn)(*p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8);
  }

  // 9 params
  template <class CASTNAME, class CLASSNAME, class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7, class PARAM8, class PARAM9>
  void vcb(void (CLASSNAME::*fn)(PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6, PARAM7, PARAM8, PARAM9), void *retval, void **params) {
    PARAM1 *p1 = static_cast<PARAM1*>(params[0]);
    PARAM2 *p2 = static_cast<PARAM2*>(params[1]);
    PARAM3 *p3 = static_cast<PARAM3*>(params[2]);
    PARAM4 *p4 = static_cast<PARAM4*>(params[3]);
    PARAM5 *p5 = static_cast<PARAM5*>(params[4]);
    PARAM6 *p6 = static_cast<PARAM6*>(params[5]);
    PARAM7 *p7 = static_cast<PARAM7*>(params[6]);
    PARAM8 *p8 = static_cast<PARAM8*>(params[7]);
    PARAM9 *p9 = static_cast<PARAM9*>(params[8]);
    (static_cast<CLASSNAME *>(static_cast<CASTNAME *>(this))->*fn)(*p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *p9);
  }

  template <class CASTNAME, class CLASSNAME, class RETVAL, class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7, class PARAM8, class PARAM9>
  void cb(RETVAL (CLASSNAME::*fn)(PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6, PARAM7, PARAM8, PARAM9), void *retval, void **params) {
    PARAM1 *p1 = static_cast<PARAM1*>(params[0]);
    PARAM2 *p2 = static_cast<PARAM2*>(params[1]);
    PARAM3 *p3 = static_cast<PARAM3*>(params[2]);
    PARAM4 *p4 = static_cast<PARAM4*>(params[3]);
    PARAM5 *p5 = static_cast<PARAM5*>(params[4]);
    PARAM6 *p6 = static_cast<PARAM6*>(params[5]);
    PARAM7 *p7 = static_cast<PARAM7*>(params[6]);
    PARAM8 *p8 = static_cast<PARAM8*>(params[7]);
    PARAM9 *p9 = static_cast<PARAM9*>(params[8]);
    *static_cast<RETVAL*>(retval) = (static_cast<CLASSNAME *>(static_cast<CASTNAME *>(this))->*fn)(*p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *p9);
  }

  // 10 params
  template <class CASTNAME, class CLASSNAME, class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7, class PARAM8, class PARAM9, class PARAM10>
  void vcb(void (CLASSNAME::*fn)(PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6, PARAM7, PARAM8, PARAM9, PARAM10), void *retval, void **params) {
    PARAM1 *p1 = static_cast<PARAM1*>(params[0]);
    PARAM2 *p2 = static_cast<PARAM2*>(params[1]);
    PARAM3 *p3 = static_cast<PARAM3*>(params[2]);
    PARAM4 *p4 = static_cast<PARAM4*>(params[3]);
    PARAM5 *p5 = static_cast<PARAM5*>(params[4]);
    PARAM6 *p6 = static_cast<PARAM6*>(params[5]);
    PARAM7 *p7 = static_cast<PARAM7*>(params[6]);
    PARAM8 *p8 = static_cast<PARAM8*>(params[7]);
    PARAM9 *p9 = static_cast<PARAM9*>(params[8]);
    PARAM10 *p10 = static_cast<PARAM10*>(params[9]);
    (static_cast<CLASSNAME *>(static_cast<CASTNAME *>(this))->*fn)(*p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *p9, *p10);
  }

  template <class CASTNAME, class CLASSNAME, class RETVAL, class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7, class PARAM8, class PARAM9, class PARAM10>
  void cb(RETVAL (CLASSNAME::*fn)(PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6, PARAM7, PARAM8, PARAM9, PARAM10), void *retval, void **params) {
    PARAM1 *p1 = static_cast<PARAM1*>(params[0]);
    PARAM2 *p2 = static_cast<PARAM2*>(params[1]);
    PARAM3 *p3 = static_cast<PARAM3*>(params[2]);
    PARAM4 *p4 = static_cast<PARAM4*>(params[3]);
    PARAM5 *p5 = static_cast<PARAM5*>(params[4]);
    PARAM6 *p6 = static_cast<PARAM6*>(params[5]);
    PARAM7 *p7 = static_cast<PARAM7*>(params[6]);
    PARAM8 *p8 = static_cast<PARAM8*>(params[7]);
    PARAM9 *p9 = static_cast<PARAM9*>(params[8]);
    PARAM10 *p10 = static_cast<PARAM10*>(params[9]);
    *static_cast<RETVAL*>(retval) = (static_cast<CLASSNAME *>(static_cast<CASTNAME *>(this))->*fn)(*p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *p9, *p10);
  }

  // 14 params
  template <class CASTNAME, class CLASSNAME, class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7, class PARAM8, class PARAM9, class PARAM10, class PARAM11, class PARAM12, class PARAM13, class PARAM14>
  void vcb(void (CLASSNAME::*fn)(PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6, PARAM7, PARAM8, PARAM9, PARAM10, PARAM11, PARAM12, PARAM13, PARAM14), void *retval, void **params) {
    PARAM1 *p1 = static_cast<PARAM1*>(params[0]);
    PARAM2 *p2 = static_cast<PARAM2*>(params[1]);
    PARAM3 *p3 = static_cast<PARAM3*>(params[2]);
    PARAM4 *p4 = static_cast<PARAM4*>(params[3]);
    PARAM5 *p5 = static_cast<PARAM5*>(params[4]);
    PARAM6 *p6 = static_cast<PARAM6*>(params[5]);
    PARAM7 *p7 = static_cast<PARAM7*>(params[6]);
    PARAM8 *p8 = static_cast<PARAM8*>(params[7]);
    PARAM9 *p9 = static_cast<PARAM9*>(params[8]);
    PARAM10 *p10 = static_cast<PARAM10*>(params[9]);
    PARAM11 *p11 = static_cast<PARAM11*>(params[10]);
    PARAM12 *p12 = static_cast<PARAM12*>(params[11]);
    PARAM13 *p13 = static_cast<PARAM13*>(params[12]);
    PARAM14 *p14 = static_cast<PARAM14*>(params[13]);
    (static_cast<CLASSNAME *>(static_cast<CASTNAME *>(this))->*fn)(*p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *p9, *p10, *p11, *p12, *p13, *p14);
  }

  // 16 params
  template <class CASTNAME, class CLASSNAME, class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7, class PARAM8, class PARAM9, class PARAM10, class PARAM11, class PARAM12, class PARAM13, class PARAM14, class PARAM15, class PARAM16>
  void vcb(void (CLASSNAME::*fn)(PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6, PARAM7, PARAM8, PARAM9, PARAM10, PARAM11, PARAM12, PARAM13, PARAM14, PARAM15, PARAM16), void *retval, void **params) {
    PARAM1 *p1 = static_cast<PARAM1*>(params[0]);
    PARAM2 *p2 = static_cast<PARAM2*>(params[1]);
    PARAM3 *p3 = static_cast<PARAM3*>(params[2]);
    PARAM4 *p4 = static_cast<PARAM4*>(params[3]);
    PARAM5 *p5 = static_cast<PARAM5*>(params[4]);
    PARAM6 *p6 = static_cast<PARAM6*>(params[5]);
    PARAM7 *p7 = static_cast<PARAM7*>(params[6]);
    PARAM8 *p8 = static_cast<PARAM8*>(params[7]);
    PARAM9 *p9 = static_cast<PARAM9*>(params[8]);
    PARAM10 *p10 = static_cast<PARAM10*>(params[9]);
    PARAM11 *p11 = static_cast<PARAM11*>(params[10]);
    PARAM12 *p12 = static_cast<PARAM12*>(params[11]);
    PARAM13 *p13 = static_cast<PARAM13*>(params[12]);
    PARAM14 *p14 = static_cast<PARAM14*>(params[13]);
    PARAM15 *p15 = static_cast<PARAM15*>(params[14]);
    PARAM16 *p16 = static_cast<PARAM16*>(params[15]);
    (static_cast<CLASSNAME *>(static_cast<CASTNAME *>(this))->*fn)(*p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *p9, *p10, *p11, *p12, *p13, *p14, *p15, *p16);
  }

  // 17 params
  template <class CASTNAME, class CLASSNAME, class PARAM1, class PARAM2, class PARAM3, class PARAM4, class PARAM5, class PARAM6, class PARAM7, class PARAM8, class PARAM9, class PARAM10, class PARAM11, class PARAM12, class PARAM13, class PARAM14, class PARAM15, class PARAM16, class PARAM17>
  void vcb(void (CLASSNAME::*fn)(PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6, PARAM7, PARAM8, PARAM9, PARAM10, PARAM11, PARAM12, PARAM13, PARAM14, PARAM15, PARAM16, PARAM17), void *retval, void **params) {
    PARAM1 *p1 = static_cast<PARAM1*>(params[0]);
    PARAM2 *p2 = static_cast<PARAM2*>(params[1]);
    PARAM3 *p3 = static_cast<PARAM3*>(params[2]);
    PARAM4 *p4 = static_cast<PARAM4*>(params[3]);
    PARAM5 *p5 = static_cast<PARAM5*>(params[4]);
    PARAM6 *p6 = static_cast<PARAM6*>(params[5]);
    PARAM7 *p7 = static_cast<PARAM7*>(params[6]);
    PARAM8 *p8 = static_cast<PARAM8*>(params[7]);
    PARAM9 *p9 = static_cast<PARAM9*>(params[8]);
    PARAM10 *p10 = static_cast<PARAM10*>(params[9]);
    PARAM11 *p11 = static_cast<PARAM11*>(params[10]);
    PARAM12 *p12 = static_cast<PARAM12*>(params[11]);
    PARAM13 *p13 = static_cast<PARAM13*>(params[12]);
    PARAM14 *p14 = static_cast<PARAM14*>(params[13]);
    PARAM15 *p15 = static_cast<PARAM15*>(params[14]);
    PARAM16 *p16 = static_cast<PARAM16*>(params[15]);
    PARAM17 *p17 = static_cast<PARAM17*>(params[16]);
    (static_cast<CLASSNAME *>(static_cast<CASTNAME *>(this))->*fn)(*p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *p9, *p10, *p11, *p12, *p13, *p14, *p15, *p16, *p17);
  }


  enum { DESTRUCT=0xffff };
};
#define CB(x, y) case (x): cb<CBCLASS>(&CBCLASS::y, retval, params); break;
#define CBT(x, y) case (x): cb<CBCLASST>(&CBCLASS::y, retval, params); break;
#define VCB(x, y) case (x): vcb<CBCLASS>(&CBCLASS::y, retval, params); break;
#define VCBT(x, y) case (x): vcb<CBCLASST>(&CBCLASS::y, retval, params); break;

#define RECVS_DISPATCH virtual int _dispatch(int msg, void *retval, void **params=0, int nparam=0)

#define START_DISPATCH \
  int CBCLASS::_dispatch(int msg, void *retval, void **params, int nparam) { \
    switch (msg) {
#define START_DISPATCH_INLINE \
  int  _dispatch(int msg, void *retval, void **params, int nparam) { \
    switch (msg) {

//FINISH      case DESTRUCT: delete this; return 1;
#define END_DISPATCH \
      default: return 0; \
    } \
    return 1; \
  }
#define FORWARD_DISPATCH(x) \
      default: return x::_dispatch(msg, retval, params, nparam); \
    } \
    return 1; \
  }

#define DISPATCH_CODES enum

inline size_t Dispatchable::AddRef()
{
	return _call(Dispatchable::ADDREF, 0);
}

inline size_t Dispatchable::Release()
{
	return _call(Dispatchable::RELEASE, 0);
}

inline int Dispatchable::QueryInterface(GUID interface_guid, void **object)
{
	return _call(Dispatchable::QUERYINTERFACE, 0, interface_guid, object);
}

#ifndef DECLARE_EXTERNAL_SERVICE
#define DECLARE_EXTERNAL_SERVICE(_type, _name) extern _type *_name
#endif

#ifndef DEFINE_EXTERNAL_SERVICE
#define DEFINE_EXTERNAL_SERVICE(_type, _name) _type *_name=0
#endif
