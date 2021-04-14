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

#include <cinttypes>
#include <cstring>
#include <cerrno>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>

#include <internal/i2c.h>


using namespace GPIO;

class I2CError : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

enum {
    I2C_SLAVE                   = 0x0703,
    I2C_SMBUS                   = 0x0720,
    I2C_SMBUS_READ              = 1,
    I2C_SMBUS_WRITE             = 0,
    I2C_SMBUS_BLOCK_MAX         = 32,
    I2C_SMBUS_BYTE              = 1,
    I2C_SMBUS_BYTE_DATA         = 2,
    I2C_SMBUS_WORD_DATA         = 3,
};

union i2c_smbus_data {
    uint8_t byte;
    uint16_t word;
    uint8_t block[I2C_SMBUS_BLOCK_MAX + 2];
};

struct i2c_smbus_ioctl_data {
    uint8_t read_write;
    uint8_t command;
    uint32_t size;
    i2c_smbus_data *data;
};

static inline void i2c_smbus(int fd, char rw, uint8_t command, int size, union i2c_smbus_data *data)
{
    i2c_smbus_ioctl_data args;
    args.read_write = rw;
    args.command = command;
    args.size = size;
    args.data = data;
    if (::ioctl(fd, I2C_SMBUS, &args)) throw I2CError(std::string("i2c/smbus error: ") + std::strerror(errno));
}

int I2C::read() const
{
    i2c_smbus_data data;
    i2c_smbus(m_fd, I2C_SMBUS_READ, 0, I2C_SMBUS_BYTE, &data);
    return data.byte;
}

int I2C::regread8(int reg) const
{
    i2c_smbus_data data;
    i2c_smbus(m_fd, I2C_SMBUS_READ, reg, I2C_SMBUS_BYTE_DATA, &data);
    return data.byte;
}

int I2C::regread16(int reg) const
{
    i2c_smbus_data data;
    i2c_smbus(m_fd, I2C_SMBUS_READ, reg, I2C_SMBUS_WORD_DATA, &data);
    return data.word;
}

void I2C::write(int data) const
{
    i2c_smbus(m_fd, I2C_SMBUS_WRITE, data, I2C_SMBUS_BYTE, nullptr);
}

void I2C::regwrite8(int reg, int value) const
{
    i2c_smbus_data data;
    data.byte = value;
    i2c_smbus(m_fd, I2C_SMBUS_WRITE, reg, I2C_SMBUS_BYTE_DATA, &data);
}

void I2C::regwrite16(int reg, int value) const
{
    i2c_smbus_data data;
    data.word = value;
    i2c_smbus(m_fd, I2C_SMBUS_WRITE, reg, I2C_SMBUS_WORD_DATA, &data);
}

void I2C::open(const std::string& interface, unsigned int device_id)
{
    if ((m_fd = ::open(interface.c_str(), O_RDWR)) < 0) {}
    if (::ioctl(m_fd, I2C_SLAVE, device_id)) throw I2CError(interface + ": cannot open i2c/smbus: " + std::strerror(errno));
}

void I2C::close()
{
    if (m_fd >= 0) {
        ::close(m_fd);
        m_fd = -1;
    }
}

