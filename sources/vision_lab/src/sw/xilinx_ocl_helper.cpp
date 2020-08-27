/**********
Copyright (c) 2019, Xilinx, Inc.
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

#include "xilinx_ocl_helper.hpp"

#include <unistd.h>

namespace xilinx {
namespace example_utils {
std::vector<cl::Device> XilinxOclHelper::find_xilinx_devices()
{
    size_t i;
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    cl::Platform platform;
    for (i = 0; i < platforms.size(); i++) {
        platform                  = platforms[i];
        std::string platform_name = platform.getInfo<CL_PLATFORM_NAME>();
        if (platform_name == "Xilinx") {
            break;
        }
    }
    if (i == platforms.size()) {
        throw_lineexception("Unable to find Xilinx OpenCL devices");
    }

    // Get ACCELERATOR devices
    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices);
    return devices;
}

void XilinxOclHelper::initialize(std::string xclbin_file_name)
{
    // Find Xilinx OpenCL devices
    std::vector<cl::Device> devices = find_xilinx_devices();

    // Initialize our OpenCL context
    device  = devices[0];
    context = cl::Context(device);

    // Load the XCLBIN
    if (access(xclbin_file_name.c_str(), R_OK) != 0) {
        throw_lineexception("Specified XCLBIN not found");
    }

    std::ifstream xclbin(xclbin_file_name.c_str(), std::ifstream::binary);
    xclbin.seekg(0, xclbin.end);
    unsigned int nb = xclbin.tellg();
    xclbin.seekg(0, xclbin.beg);
    char *buf = new char[nb];
    xclbin.read(buf, nb);
    cl::Program::Binaries bins;
    bins.push_back({buf, nb});

    // TODO: Don't automatically assume that device 0 is the correct one
    devices.resize(1);

    // Program the device
    program = cl::Program(context, devices, bins);

    is_initialized = true;
}

cl::Kernel XilinxOclHelper::get_kernel(std::string kernel_name)
{
    if (!is_initialized) {
        throw_lineexception("Attempted to get kernel without initializing OCL");
    }

    cl::Kernel krnl(program, kernel_name.c_str());
    return krnl;
}

cl::CommandQueue XilinxOclHelper::get_command_queue(bool in_order, bool enable_profiling)
{
    if (!is_initialized) {
        throw_lineexception("Attempted to get command queue without initializing OCL");
    }

    cl_command_queue_properties props = 0;

    if (!in_order) {
        props |= CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE;
    }
    if (enable_profiling) {
        props |= CL_QUEUE_PROFILING_ENABLE;
    }

    cl::CommandQueue q(context, device, props);
    return q;
}

cl::Buffer XilinxOclHelper::create_buffer(size_t size, cl_mem_flags flags)
{
    if (!is_initialized) {
        throw_lineexception("Attempted to create buffer before initialization");
    }

    cl::Buffer buf(context, flags, size, NULL, NULL);
    return buf;
}

cl::Buffer XilinxOclHelper::create_buffer_in_bank(int bank, size_t size, cl_mem_flags flags)
{
    if (!is_initialized) {
        throw_lineexception("Attempted to create buffer before initialization");
    }

    cl_mem_ext_ptr_t bank_ext;
    bank_ext.flags = bank | XCL_MEM_TOPOLOGY;
    bank_ext.obj   = NULL;
    bank_ext.param = 0;

    cl::Buffer buf(context, flags | CL_MEM_EXT_PTR_XILINX, size, &bank_ext, NULL);
    return buf;
}

int XilinxOclHelper::get_fd_for_buffer(cl::Buffer buf)
{
    int fd;
    xclGetMemObjectFd(buf(), &fd);
    return fd;
}

cl::Buffer XilinxOclHelper::get_buffer_from_fd(int fd)
{
    cl::Buffer buffer;
    xclGetMemObjectFromFd(context(), device(), 0, fd, &buffer());
    return buffer;
}

const cl::Context &XilinxOclHelper::get_context()
{
    return context;
}

XilinxOclHelper::XilinxOclHelper()
{
}

XilinxOclHelper::~XilinxOclHelper()
{
}

} // namespace example_utils
} // namespace xilinx
