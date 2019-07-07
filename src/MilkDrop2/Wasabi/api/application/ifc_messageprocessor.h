#ifndef __WASABI_IFC_MESSAGEPROCESSOR_H
#define __WASABI_IFC_MESSAGEPROCESSOR_H

#include <bfc/dispatch.h>
#include <windows.h>
class ifc_messageprocessor : public Dispatchable
{
protected:
	ifc_messageprocessor() {}
	~ifc_messageprocessor() {}
public:
	bool ProcessMessage(MSG *msg); // return true to 'eat' the message
public:
	DISPATCH_CODES
	{
		IFC_MESSAGEPROCESSOR_PROCESS_MESSAGE = 10,
	};
};
inline bool ifc_messageprocessor::ProcessMessage(MSG *msg)
{
	return _call(IFC_MESSAGEPROCESSOR_PROCESS_MESSAGE, false, msg);
}

typedef ifc_messageprocessor api_messageprocessor; // TODO: CUT!
#endif