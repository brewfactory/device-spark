#include <string>
#include <sstream>
#include <vector>
#include <queue>

#include "Phase.h"

using namespace std;

class PhaseRunner
{
  private:
    queue<Phase*> *_phases;
    Phase *_actualPhase;
    void nextPhase();

  public:
    PhaseRunner();
    
    void setSchedule(string phasesStr);
    void reset();
    void onTimeElapsed(double diff);
    Phase *getActualPhase();
};
