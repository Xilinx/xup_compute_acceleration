---
layout: default
---

# Improving Performance Lab

## Introduction

In Introduction to Vitis [Part 1](Vitis_intro-1.md) and [Part 2](Vitis_intro-2.md), you learned how to create a project using GUI mode and went through entire design flow. At the end of the lab, you saw the limited transfer bandwidth due to 32-bit data operations. This bandwidth can be improved, and in turn system performance can be improved, by transferring wider data, and performing multiple operations in parallel.  This is one of the common optimization methods to improve the kernel's bandwidth.

## Objectives

After completing this lab, you will learn to:

- Create a project using Empty Application template in the Vitis GUI flow
- Import provided source files
- Run Hardware Emulation to see increased bandwidth
- Build the system and test it in hardware
- Perform profile and application timeline analysis in hardware emulation

## Steps

### Create a Vitis Project

1. Make sure Vitis environment and and AWS tools are set

    If you have opened a new terminal window then source AWS tools

1. Launch Vitis GUI

    Continue with the workspace you have used in previous lab  

1. Create a new acceleration project giving **wide_vadd** as the project name, and click **Next >**

    You should see `xilinx_aws-vu9p-f1_shell-v04261818_201920_2` as one of the platforms if you are continuing with previous lab, otherwise add it from `/home/centos/src/project_data/aws-fpga/Vitis/aws_platform/`

1. Select `Empty Application` as the template and click **Finish**

    The project is created

1. Import provided source files

    Right click on `src` directory in *Explorer* view and select `Import Sources...`

    ![](./images/improving_performance/add_sources.png)

1. Import all `*.cpp` and `*.hpp` files in `~/xup_compute_acceleration/sources/improving_performance_lab/` 
    These files include both host code and hardware accelerator

1. Within *Application Project Settings* view click on ![](./images/Fig-hw_button.png) in the *Hardware Functions* window and add **wide_vadd** function as a *Hardware Function* (kernel)

### Analyze the kernel code

DDR controllers have a 512-bit wide interface internally. If we parallelize the dataﬂow in the accelerator, we will be able to process 16 array elements per clock tick instead of one. Hence, we should be able to get an instant 16x computation speed-up by just vectorizing the input

1. Double-click on `wide_vadd_krnl.cpp` to view its content

    Look at lines 62-67 and note wider (512-bit) kernel interface. `uint512_dt` is used in stead of `unsigned int` for input, output and internal variables for data storage. Notice `uint512_dt` is defined as an arbitrary precision data type `ap_uint<512>` in line 46

    ```C
    void wide_vadd(
      const uint512_dt *in1, // Read-Only Vector 1
      const uint512_dt *in2, // Read-Only Vector 2
      uint512_dt       *out, // Output Result
      int size               // Size in integer
    )
    ```

1. Scroll down further and look at lines 78-80 where local memories are defined of the same data type and width (512-bit)

    ```C
    uint512_dt v1_local[BUFFER_SIZE]; // Local memory to store vector1
    uint512_dt v2_local[BUFFER_SIZE];
    uint512_dt result_local[BUFFER_SIZE]; // Local Memory to store result
    ```

### Setup Hardware Emulation

1. Set *Active build configuration:* to **Emulation-HW**

1. Notice the host code uses a 1,024 times smaller vector to run software and hardware emulations (`wide_vadd.cpp` line 60) to save emulation time

1. Set dedicated location of kernel and memory interface
  - Right click on `wide_vadd > Emulation-HW` in *Assistant* view, select `Settings`
  - Navigate to *wide_vadd* kernel, adjust memory and SLR settings according to the screenshot below
  - Click Apply and Close

	![](./images/improving_performance/kernel_slr_setting.png)

### Build and run in hardware emulation mode

1. Build in Emulation-HW mode

    This will take about 10 minutes

1. After build completes, right click on **wide_vadd** * Run as > Run Configurations...* window

1. Navigate to *Arguments* tab. Make sure *Automatically add binary container(s) to arguments* is checked

1. Click **Apply** and then **Run**  

    Notice the kernel wait time is about 12 seconds.

	![](./images/improving_performance/perf_single_bank.png)

1. Check generated kernel interface

    - Open Run Summary with Vitis Analyzer by double-clicking on `Emulation-HW > wide_vadd-Default > Run Summary (xclbin)` in the *Assistant* view
    - Select **System Diagram**. Notice that all ports (in1, in2, and out) are using one bank
    - Click **Kernels** tab
    - Check the `Port Data Width` parameter. All input and output ports are 512 bits wide whereas size (scalar) port is 32 bits wide

	![](./images/improving_performance/system_diagram.png)

    - Select **Platform Diagram** in the left panel

    Observe that there are four DDR4 memory banks and three PLRAM banks. In this design `bank1` is used for all operands, which is located in SLR2. Also notice that `bank0` and `bank2` are located in SLR1  

	![](./images/improving_performance/platform_diagram.png)

1. Click on **Application Timeline**

1. Zoom into area where data transfer on various ports of the kernel is taking place and observe the sequential data transfer between two input operands and a result since only one memory controller is being used

	![](./images/improving_performance/single_controller_timeline.png)

1. Close Vitis Analyzer

### Use multiple memory banks

There are four DDR4 memory banks available on the accelerator card. In the previous section, we used only one bank. As we have three operands (two read and one write) it may be possible to improve performance if more memory banks are used simultaneously, providing maximize the bandwidth available to each of the interfaces. So it is possible to use the topology shown in following Figure.

![](./images/improving_performance/multi_banks.png)

This will provide the ability to perform high-bandwidth transactions simultaneously with different
external memory banks. Remember, long bursts are generally better for performance than many small reads
and writes, but you cannot fundamentally perform two operations on the memory at the same time.

To connect a kernel to multiple memory banks, you need to: Assign the kernel's interface to a memory controller and Assign the kernel to an SLR region. 

Please note that since the DDR controllers are constrained to different SLR (Super Logic Region), the routing between two SLR may have some challenges in timing closure when the design is compiled for bitstream. This technique is valuable in the cases where one SLR has two DDR controllers.

1. Assign memory banks as shown in figure below

    - Right click `wide_vadd > Emulation-HW` in *Assistant* view, select `Settings`
    - Navigate to `wide_vadd` kernel, and adjust memory and SLR settings
    - Click Apply and Close

	![](./images/improving_performance/Using_multiple_banks.png)

1. Build Emulation-HW

    This will take about 10 minutes. After build completes, open Run Configurations window

1. Click **Run**  

    Notice that the kernel wait time has reduced from about 12 seconds (single memory bank) to 9 seconds (three memory banks) indicating performance improvement

	![](./images/improving_performance/perf_multi_bank.png)

1. Check generated kernel interface

    - Open Run Summary with Vitis Analyzer by double-clicking on `Emulation-HW > wide_vadd-Default > Run Summary (xclbin)` in the *Assistant* view
    - Select System Diagram
    - Click **Kernels** tab

    Notice all ports (in1, in2, and out) are using different memory banks

    ![](./images/improving_performance/multibank_system_diagram.png)

1. Click on **Application Timeline**

1. Zoom into area where data transfer on various ports of the kernel is taking place and observe that data fetching is taking place in parallel and result is written overlapping the fetching data, increasing the bandwidth

	![](./images/improving_performance/multibank_controller_timeline.png)

1. Close Vitis Analyzer

## Conclusion

From a simple vadd application, we explored several steps to increase system performance:
- Expand kernel interface width
- Assign dedicated memory controller
- Use Vitis Analyzer to view the result

---------------------------------------
Copyright&copy; 2020 Xilinx