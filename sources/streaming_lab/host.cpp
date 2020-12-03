/**********
Copyright (c) 2020, Xilinx, Inc.
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


/*******************************************************************************
Description:
    Kernel to kernel streaming example consisting of three compute
    units in a linear hardware pipeline.
    1) mm2s
        This Kernel reads the input vector from Global Memory and streams its
output.
    2) FIR
        This kernel implements a digital FIR filter with static coefficients 
output.
    3) s2mm
        This Kernel reads from its input stream and writes into Global Memory.
                     _____________
                    |             |<----- Global Memory
                    |     mm2s    |
                    |_____________|------+
                     _____________       | AXI4 Stream
                    |             |<-----+
                    |     fir     |
                    |_____________|----->+
                     ______________      | AXI4 Stream
                    |              |<----+
                    |     s2mm     |
                    |______________|-----> Global Memory
*******************************************************************************/

#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>
#include <iomanip>

// This extension file is required for stream APIs
#include "CL/cl_ext_xilinx.h"
// This file is required for OpenCL C++ wrapper APIs
#include "xcl2.hpp"

#define TAP_NUM 73

int coe[TAP_NUM] = {
  -137, -73, 139, 210, 384, 339, 263, 69, -15, 0, 177, 352, 424, 275,
  -20, -318, -420, -273, 21, 211, 86, -387, -991, -1380, -1294, -772,
  -175, -6, -582, -1761, -2918, -3223, -2086, 449, 3620, 6243, 7258, 
  6243, 3620, 449, -2086, -3223, -2918, -1761, -582, -6, -175, -772,
  -1294, -1380, -991, -387, 86, 211, 21, -273, -420, -318, -20, 275,
  424, 352, 177, 0, -15, 69, 263, 339, 384, 210, 139, -73, -137
};

typedef struct {
  int history[TAP_NUM];
  unsigned int last_index;
} fir_struct;

// Initialize FIR internal structure
void fir_init(fir_struct* f) {
  for(unsigned int i = 0; i < TAP_NUM; i++)
    f->history[i] = 0;
  f->last_index = 0;
}

// Insert element and compute FIR output
int fir_compute(fir_struct* f, int input) {
  // First insert the sample 
  f->history[f->last_index++] = input;
  if(f->last_index == TAP_NUM)
    f->last_index = 0;
  // Compute FIR output
  long long acc = 0;
  int index = f->last_index, i;
  for(i = 0; i < TAP_NUM; i++) {
    index = index != 0 ? index-1 : TAP_NUM-1;
    acc += (long long)f->history[index] * coe[i];
  };
  return acc >> 16;
}


