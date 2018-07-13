#include <Arduino.h>
#include "ArlecRC210Switch.h"

void ArlecRC210Switch::begin(const int rf433OutputPin) {
    pinMode(rf433OutputPin, OUTPUT);
    _rf433OutputPin = rf433OutputPin;
}

void ArlecRC210Switch::setArlecSwitchState(const int switchId, const bool on, const int timesToSend) {
    if(switchId < 0 || switchId > 7) {
        return;
    }

    const int periodMicroseconds = 385;

    auto sendArlecCycle = [&](int highCycles, int lowCycles) {
        digitalWrite(_rf433OutputPin, HIGH);
        delayMicroseconds(highCycles * periodMicroseconds);
        digitalWrite(_rf433OutputPin, LOW);
        delayMicroseconds(lowCycles * periodMicroseconds);
    };

    auto sendArlecBit = [&](bool high) {
        sendArlecCycle(high ? 2 : 1, high ? 1 : 2);
    };

    auto sendArlecFrame = [&](const char * code) {
        for(int i = 0; i != 32; ++i) {
            sendArlecBit(code[i] == '1');
        }
        sendArlecCycle(1, 17);
    };

    auto writeBinary = [](int value, char* writeTo) {
        for(int i = 2; i >= 0; --i) {
            writeTo[i] = value & 1 ? '1' : '0';
            value >>= 1;
        }
    };

    // A = Fixed bit stream.
    // B = Switch ID.
    // C = on = 1, off = 0.
    // D = Always 1001.
    // E = Same as B.
    // F = Inverse of C.
    // G = 0.
    //             0         1         2         3
    //             012345678901234567890123456789012
    //             AAAAAAAAAAAAAAAAAAAABBBCDDDDEEEFG
    char code[] = "011101101101100000000000100100000";
    writeBinary(switchId, &code[20]);
    writeBinary(switchId, &code[28]);
    code[23] = on ? '1' : '0';
    code[31] = on ? '0' : '1';

    for(int i = 0; i != timesToSend; ++i) {
        sendArlecFrame(code);
    }
}