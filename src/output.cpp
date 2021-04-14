/********************************************************************
 * Copyright © 2021 John R. Patek
 * 
 * This software is provided 'as-is', without any express or implied 
 * warranty. In no event will the authors be held liable for any 
 * damages arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any 
 * purpose, including commercial applications, and to alter it and 
 * redistribute it freely, subject to the following restrictions:
 * 
 *    1. The origin of this software must not be misrepresented; you 
 *       must not claim that you wrote the original software. If you 
 *       use this software in a product, an acknowledgment in the 
 *       product documentation would be appreciated but is not 
 *       required.
 *    
 *    2. Altered source versions must be plainly marked as such, and 
 *       must not be misrepresented as being the original software.
 *    
 *    3. This notice may not be removed or altered from any source 
 *       distribution.
 * 
 *******************************************************************/

#include <internal/output.h>

using namespace GPIO;


DigitalOut::DigitalOut(unsigned int pin)
: m_pin(pin)
{
    m_gpio.mode(m_pin, GPIO_MODE::GPIO_OUTPUT);
}

DigitalOut::~DigitalOut()
{
    m_gpio.mode(m_pin, GPIO_MODE::GPIO_INPUT);
}

void DigitalOut::on(std::chrono::nanoseconds nanos) const
{
    on();
    std::this_thread::sleep_for(nanos);
    off();
}

void DigitalOut::off(std::chrono::nanoseconds nanos) const
{
    off();
    std::this_thread::sleep_for(nanos);
    on();
}


ShiftOut::ShiftOut(unsigned int pin_data, unsigned int pin_clock, unsigned int pin_latch, DIRECTION direction)
: m_pin_data(pin_data)
, m_pin_clock(pin_clock)
, m_pin_latch(pin_latch)
, m_direction(direction)
{
    if (   m_pin_data  == m_pin_latch
        || m_pin_data  == m_pin_clock
        || m_pin_latch == m_pin_clock) {
        throw GPIOError("need 3 different GPIO pins for ShiftOut configuration");
    }
    m_gpio.mode(m_pin_data,  GPIO_MODE::GPIO_OUTPUT);
    m_gpio.mode(m_pin_clock, GPIO_MODE::GPIO_OUTPUT);
    m_gpio.mode(m_pin_latch, GPIO_MODE::GPIO_OUTPUT);
    m_gpio.clear(m_pin_data);
    m_gpio.clear(m_pin_clock);
    m_gpio.clear(m_pin_latch);
}

ShiftOut::~ShiftOut()
{
    m_gpio.mode(m_pin_data,  GPIO_MODE::GPIO_INPUT);
    m_gpio.mode(m_pin_clock, GPIO_MODE::GPIO_INPUT);
    m_gpio.mode(m_pin_latch, GPIO_MODE::GPIO_INPUT);
}

void ShiftOut::write1(bool value) const
{
    m_gpio.write(m_pin_data, value);
    m_gpio.write(m_pin_clock, true);
    std::this_thread::sleep_for(std::chrono::nanoseconds(10*1000));
    m_gpio.write(m_pin_clock, false);
    std::this_thread::sleep_for(std::chrono::nanoseconds(10*1000));
}


PWMOut::PWMOut(unsigned int pin, unsigned int range, unsigned int ratio)
: m_pin(pin)
{
    m_is_soft_pwm = !m_gpio.has_hard_pwm(m_pin);
    m_gpio.mode(m_pin, (m_is_soft_pwm) ? GPIO_MODE::SOFT_PWM_OUTPUT : GPIO_MODE::PWM_OUTPUT);
    set_range(range);
    set_ratio(ratio);
}

PWMOut::~PWMOut()
{
    m_gpio.mode(m_pin, GPIO_MODE::GPIO_INPUT);
}

void PWMOut::set_range(unsigned int range) const
{
    m_is_soft_pwm ? m_gpio.soft_pwm_range(m_pin, range) : m_gpio.pwm_range(m_pin, range);
}

void PWMOut::set_ratio(unsigned int ratio) const
{
    m_is_soft_pwm ? m_gpio.soft_pwm_write(m_pin, ratio) : m_gpio.pwm_write(m_pin, ratio);
}

void ToneOut::set_freq(unsigned int hz) const
{
    if (m_is_soft_tone) m_gpio.soft_tone_write(m_pin, hz);
    else m_gpio.pwm_tone_write(m_pin, hz);
}

ToneOut::ToneOut(unsigned int pin, unsigned int hz)
: m_pin(pin)
{
    m_is_soft_tone = !m_gpio.has_hard_pwm(m_pin);
    m_gpio.mode(m_pin, (m_is_soft_tone) ? GPIO_MODE::SOFT_TONE_OUTPUT : GPIO_MODE::PWM_TONE_OUTPUT);
    set_freq(hz);
}

ToneOut::~ToneOut()
{
    m_gpio.mode(m_pin, GPIO_MODE::GPIO_INPUT);
}


