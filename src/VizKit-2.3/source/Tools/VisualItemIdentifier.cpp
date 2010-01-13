/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualItemIdentifier.cpp
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

#include "VisualItemIdentifier.h"
#include "VisualErrorHandling.h"

#include <string>



using namespace VizKit;


VisualItemIdentifier::VisualItemIdentifierNameTokenMap VisualItemIdentifier::visualItemIdentifierNameTokenMap;


VisualItemIdentifier::VisualItemIdentifier() {
	name = (char*)malloc(128 * sizeof(char));
	strcpy(name, "\0");
	token = VisualItemIdentifier::getNextToken();
}


VisualItemIdentifier::VisualItemIdentifier(const char* const aName, uint32 aToken) {
	name = (char*)malloc(128 * sizeof(char));
	strcpy(name, aName);
	token = aToken;
}


VisualItemIdentifier::VisualItemIdentifier(const char* const aName) {
	name = (char*)malloc(128 * sizeof(char));
	strcpy(name, aName);
	std::string itemIdentifierString(aName);
	VisualItemIdentifierNameTokenMapIterator it = VisualItemIdentifier::visualItemIdentifierNameTokenMap.find(itemIdentifierString);
	if (it != VisualItemIdentifier::visualItemIdentifierNameTokenMap.end()) {
		token = it->second;
	} else {
		token = VisualItemIdentifier::getNextToken();
		VisualItemIdentifier::visualItemIdentifierNameTokenMap[itemIdentifierString] = token;
	}
}


VisualItemIdentifier::~VisualItemIdentifier() {
	free(name);
}


VisualItemIdentifier::VisualItemIdentifier(const VisualItemIdentifier& other) {
	this->copy(other);
}


VisualItemIdentifier& VisualItemIdentifier::operator=(const VisualItemIdentifier& other) {
	if (this != &other) {
		free(name);
		this->copy(other);
	}
	return *this;
}


bool VisualItemIdentifier::operator<(const VisualItemIdentifier& other) const {
	bool compareResult = false;
	if (this->token < other.token) {
		compareResult = true;
	}
	return compareResult;
}


bool VisualItemIdentifier::operator==(const VisualItemIdentifier& other) const {
	bool compareResult = false;
	if (this->token == other.token) {
		compareResult = true;
	}
	return compareResult;
}


bool VisualItemIdentifier::operator!=(const VisualItemIdentifier& other) {
	return !(*this == other);
}


VisualItemIdentifier* VisualItemIdentifier::createVisualItemIdentifier(const char* const aName) {
	VisualItemIdentifier* requestedVisualItemIdentifier = NULL;
	uint32 token = 0;
	std::string itemIdentifierString(aName);
	VisualItemIdentifierNameTokenMapIterator it = VisualItemIdentifier::visualItemIdentifierNameTokenMap.find(itemIdentifierString);
	if (it != VisualItemIdentifier::visualItemIdentifierNameTokenMap.end()) {
		token = it->second;
	} else {
		token = VisualItemIdentifier::getNextToken();
		VisualItemIdentifier::visualItemIdentifierNameTokenMap[itemIdentifierString] = token;
	}
	requestedVisualItemIdentifier = new VisualItemIdentifier(aName, token);
	return requestedVisualItemIdentifier;
}


const char* const VisualItemIdentifier::getName() {
	return this->name;
}


void VisualItemIdentifier::copy(const VisualItemIdentifier& other) {
	this->name = (char*)malloc(128 * sizeof(char));
	if (strlen(other.name) > 0) {
		strcpy(this->name, other.name);
	} else {
		strcpy(this->name, "\0");
	}
	this->token = other.token;
}


uint32 VisualItemIdentifier::getNextToken() {
	static uint32 theToken = 1;
	if (theToken > 4294967295UL) {
		writeLog("ERR: getNextToken: MAX exceeded");
		theToken = 1;
	}
	theToken += 1;
	return theToken;
}
