#include "Phase.h"

Phase::Phase(double temperature, double duration) {
  _temperature = temperature;
  _duration = duration;
  _remaining = _duration;
  _paused = false;
  _finished = false;
}

bool Phase::isPaused() {
  return _paused;
}

bool Phase::isFinished() {
  return _finished;
}

double Phase::getRemaining() {
  return _remaining;
}

double Phase::getTemperature() {
  return _temperature;
}

void Phase::onTimeElapsed(double diff) {

  if(_paused) {
    return;
  }

  _remaining -= diff;

  if(_remaining <= 0) {
    _remaining = 0;
    _finished = true;
  }
}
