# Presentations

## Video Presentations

1. [Xilinx Platforms Introduction ![alt text](assets/images/youtube.png)](https://youtu.be/sb9qcgOcTDY)

1. [Intro to Vitis for Acceleration Platforms ![alt text](assets/images/youtube.png)](https://youtu.be/t6CvKG0NzcM)

1. [Vitis Tool Flow ![alt text](assets/images/youtube.png)](https://youtu.be/ObfOSltWWEM)

1. [Open CL Execution Model ![alt text](assets/images/youtube.png)](https://youtu.be/z1Yk6jctJeY)

1. [Vitis Design Analysis ![alt text](assets/images/youtube.png)](https://youtu.be/N941PGe9q_c)

1. [Vitis Design Methodology ![alt text](assets/images/youtube.png)](https://youtu.be/W-O66ASW_ls)

1. [Host Code Optimization ![alt text](assets/images/youtube.png)](https://youtu.be/6SqQ-tgMREg)

1. [Kernel Optimization ![alt text](assets/images/youtube.png)](https://youtu.be/BD7nXF0umpo)

1. [Vitis Accelerated Libraries ![alt text](assets/images/youtube.png)](https://youtu.be/bqttBaih_Ao)

1. [PYNQ for Compute Acceleration ![alt text](assets/images/youtube.png)](https://youtu.be/WgA_FgO_rAo)

    Short companion videos that cover topics that are not addressed on the main presentation. We recommend watching them while doing the PYNQ labs.

    * [Using Multiple Devices ![alt text](assets/images/youtube.png)](https://youtu.be/tk2XDW-Hpco)

    * [Hardware Emulation ![alt text](assets/images/youtube.png)](https://youtu.be/ylVEo0d83iM)

    * [Packaging Your Designs ![alt text](assets/images/youtube.png)](https://youtu.be/S2oSliWHpsA)


## Overview of the Vitis flow

![alt tag](./images/vitis_flow.png)


1. Vitis is the development environment used to create host applications and hardware accelerators. It includes host CPU and FPGA compilers as well as profiling and debugging tools
2. In Vitis, the host application can be written in C or C++ and uses the OpenCL API or the [XRT](https://github.com/Xilinx/XRT) (Xilinx Runtime Library) to interact with the accelerated hardware functions running on the FPGA. The accelerated hardware functions (also referred to as 'hardware kernels', or just 'kernels') can be written in C, C++, OpenCL or RTL
