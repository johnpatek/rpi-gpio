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

#ifndef _RPI_GPIO_I2C_H_
#define _RPI_GPIO_I2C_H_

#include <string>

namespace GPIO {

class I2C {
public:
    I2C(const std::string& interface, unsigned int device_id)
    {
        open(interface, device_id);
    }
    I2C() {}
    ~I2C()
    {
        close();
    }

    void open(const std::string& interface, unsigned int device_id);
    void close();

    int read() const;
    int regread8(int reg) const;
    int regread16(int reg) const;

    void write(int data) const;
    void regwrite8(int reg, int value) const;
    void regwrite16(int reg, int value) const;

private:
    int m_fd = -1;
};

}

#endif /* i2c_hpp */
