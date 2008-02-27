#ifndef WIN32
#include <sys/time.h>
#else
#endif /** !WIN32 */
#include <stdlib.h>
#include "TimeKeeper.hpp"



TimeKeeper::TimeKeeper(double presetDuration, double smoothDuration)
  {
    _presetDuration = presetDuration;
    _smoothDuration = smoothDuration;

#ifndef WIN32
	gettimeofday ( &this->startTime, NULL );
#else
	startTime = GetTickCount();
#endif /** !WIN32 */

	UpdateTimers();
  }

  void TimeKeeper::UpdateTimers()
  {
#ifndef WIN32
	_currentTime = getTicks ( &startTime ) * 0.001;
#else
	_currentTime = getTicks ( startTime ) * 0.001;
#endif /** !WIN32 */

	_presetFrameA++;
	_presetFrameB++;

  }

  void TimeKeeper::StartPreset()
  {
    _isSmoothing = false;
    _presetTimeA = _currentTime;
    _presetFrameA = 1;
  }
  void TimeKeeper::StartSmoothing()
  {
    _isSmoothing = true;
    _presetTimeB = _currentTime;
    _presetFrameB = 1;
  }
  void TimeKeeper::EndSmoothing()
  {
    _isSmoothing = false;
    _presetTimeA = _presetTimeB;
    _presetFrameA = _presetFrameB;
  }
 
  bool TimeKeeper::CanHardCut()
  {
    return ((_currentTime - _presetTimeA) > HARD_CUT_DELAY);      
  }

  double TimeKeeper::SmoothRatio()
  {
    return (_currentTime - _presetTimeB) / _smoothDuration;
  }
  bool TimeKeeper::IsSmoothing()
  {
    return _isSmoothing;
  }

  double TimeKeeper::GetRunningTime()
  {
    return _currentTime;
  } 

  double TimeKeeper::PresetProgressA()
  {
    if (_isSmoothing) return 1.0;
    else return (_currentTime - _presetTimeA) / _presetDuration;
  }
  double TimeKeeper::PresetProgressB()
  {
    return (_currentTime - _presetTimeB) / _presetDuration;
  }

int TimeKeeper::PresetFrameB()
  {
    return _presetFrameB;
  }

int TimeKeeper::PresetFrameA()
  {
    return _presetFrameA;
  }
