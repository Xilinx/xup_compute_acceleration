/**********
Copyright (c) 2019-2020, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/

#ifndef LINE_EXCEPTION__
#define LINE_EXCEPTION__

#include <cerrno>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

class LineException : public std::runtime_error
{
    std::string message;
    int exception_errno;

public:
    LineException(const std::string &arg, int errno_in, const char *file, int line) : std::runtime_error(arg)
    {
        std::ostringstream output;
        const char *filename = NULL;
        filename             = strrchr(file, '/');

        output << "(";
        if (filename) {
            output << (filename + 1);
        }
        else {
            output << file;
        }
        output << ":" << line << ") " << arg;
        if (errno != 0) {
            output << std::endl
                   << "   (" << std::strerror(errno) << ")" << std::endl;
        }
        message         = output.str();
        exception_errno = errno_in;
    }
    ~LineException() throw() {}
    const char *what() const throw()
    {
        return message.c_str();
    }
    const int le_errno()
    {
        return exception_errno;
    }
};

#define throw_lineexception(arg) throw LineException(arg, 0, __FILE__, __LINE__);
#define throw_lineexception_errno(arg, errno_in) throw LineException(arg, errno_in, __FILE__, __LINE__);

#endif
