#ifndef WIN32
#include <sys/time.h>
#else
#endif /** !WIN32 */
#include <stdlib.h>

#include "TimeKeeper.hpp"
#include "RandomNumberGenerators.hpp"

TimeKeeper::TimeKeeper(double presetDuration, double smoothDuration, double hardcutDuration, double easterEgg)
  {
    _softCutDuration = smoothDuration;
    _presetDuration = presetDuration;
    _hardCutDuration = hardcutDuration;
    _easterEgg = easterEgg;

#ifndef WIN32
	projectm_gettimeofday ( &this->startTime, NULL );
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
    _presetDurationA = sampledPresetDuration();
  }
  void TimeKeeper::StartSmoothing()
  {
    _isSmoothing = true;
    _presetTimeB = _currentTime;
    _presetFrameB = 1;
    _presetDurationB = sampledPresetDuration();
  }
  void TimeKeeper::EndSmoothing()
  {
    _isSmoothing = false;
    _presetTimeA = _presetTimeB;
    _presetFrameA = _presetFrameB;
    _presetDurationA = _presetDurationB;
  }

  bool TimeKeeper::CanHardCut()
  {
    return ((_currentTime - _presetTimeA) > _hardCutDuration);
  }

  double TimeKeeper::SmoothRatio()
  {
    return (_currentTime - _presetTimeB) / _softCutDuration;
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
    else return (_currentTime - _presetTimeA) / _presetDurationA;
  }
  double TimeKeeper::PresetProgressB()
  {
    return (_currentTime - _presetTimeB) / _presetDurationB;
  }

int TimeKeeper::PresetFrameB()
  {
    return _presetFrameB;
  }

int TimeKeeper::PresetFrameA()
  {
    return _presetFrameA;
  }

int TimeKeeper::PresetTimeB()
  {
    return _presetTimeB;
  }

int TimeKeeper::PresetTimeA()
  {
    return _presetTimeA;
  }

double TimeKeeper::sampledPresetDuration() {
#ifdef WIN32
	return  _presetDuration;
#else
		return fmax(1, fmin(60, RandomNumberGenerators::gaussian
			(_presetDuration, _easterEgg)));
#endif
}
