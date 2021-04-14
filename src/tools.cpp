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

#include <errno.h>
#include <unistd.h>
#include <memory>

#include "tools.h"




using namespace GPIO::Tools;

void AutoFile::open(const std::string& name, int mode)
{
    close();
    if ((m_fd = ::open(name.c_str(), mode)) < 0) throw ToolsError(std::string("cannot open file: ") + std::strerror(errno));
}

bool AutoFile::open_nothrow(const std::string& name, int mode) noexcept
{
    if (m_fd >= 0) {
        if (::close(m_fd) < 0) return false;
        m_fd = -1;
    }
    if ((m_fd = ::open(name.c_str(), mode)) < 0) return false;
    return true;
}

ssize_t AutoFile::read(void *buf, size_t count) const
{
    ssize_t rb = ::read(m_fd, buf, count);
    if (rb < 0) throw ToolsError(std::string("cannot read file: ") + std::strerror(errno));
    if (static_cast<size_t>(rb) != count) throw ToolsError("cannot read file, not all data read");
    return rb;
}

void AutoFile::write(const void *buf, size_t count) const
{
    ssize_t wb = ::write(m_fd, buf, count);
    if (wb < 0) throw ToolsError(std::string("cannot write file: ") + std::strerror(errno));
    if (static_cast<size_t>(wb) != count) throw ToolsError("cannot write file, not all data written");
}

off_t AutoFile::seek(off_t offset, int whence)
{
    off_t offs = ::lseek(m_fd, offset, whence);
    if (offs < 0) throw ToolsError(std::string("cannot seek file: ") + std::strerror(errno));
    return offs;
}

void AutoFile::close()
{
    if (m_fd < 0) return;
    if (::close(m_fd) < 0) throw ToolsError(std::string("cannot close file: ") + std::strerror(errno));
    m_fd = -1;
}

AutoFile::~AutoFile()
{
    if (m_fd >= 0) ::close(m_fd);
}

