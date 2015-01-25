#include "PhaseRunner.h"

vector<string> &split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

/*
 * Phase runner
 *
 * @constructor PhaseRunner
 *
 * @param {String} phases "42:30,63:45,72:60"
 */
PhaseRunner::PhaseRunner() {
    _phases = new queue<Phase*>();
}

void PhaseRunner::reset() {
    _phases = new queue<Phase*>();
    
    _actualPhase = NULL;
}

void PhaseRunner::setSchedule(string phasesStr) {
    this->reset();
    
    // Parse phases
    vector<string> parsedPhases = split(phasesStr, ',');
    long phasesSize = parsedPhases.size();
    
    
    for (int i = 0; i < phasesSize; i++) {
        vector<string> parsedPhase = split(parsedPhases[i], ':');
        
        double duration = atof(parsedPhase[0].c_str()) * 60000.0;
        double temperature = atof(parsedPhase[1].c_str());
        
        Phase *phase = new Phase(temperature, duration);
        _phases->push(phase);
    }
    
    // Start first phase
    if(phasesSize > 0) {
        this->nextPhase();
    }
}

void PhaseRunner::nextPhase() {
    _actualPhase = _phases->front();
    _phases->pop();    
}

void PhaseRunner::onTimeElapsed(double diff) {
    if(!_actualPhase) {
        return;
    }
    
    while(diff > 0) {
        if(_actualPhase->getRemaining() >= diff) {
            _actualPhase->onTimeElapsed(diff);
            diff = 0;
        } else {
            double remaining = _actualPhase->getRemaining();
            diff -= remaining;
            
            // Finnish actual and get next
            _actualPhase->onTimeElapsed(remaining);
            this->nextPhase();
            
            if(!_actualPhase) {
                diff = 0;
            }
        }
    }
}

Phase* PhaseRunner::getActualPhase() {
    return _actualPhase;
}
