class ArlecRC210Switch
{
public:
    void begin(int rf433OutputPin);
    void setArlecSwitchState(int switchId, bool on, int timesToSend = 15);

private:
    int _rf433OutputPin;
};