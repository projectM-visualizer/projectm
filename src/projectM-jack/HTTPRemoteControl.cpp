#include "HTTPRemoteControl.h"
#include <sstream>

#define HARDCUT true

void HTTPRemoteControl::selectPrevious(QxtWebRequestEvent* event) {
  postEvent(new QxtWebPageEvent(event->sessionID, event->requestID, "selectPrevious"));
  parent3->selectPrevious(HARDCUT);
}

void HTTPRemoteControl::selectNext(QxtWebRequestEvent* event) {
  postEvent(new QxtWebPageEvent(event->sessionID, event->requestID, "selectNext"));
  parent3->selectNext(HARDCUT);
}

void HTTPRemoteControl::selectRandom(QxtWebRequestEvent* event) {
  postEvent(new QxtWebPageEvent(event->sessionID, event->requestID, "selectRandom"));
  parent3->selectRandom(HARDCUT);
}

void HTTPRemoteControl::index(QxtWebRequestEvent* event) {
  QString s="";
  std::stringstream convert, convert2;
  s=s+"<html>";
  s=s+"<head><title>"+qgetenv("HTTP_NAME")+"</title><base target='datatransfer'></head>";
  s=s+"<body bgcolor='black' text='white' link='#9999FF' alink='#AAAAFF' vlink='#EE88EE'>";
  s=s+"<iframe name='datatransfer' style='display:none'></iframe>";
  s=s+"<a href='/selectPrevious'>&uarr;</a> <a href='/selectNext'>&darr;</a> <a href='/selectRandom'>Random</a> ";
  for(int i=1; i<=6; i++) {
    convert.str("");
    convert << i;
    s=s+"<a href='/changePresetRating/"+convert.str().c_str()+"'>"+convert.str().c_str()+"</a> ";
  }
  s=s+"<p>";
  for(int i=0; i<parent3->getPlaylistSize(); i++) {
    convert.str("");
    convert << i;
    std::string n=parent3->getPresetName(i);
    convert2.str("");
    convert2 << parent3->getPresetRating(i, FIRST_RATING_TYPE);
    s=s+"<a href='/selectPresetPosition/"+convert.str().c_str()+"'>"+n.substr(0, n.find(".milk")).c_str()+" ["+convert2.str().c_str()+"]</a><br>";
  }
  s=s+"</body>";
  s=s+"</html>";
  postEvent(new QxtWebPageEvent(event->sessionID, event->requestID, s.toUtf8()));
}

void HTTPRemoteControl::selectPresetPosition(QxtWebRequestEvent* event, QString a) {
  postEvent(new QxtWebPageEvent(event->sessionID, event->requestID, "selectPresetPosition('"+a.toUtf8()+"')"));
  parent3->selectPreset(a.toUInt(), HARDCUT);
}

void HTTPRemoteControl::changePresetRating(QxtWebRequestEvent* event, QString a) {
  postEvent(new QxtWebPageEvent(event->sessionID, event->requestID, "changePresetRating('"+a.toUtf8()+"')"));
  unsigned int index;
  parent3->selectedPresetIndex(index);
  parent3->changePresetRating(index, a.toUInt(), FIRST_RATING_TYPE);
  parent3->selectNext(HARDCUT);
}
