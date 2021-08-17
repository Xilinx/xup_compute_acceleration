<!-- 
DO NOT add layout, it will prevent README.md to be render as index.html by GitHub pages
-->

# Xilinx University Program Vitis Tutorial

## Introduction

Welcome to the XUP Vitis-based Compute Acceleration tutorial. These labs will provide hands-on experience using the [Vitis unified software platform](https://www.xilinx.com/products/design-tools/vitis.html) with Xilinx FPGA hardware. You will learn how to develop applications using the Vitis development environment that supports OpenCL/C/C++ and RTL kernels.

The tutorial instructions target the following hardware and software:

* Vitis 2021.1
* AWS EC2 F1 f1.2xlarge (cloud)

This tutorial shows you how to use Vitis with AWS EC2 F1. Sources and precompiled solutions are provided for AWS EC2 F1 x2.large. You may be able to use the Vitis tutorial instructions with other cloud providers, and other hardware.


## Run Tutorial

You can run this tutorial in different ways.

1. If you have an Alveo board, you can run all parts of the tutorial on a local machine.

1. You can use the Vitis software in the cloud, with hardware in the cloud (AWS F1).
  * For running your design in AWS you will need to [create an AFI](Creating_AFI.md)

1. You can use the Vitis software on a local machine for building designs, and only switch to the cloud to deploy in hardware, make sure you build for the correct shell.

Once you have decided how you want to run the tutorial, follow the appropriate instructions below.

### XUP AWS Tutorial

If you are attending an instructor-led XUP AWS tutorial, preconfigured AWS F1 instances will be provided for you. Use the following instructions to [connect to your assigned AWS XUP tutorial instance](./setup_xup_aws_workshop.md)

### AWS EC2 F1

An [FPGA Developer AMI](https://aws.amazon.com/marketplace/pp/B06VVYBLZZ) (Amazon Machine Image) is available with the Xilinx Vitis software preinstalled. This can be used to target AWS EC2 F1 hardware. An AMI is like a Virtual Machine image. You can use this AMI and the following instructions to [set up and connect to an AWS instance](./setup_aws.md)

You can also install [Vitis unified software platform](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vitis.html) on your local machine, build design offline, and use AWS F1 hardware for testing. See the Amazon guide to use [AWS EC2 FPGA Development Kit](https://github.com/aws/aws-fpga) for details on setting up your machine.

### Local computer

To use your own computer, [install and set up Vitis and install the Alveo U200 packages](./setup_local_computer.md)

### Clone repository 

You also need to clone this repository to get a copy of the source code, the lab steps consider that this repository is cloned directly in the home directory (\~).

```sh
git clone git@github.com:Xilinx/xup_compute_acceleration.git ~
```

## Tutorial overview

The complete set of labs includes the following modules; it is recommended to complete each lab before proceeding to the next

1. [**Introduction to Vitis Part 1**](Vitis_intro-1.md):
	This lab shows you how to use the Vitis GUI to create a new project using a simple vector addition example. You will run CPU emulation (`sw_emu`) to verify functional correctness of the example design. 

1. [**Introduction to Vitis Part 2**](Vitis_intro-2.md):
	In this lab you will continue with the previous example and run hardware emulation (`hw_emu`) to verify the functionality of the generated hardware design and profile the whole application. You will then use *AWS F1* or *on-premise* hardware to validate the design using a pre-generated host application and FPGA binary.

1. [**Improving Performance**](Improving_Performance_lab.md):
	This lab shows how bandwidth can be improved, and thus system performance, by using wider data path and transferring data in parallel using multiple memory banks.

1. [**Optimization**](Optimization_lab.md):
	This lab guides you through the steps to analyze various generated reports and then apply optimization techniques, such as `DATAFLOW` on the host program and `PIPELINING` on kernel side to improve throughput and data transfer rate.

1. [**Vision Lab**](Vision_lab.md):
    In this lab you will create a Vitis design using the command line. The design uses two kernels from the [Vitis Accelerated Libraries](https://xilinx.github.io/Vitis_Libraries/), **image resize** and **image resize & blur**. You will run software emulation and test the kernels in hardware.

1. [**PYNQ Labs**](pynq_labs.md):
	In this series of labs you will learn how to use PYNQ for easier user of Xilinx compute acceleration platforms.

### Advanced labs

These labs are intended for hardware designers who may want to use RTL to build kernels, and learn how to use lower level hardware debug features in Vitis.

1. [**RTL-Kernel**](rtl_kernel_lab.md):
	This lab guides you through the steps involved in using a RTL Kernel wizard to wrap a user RTL-based IP so the generated IP can be used in a Vitis project and application development.

1. [**Hardware Debugging**](debug_lab.md):
	This lab will show you how to carry out host application debug, and debug of the hardware kernel.

1. [**Streaming**](streaming_lab.md):
	This lab will show you how to incorporate kernels having streaming interfaces.

---------------------------------------
<p align="center">Copyright&copy; 2021 Xilinx</p>