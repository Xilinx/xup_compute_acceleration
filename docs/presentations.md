---
layout: default
---

# Presentations

## Vitis

| Title | YouTube Link | PDF Link |
| --- | --- | ---- |
| Xilinx Platforms Introduction | [![alt text](assets/images/youtube.png) Xilinx Platforms Introduction](https://youtu.be/sb9qcgOcTDY) | [![alt text](images/pdf.png)](https://www.xilinx.com/support/documentation/university/Vitis-Workshops/2019_2/xilinx_platforms_introduction.pdf) |
| Intro to Vitis for Acceleration Platforms | [![alt text](assets/images/youtube.png) Intro to Vitis for Acceleration Platforms](https://youtu.be/t6CvKG0NzcM) | [![alt text](images/pdf.png)](https://www.xilinx.com/support/documentation/university/Vitis-Workshops/2019_2/vitis_introduction.pdf) |
| Vitis Tool Flow | [![alt text](assets/images/youtube.png) Vitis Tool Flow](https://youtu.be/ObfOSltWWEM) | [![alt text](images/pdf.png)](https://www.xilinx.com/support/documentation/university/Vitis-Workshops/2019_2/vitis_toolflow.pdf) |
| Open CL Execution Model | [![alt text](assets/images/youtube.png) Open CL Execution Model](https://youtu.be/z1Yk6jctJeY) | [![alt text](images/pdf.png)](https://www.xilinx.com/support/documentation/university/Vitis-Workshops/2019_2/vitis_opencl_execution_model.pdf) |
| Vitis Design Analysis | [![alt text](assets/images/youtube.png) Vitis Design Analysis](https://youtu.be/N941PGe9q_c) | [![alt text](images/pdf.png)](https://www.xilinx.com/support/documentation/university/Vitis-Workshops/2019_2/vitis_design_analysis.pdf) |
| Vitis Design Methodology | [![alt text](assets/images/youtube.png) Vitis Design Methodology](https://youtu.be/W-O66ASW_ls) | [![alt text](images/pdf.png)](https://www.xilinx.com/support/documentation/university/Vitis-Workshops/2019_2/vitis_design_methodology.pdf) |
| Host Code Optimization | [![alt text](assets/images/youtube.png) Host Code Optimization](https://youtu.be/6SqQ-tgMREg) | [![alt text](images/pdf.png)](https://www.xilinx.com/support/documentation/university/Vitis-Workshops/2019_2/vitis_host_code_optimization.pdf) |
| Kernel Optimization | [![alt text](assets/images/youtube.png) Kernel Optimization](https://youtu.be/BD7nXF0umpo) | [![alt text](images/pdf.png)](https://www.xilinx.com/support/documentation/university/Vitis-Workshops/2019_2/vitis_kernel_optimizations.pdf) |
| Vitis Accelerated Libraries | [![alt text](assets/images/youtube.png) Vitis Accelerated Libraries](https://youtu.be/bqttBaih_Ao) | [![alt text](images/pdf.png)](https://www.xilinx.com/support/documentation/university/Vitis-Workshops/2019_2/vitis_accelerated_libraries.pdf) |
| Vitis hardware debug |   | [![alt text](images/pdf.png)](https://www.xilinx.com/support/documentation/university/Vitis-Workshops/2019_2/vitis_hardware_debug.pdf)|
| Vitis RTL kernels Accelerated Libraries  |   | [![alt text](images/pdf.png)](https://www.xilinx.com/support/documentation/university/Vitis-Workshops/2019_2/vitis_rtl_kernels.pdf) |

# PYNQ

PYNQ introduction, and short lab companion videos that cover topics that are not addressed in the main presentation. We recommend watching the lab while doing the PYNQ labs.

| Title | YouTube Link |
| --- | --- |
| PYNQ for Compute Acceleration | [![alt text](assets/images/youtube.png) PYNQ for Compute Acceleration](https://youtu.be/WgA_FgO_rAo) |
| Lab: Using Multiple Devices | [![alt text](assets/images/youtube.png) Lab: Using Multiple Devices](https://youtu.be/tk2XDW-Hpco)|
| Lab: Hardware Emulation | [![alt text](assets/images/youtube.png) Lab: Hardware Emulation](https://youtu.be/ylVEo0d83iM)|
| Lab: Packaging Your Designs | [![alt text](assets/images/youtube.png) Lab: Packaging Your Designs](https://youtu.be/S2oSliWHpsA)|

## Overview of the Vitis flow

![alt tag](./images/vitis_flow.png)


1. Vitis is the development environment used to create host applications and hardware accelerators. It includes host CPU and FPGA compilers as well as profiling and debugging tools
2. In Vitis, the host application can be written in C or C++ and uses the OpenCL API or the [XRT](https://github.com/Xilinx/XRT) (Xilinx Runtime Library) to interact with the accelerated hardware functions running on the FPGA. The accelerated hardware functions (also referred to as 'hardware kernels', or just 'kernels') can be written in C, C++, OpenCL or RTL

---------------------------------------
<p align="center">Copyright&copy; 2021 Xilinx</p>