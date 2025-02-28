#include <Arduino.h>

class Button
{
    byte pin, prev_state;
    bool _clicked, _held, _dblclicked;
    unsigned long pressed_time, uct, clct;
    static const unsigned int jtime = 10;
    static const unsigned int htime = 100;
    static const unsigned int dbltime = 300;

public:
    void process_button()
    {
        byte val = digitalRead(pin);
        _clicked = false;
        _dblclicked = false;
        if (val == 0 && prev_state == 1)
        {
            uct = 0;
            unsigned long now = millis();
            if (pressed_time == 0)
                pressed_time = now;
            if (now > pressed_time + jtime)
            {
                prev_state = val;
                _clicked = true;
                if (clct != 0 && now < clct + dbltime)
                {
                    _dblclicked = true;
                }
                clct = pressed_time;
            }
        }
        else if (val == 1 && prev_state == 0)
        {
            pressed_time = 0;
            if (uct == 0)
                uct = millis();
            if (millis() > uct + jtime)
            {
                prev_state = val;
                _held = false;
                uct = millis();
            }
        }
        if (val == 0 && prev_state == 0 && millis() > clct + htime)
        {
            _held = true;
        }
        else
        {
            _held = false;
        }
    }
    Button(byte _pin)
    {
        pin = _pin;
        pinMode(pin, INPUT_PULLUP);
        prev_state = 1;
        _dblclicked = _held = _clicked = false;
        clct = uct = pressed_time = 0;
    }

    bool is_clicked()
    {
        return _clicked;
    }
    unsigned long held_time()
    {
        if (_held)
            return millis() - clct;
        return 0;
    }
    bool is_dblclicked()
    {
        return _dblclicked;
    }
};
