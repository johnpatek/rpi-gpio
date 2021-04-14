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

#ifndef tools_hpp_POIJSLGAUDKGBKDSHJHSFJGFHKSDJHKVLKDSVHDSVKJHDSV
#define tools_hpp_POIJSLGAUDKGBKDSHJHSFJGFHKSDJHKVLKDSVHDSVKJHDSV

#include <string>
#include <cstring>
#include <stdexcept>
#include <cinttypes>
#include <fcntl.h>


namespace GPIO {
namespace Tools {

    class ToolsError : public std::runtime_error {
        using runtime_error::runtime_error;
    };

    /// Byte swap any scalar type. Gives compiler error if type is not scalar.

    template<class VALUE>
    void byteswap(VALUE& value)
    {
        static_assert(std::is_scalar<VALUE>::value, "need scalar type");
        unsigned int len = sizeof(VALUE);
        uint8_t* p = (uint8_t*)&value;
        for (unsigned int i = 0, e = len-1; i < len/2; ++i, --e) {
            std::swap(p[i], p[e]);
        }
    }

    /// Open a file using posix file access. Guaranteed to close the file on
    /// end of scope, including exceptions.

    class AutoFile {
    public:
        AutoFile() {}
        AutoFile(const std::string name, int mode = O_RDWR) { open(name, mode); }
        ~AutoFile();

        void open(const std::string& name, int mode = O_RDWR);
        bool open_nothrow(const std::string& name, int mode = O_RDWR) noexcept;
        ssize_t read(void *buf, size_t count) const;
        template <class VALUE>
        void read(VALUE& value) const
        {
            static_assert(std::is_scalar<VALUE>::value, "need scalar type");
            read(&value, sizeof(VALUE));
        }
        void write(const void *buf, size_t count) const;
        void write(const std::string& buf) const { write(buf.data(), buf.size()); }
        void write(const char* p) const { write(p, std::strlen(p)); }
        template <class VALUE>
        void write(const VALUE& value) const
        {
            static_assert(std::is_scalar<VALUE>::value, "need scalar type");
            write(&value, sizeof(VALUE));
        }
        off_t seek(off_t offset, int whence);
        void close();
        int release() { int fd = m_fd; m_fd = -1; return fd; }

        int get() const { return m_fd; }
        int operator*() const { return get(); }

    private:
        int m_fd = -1;
    };

    template<typename Ch>
    uint32_t codepoint_cast(Ch sch)
    {
        // All this code gets completely eliminated during
        // compilation. All it does is to make sure we can
        // expand any char type to a uint32_t without signed
        // bit expansion, treating all char types as unsigned.

        if (sizeof(Ch) == 1) {
            return static_cast<uint8_t>(sch);
        }
        else if (sizeof(Ch) == 2) {
            return static_cast<uint16_t>(sch);
        }
        else {
            return static_cast<uint32_t>(sch);
        }
    }

    /// Conversion from UTF-8 to UTF-32
    
    template<typename N, typename W>
    bool from_utf8(const std::basic_string<N>& narrow, std::basic_string<W>& wide)
    {
        uint16_t remaining { 0 };
        uint32_t codepoint { 0 };
        uint32_t lower_limit { 0 };

        for (const auto sch : narrow) {

            uint32_t ch = codepoint_cast(sch);

            if (sizeof(N) > 1 && ch > 0x0ff) {
                return false;
            }

            switch (remaining) {

                default:
                case 0: {
                    codepoint = 0;
                    if (ch < 128) {
                        wide += static_cast<W>(ch);
                        break;
                    }
                    else if ((ch & 0x0e0) == 0x0c0) {
                        remaining = 1;
                        lower_limit = 0x080;
                        codepoint = ch & 0x01f;
                    }
                    else if ((ch & 0x0f0) == 0x0e0) {
                        remaining = 2;
                        lower_limit = 0x0800;
                        codepoint = ch & 0x0f;
                    }
                    else if ((ch & 0x0f8) == 0x0f0) {
                        remaining = 3;
                        lower_limit = 0x010000;
                        codepoint = ch & 0x07;
                    }
                    else {
                        return false;
                    }
                    break;
                }

                case 5:
                case 4:
                case 3:
                case 2:
                case 1: {
                    if ((ch & 0x0c0) != 0x080) {
                        return false;
                    }
                    codepoint <<= 6;
                    codepoint |= (ch & 0x03f);
                    --remaining;
                    if (!remaining) {
                        if (codepoint < lower_limit) {
                            return false;
                        }

                        // take care - this code is written for platforms with 32 bit wide chars -
                        // if you compile this code on Windows, or want to explicitly support 16
                        // bit wide chars on other platforms, you need to supply surrogate pair
                        // replacements for characters > 0x0ffffffff at exactly this point before
                        // adding a new codepoint to 'wide'

                        wide += W(codepoint);
                    }
                    break;
                }
                    
            }
            
        }
        
        return true;
    }

    
}
}

#endif /* tools_hpp */
