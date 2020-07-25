/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: TemplateActor.cpp
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

#include "TemplateActor.h"
#include "TemplateAction.h"
#include "VisualErrorHandling.h"
#include "VisualString.h"
#include "VisualStyledString.h"
#include "VisualNetwork.h"
#include "VisualDataStore.h"
#include "VisualFile.h"
#include "VisualNotification.h"


#if TARGET_OS_MAC
#include <Carbon/Carbon.h> // CFReadStreamRef, CFURLRef
#endif

#include <string>



using namespace VizKit;


TemplateActor::TemplateActor() {
	strcpy(actorName, "TEMPLATE");
	state = kVisActOn; // state must be kVisActOn to show the action of the actor as part of the VisualEnsemble (kVisActNoShow means not being called at all)
	theTemplateAction = new TemplateAction;
	stringOfSimilarArtists = NULL;
	similarArtistsStringCreationInProgress = false;
	this->createStringOfSimilarArtists();
}


TemplateActor::~TemplateActor() {
    delete theTemplateAction;
	similarArtistsStringVector.clear();
	if (stringOfSimilarArtists != NULL) {
		delete stringOfSimilarArtists;
	}
}


void TemplateActor::init() {
	VisualNotification::registerNotification(this, kStringWithIdentifierLoadedAndCreatedMsg);
	VisualNotification::registerNotification(this, kAudioPlayTrackChangedEvt);
}


void TemplateActor::show(const VisualPlayerState& visualPlayerState) {
    theTemplateAction->show();
}


void TemplateActor::handleSimilarArtistsStringCompleted() {

	if (this->stringOfSimilarArtists == NULL) return;
	
	VisualItemIdentifier currTrackIdentifier = VisualDataStore::getIdentifierOfCurrentTrack();
	if (currTrackIdentifier != this->similarArtistsCurrentTrackIdentifier) {
		return;
	}
	
	uint32 numberOfLines = this->stringOfSimilarArtists->getNumberOfLines();

	for (uint32 i = 0; i < numberOfLines; i++) {
		VisualString* visualString = this->stringOfSimilarArtists->createStringWithLine(i);
		if (visualString) {
			std::vector<VisualString> stringVector = visualString->splitWithDelimiter(",");
			if (stringVector.size() > 2) {
				this->similarArtistsStringVector.push_back(stringVector[2]);
			}
			delete visualString;
		}
	}
	
	VisualString* joinedString = VisualString::createJoinedString(this->similarArtistsStringVector);
	if (joinedString != NULL) {
		this->setStringOfSimilarArtists(*joinedString);
		delete joinedString;
	}
}


void TemplateActor::setStringOfSimilarArtists(const VisualString& similarArtists) {
	VisualStyledString styledString;
	styledString.initWithVisualString(similarArtists);
	VisualStringStyle stringStyle;
	stringStyle.horizontalAlignment = kRightAligned;
	styledString.setStyle(stringStyle);

	bool success = this->similarArtistsStringImage.initWithStyledString(styledString);
	if (success) {
		this->theTemplateAction->setImage(this->similarArtistsStringImage);
		/*
		VisualFile aFile;
		aFile.initWithUserDesktopDirectory();
		VisualString fileName("testSimilarArtists.png");
		success = aFile.appendFileName(fileName);
		this->similarArtistsStringImage.writeToPNGFile(aFile);
		*/
	}
}


void TemplateActor::handleNotification(VisualNotification& aNotification) {

	//VisualActor::handleNotification(aNotification); // debug

	VisualNotificationKey notificationKey = aNotification.getKey();
	
	switch (notificationKey) {
			
		case kStringWithIdentifierLoadedAndCreatedMsg:
			if (aNotification.getIdentifier() == this->similarArtistsStringRequestId) {
				this->handleSimilarArtistsStringCompleted();
			}
			break;
			
		case kAudioPlayTrackChangedEvt:
			this->createStringOfSimilarArtists();
			break;
			
		default:
			char notificationString[64];
			VisualNotification::convertNotificationKeyToString(notificationKey, notificationString);
			char errLog[256];
			sprintf(errLog, "Unhandled VisualNotificationKey %s in file: %s (line: %d) [%s])", notificationString, __FILE__, __LINE__, __FUNCTION__);
			writeLog(errLog);
			break;
	}

}


void TemplateActor::createStringOfSimilarArtists() {

	if (this->similarArtistsStringCreationInProgress == true) {
		return;
	}
	
	this->similarArtistsStringCreationInProgress = true;

	this->similarArtistsStringVector.clear();
	this->theTemplateAction->removeImage();
	
	this->similarArtistsCurrentTrackIdentifier = VisualDataStore::getIdentifierOfCurrentTrack();
	
	VisualString* normalizedArtistNameForURLConstruction = VisualNetwork::createNormalizedURLString(VisualDataStore::getArtistOfCurrentTrack());
	if (!normalizedArtistNameForURLConstruction) {
		return;
	}
		
	VisualString similarArtistsURL;
	similarArtistsURL.initWithUTF8String("http://ws.audioscrobbler.com/1.0/artist/");
	similarArtistsURL = (similarArtistsURL + *normalizedArtistNameForURLConstruction);
	delete normalizedArtistNameForURLConstruction;
	similarArtistsURL = (similarArtistsURL + "/similar.txt");
	if (this->stringOfSimilarArtists != NULL) {
		delete this->stringOfSimilarArtists;
	}

	this->stringOfSimilarArtists = VisualString::createWithContentsOfURL(similarArtistsURL.getUtf8Representation(), this->similarArtistsStringRequestId);

	this->similarArtistsStringCreationInProgress = false;
}
