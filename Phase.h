class Phase
{
  private:
    double _temperature;
    double _duration;
    double _remaining;
    bool _paused;
    bool _finished;

  public:
    Phase(double temperature, double duration);
    bool isPaused();
    bool isFinished();
    double getRemaining();
    double getTemperature();
    void onTimeElapsed(double diff);
};