////////////////////RESET FUNCTION//////////////////////////////////
void reset(int *samples, int *sw_results, int *hw_results,
          unsigned int num_samples) {
  // Fill the input vectors with random data
  for (size_t i = 0; i < num_samples; i++) {
    samples[i]    = std::rand() * ((std::rand() % 2) ? 1 : -1);
    hw_results[i] = 0;
    sw_results[i] = 0;
  }
}
///////////////////VERIFY FUNCTION///////////////////////////////////
bool verify(int *sw_results, int *hw_results, int size) {
  bool match = true;
  for (int i = 0; i < size; i++) {
    if (sw_results[i] != hw_results[i]) {
      match = false;
      break;
    }
  }
  std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
  return match;
}
////////MAIN FUNCTION//////////
int main(int argc, char **argv) {
  
  // Check input arguments
  if (argc < 2 || argc > 4) {
    std::cout << "Usage: " << argv[0] << " <XCLBIN File> <#samples(optional)> <debug(optional)>" << std::endl;
    return EXIT_FAILURE;
  }
  // Read FPGA binary file
  auto binaryFile = argv[1];
  unsigned int num_samples = 128;
  bool user_size = false;
  // Check if the user defined the # of samples
  if (argc >= 3){
    user_size = true;
    unsigned int val;
    try {
      val = std::stoi(argv[2]);
    }
    catch (const std::invalid_argument val) { 
      std::cerr << "Invalid argument in position 2 (" << argv[2] << ") program expects an integer as number of samples" << std::endl;
      return EXIT_FAILURE;
    }
    catch (const std::out_of_range val) { 
      std::cerr << "Number of samples out of range, try with a number lower than 2147483648" << std::endl;
      return EXIT_FAILURE;
    }
    if (val > 67108864){
      std::cout << "Warning: number of samples is not supported, set maximum value instead" << std::endl;
      val = 67108864;
    }
    num_samples = val;
    std::cout << "User size enabled" << std::endl;
  }
  bool debug = false;
  // Check if the user defined debug
  if (argc == 4){
    std::string debug_arg = argv[3];
    if(debug_arg.compare("debug") == 0)
      debug = true;
    std::cout << "Debug enabled" << std::endl;
  }
  
  if (!user_size){
    // Define number of num_samples  
    if (xcl::is_hw_emulation())
      num_samples= 2048;
    else if (xcl::is_emulation())
      num_samples= 128;
    else{
      num_samples= 2048 * 2048;
    }
  }

  // I/O Data Vectors
  std::vector<int, aligned_allocator<int>> samples(num_samples);
  std::vector<int, aligned_allocator<int>> hw_results(num_samples);
  std::vector<int, aligned_allocator<int>> sw_results(num_samples);

  // OpenCL Host Code Begins.
  // OpenCL objects
  cl::Device device;
  cl::Context context;
  cl::CommandQueue q;
  cl::Program program;
  cl::Kernel krnl_mm2s;
  cl::Kernel krnl_s2mm;
  cl::Kernel krnl_fir;
  cl_int err;

  // get_xil_devices() is a utility API which will find the Xilinx
  // platforms and will return list of devices connected to Xilinx platform
  auto devices = xcl::get_xil_devices();

  // read_binary_file() is a utility API which will load the binaryFile
  // and will return the pointer to file buffer.
  auto fileBuf = xcl::read_binary_file(binaryFile);
  cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
  bool valid_device = false;
  for (unsigned int i = 0; i < devices.size(); i++) {
    device = devices[i];
    // Creating Context and Command Queue for selected Device
    OCL_CHECK(err, context = cl::Context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(err,
              q = cl::CommandQueue(context, device,
                                   CL_QUEUE_PROFILING_ENABLE |
                                       CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE,
                                   &err));
    std::cout << "Trying to program device[" << i
              << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
    cl::Program program(context, {device}, bins, NULL, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
    } else {
      std::cout << "Device[" << i << "]: program successful!\n";
      // Creating Kernel
      OCL_CHECK(err, krnl_fir  = cl::Kernel(program, "krnl_fir" , &err));
      OCL_CHECK(err, krnl_mm2s = cl::Kernel(program, "krnl_mm2s", &err));
      OCL_CHECK(err, krnl_s2mm = cl::Kernel(program, "krnl_s2mm", &err));
      valid_device = true;
      break; // we break because we found a valid device
    }
  }
  if (!valid_device) {
    std::cout << "Failed to program any device found, exit!\n";
    exit(EXIT_FAILURE);
  }

  std::cout << "Running FIR filter with " << num_samples << " samples, each sample is a 32-bit signed element" << std::endl;

  // Reset the data vectors
  reset(samples.data(), sw_results.data(), hw_results.data(), num_samples);

  // Running the kernel
  unsigned int size_bytes  = num_samples * sizeof(int);

  // Allocate Buffer in Global Memory
  // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
  // Device-to-host communication
  OCL_CHECK(err, cl::Buffer buffer_input(context,
                                      CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                      size_bytes, samples.data(), &err));
                     
  OCL_CHECK(err, cl::Buffer buffer_output(context, 
                                      CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                      size_bytes, hw_results.data(), &err));

  // Setting Kernel Arguments mm2s
  OCL_CHECK(err, err = krnl_mm2s.setArg(0, buffer_input));
  OCL_CHECK(err, err = krnl_mm2s.setArg(2, num_samples));

  // Setting Kernel Arguments s2mm
  OCL_CHECK(err, err = krnl_s2mm.setArg(0, buffer_output));
  OCL_CHECK(err, err = krnl_s2mm.setArg(2, num_samples));

  // Copy input data to device global memory
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_input}, 0 /* 0 means from host*/));
  OCL_CHECK(err, err = q.finish());

  // Launching the Kernels
  std::cout << "Launching Hardware Kernels..." << std::endl;
  OCL_CHECK(err, err = q.enqueueTask(krnl_mm2s));
  OCL_CHECK(err, err = q.enqueueTask(krnl_s2mm));
  // wait for all kernels to finish their operations
  OCL_CHECK(err, err = q.finish());

  // Copy Result from Device Global Memory to Host Local Memory
  std::cout << "Getting Hardware Results..." << std::endl;
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_output},CL_MIGRATE_MEM_OBJECT_HOST));
  OCL_CHECK(err, err = q.finish());

  // OpenCL Host Code Ends

  // Compute software results
  std::cout << "Computing Software results..." << std::endl;
  fir_struct fir_fiter;
  fir_init(&fir_fiter);
  for (unsigned int i = 0 ; i < num_samples; i++){
    sw_results[i] = fir_compute(&fir_fiter, samples[i]);
  }

  // Compare the device results with software results
  bool match = verify(sw_results.data(), hw_results.data(), num_samples);

  if (debug){
    for (unsigned int i = 0 ; i < num_samples; i++){
      std::cout << "Sample [" << std::setw(8) << i << "]" <<"\tsw " << std::setw(14);
      std::cout << sw_results[i] << "\thw " << std::setw(14) << hw_results[i];
      std::cout << "\tequal "<< ((hw_results[i] == sw_results[i]) ? "True" : "False") << std::endl;
    }
  }

  return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}