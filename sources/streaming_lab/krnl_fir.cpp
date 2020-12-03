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

/*
 *     x(k)   --------------------------------------    ------------
 *              |             |             |                      |
 *              |             |             |                      |
 *       c(0)->(x)     c(1)->(x)     c(2)->(x)      ...   c(n-1)->(x)
 *              |             |             |                      |   
 *              |             |             |                      | 
 *     y(k) <--(+)<--|z-1|<--(+)<--|z-1|<--(+)<--       <--|z-1|<---
 *
 *
 *  prod        0             1             2                     n-1
 *  state              0             1                     n-2
 *
 *
 *
 *
 * Sampling frequency 44,100 Hz
 * 
 * Bands:
 * -------------------------------------------
 * |   from   |     to    | gain | ripple/att|
 * |----------|-----------|------|-----------|
 * | 0 Hz     |    500 Hz |    0 |    -40 dB |
 * | 1,500 Hz |  5,500 Hz |    1 |    0.9 dB |
 * | 6,500 Hz | 22,050 Hz |    0 |     -40dB |
 * -------------------------------------------
*/

#include "ap_axi_sdata.h"
#include "ap_int.h"
#include "hls_stream.h"

typedef ap_axis<32, 0, 0, 0> axis;

#define TAP_NUM 73

ap_int<16> coe[TAP_NUM] = {
  -137, -73, 139, 210, 384, 339, 263, 69, -15, 0, 177, 352, 424, 275,
  -20, -318, -420, -273, 21, 211, 86, -387, -991, -1380, -1294, -772,
  -175, -6, -582, -1761, -2918, -3223, -2086, 449, 3620, 6243, 7258, 
  6243, 3620, 449, -2086, -3223, -2918, -1761, -582, -6, -175, -772,
  -1294, -1380, -991, -387, 86, 211, 21, -273, -420, -318, -20, 275,
  424, 352, 177, 0, -15, 69, 263, 339, 384, 210, 139, -73, -137
};


/**
 * @brief      FIR filter. Implemented using the transposed form
 *             The coefficients are static.
 *             
 *             NOTE: This is not the most efficient way to implement a FIR filter
 *             on an Xilinx FPGA. However, it is the simples version that reduces
 *             the long critical path to a multiply accumulate.
 *             For efficient implementation review UG073 and PG149
 *
 * @param[in]  x   Sample (k)
 * @param[out] y   Output (k)
 */

extern "C" {
void krnl_fir(hls::stream<axis> &x, hls::stream<axis> &y) {
#pragma HLS PIPELINE II=1
#pragma HLS INTERFACE ap_ctrl_none port=return

  ap_int<48> prod[TAP_NUM];
#pragma HLS ARRAY_PARTITION variable=prod dim=1 complete
  static ap_int<48> state[TAP_NUM-1] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
#pragma HLS ARRAY_PARTITION variable=state dim=1 complete

#ifndef __SYNTHESIS__
    while(1)
#else
    if (!x.empty())
#endif
  {
    axis x_k = x.read();
    /*Multiply input x_k with coefficients*/
    for (unsigned int i = 0; i < TAP_NUM; i++){
      prod[i] = coe[i] * x_k.data;
    }

    /*Compute output*/
    ap_int<48> y_k = prod[0] + state[0];

    /*Compute addition and update the state. If the AXI4-Stream channel
      has tlast asserted reset the state*/
    if(!x_k.last){
      for (unsigned int i = 0; i < TAP_NUM-2; i++){
        state[i] = state[i+1] + prod[i+1];
      }
      state[TAP_NUM-2] = prod[TAP_NUM-1];
    }
    else {
      for (unsigned int i = 0; i < TAP_NUM-1; i++){
        state[i] = 0;
      }
    }

    /* generate output stream */
    axis out;
    out.data = y_k(47,16);
    out.last = x_k.last;
    y.write(out);
  }
}
}