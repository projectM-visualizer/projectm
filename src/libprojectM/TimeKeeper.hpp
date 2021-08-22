#ifndef TimeKeeper_HPP
#define TimeKeeper_HPP

#ifndef WIN32
#include <sys/time.h>
#endif

#include "timer.h"

#define HARD_CUT_DELAY 3

class TimeKeeper
{

public:

  TimeKeeper(double presetDuration, double smoothDuration, double hardcutDuration, double easterEgg);

  void UpdateTimers();

  void StartPreset();
  void StartSmoothing();
  void EndSmoothing();
 
  bool CanHardCut();

  double SmoothRatio();
  bool IsSmoothing();

  double GetRunningTime(); 

  double PresetProgressA();
  double PresetProgressB();

  int PresetFrameA();
  int PresetFrameB();

  int PresetTimeA();
  int PresetTimeB();

  double sampledPresetDuration();

  void ChangeHardCutDuration(int seconds) { _hardCutDuration = seconds; }
  void ChangeHardCutDuration(double seconds) { _hardCutDuration = seconds; }
  void ChangeSoftCutDuration(int seconds) { _softCutDuration = seconds; }
  void ChangeSoftCutDuration(double seconds) { _softCutDuration = seconds; }
  void ChangePresetDuration(int seconds) { _presetDuration = seconds; }
  void ChangePresetDuration(double seconds) { _presetDuration = seconds; }
  void ChangeEasterEgg(float value) { _easterEgg = value; }

#ifndef WIN32
  /* The first ticks value of the application */
  struct timeval startTime;
#else  
  long startTime;
#endif /** !WIN32 */

private:

  double _easterEgg;
  double _presetDuration;
  double _presetDurationA;
  double _presetDurationB;
  double _softCutDuration;
  double _hardCutDuration;

  double _currentTime;
  double _presetTimeA;
  double _presetTimeB;
  int _presetFrameA;
  int _presetFrameB;

  bool _isSmoothing;
  

};
#endif
