#ifndef HTTPREMOTECONTROL
#define HTTPREMOTECONTROL

#include <QxtHttpServerConnector>
#include <QxtHttpSessionManager>
#include <QxtWebSlotService>
#include <QxtWebPageEvent>
#include "qprojectm.hpp"

class HTTPRemoteControl : public QxtWebSlotService {
    Q_OBJECT
    QProjectM * parent3;
public:
    HTTPRemoteControl(QxtAbstractWebSessionManager * sm, QProjectM * parent2): QxtWebSlotService(sm, 0) {
      parent3=parent2;
      printf("HTTPRemoteControl()\n"); fflush(stdout);
    }
public slots:
    void selectPrevious(QxtWebRequestEvent*);
    void selectNext(QxtWebRequestEvent*);
    void selectRandom(QxtWebRequestEvent*);
    void index(QxtWebRequestEvent*);
    void selectPresetPosition(QxtWebRequestEvent*, QString);
    void changePresetRating(QxtWebRequestEvent*, QString);
};

#endif
