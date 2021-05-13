---
layout: default
---

# Hardware/Software Debugging

## Introduction

This lab is a continuation of the previous [RTL Kernel Wizard Lab](rtl_kernel_lab.md) lab. You will use ChipScope to monitor signals at the kernel interface level and perform software debugging using Vitis.

## Objectives

After completing this lab, you will be able to:

* Add ChipScope cores to a design created using Vitis
* Use ChipScope to monitor signals at the kernel interface
* Debug a software application in Vitis

## Steps

### Open Vitis and import the project

To save time on compilation, a precompiled project will be provided with the Chipscope debug cores already included in the design.  See [Appendix](#steps-to-add-chipscope-debug-core-and-build-the-design) to learn how to add ChipScope debug cores

1. Continue with the same workspace `~/workspace`
1. Click **File > Import...**
1. In the *Import Projects* window, select **Vitis project exported zip file** and click **Next**
1. Click on **Browse...** and select **debug_lab.ide.zip** from `~/xup_compute_acceleration/solutions/` and click **OK**
     Make sure that the three hierarchical options are checked
1. Click **Finish**

#### Set active solution and permissions on executable

1. In the *Explorer* view, expand the hierarchy and double-click on **debug.prj**
1. Set **Hardware** as an *Active build configuration:*
1. In the *Explorer* view expand **debug > Hardware**
1. Right click on *debug* and select **Properties**

    ![](./images/debug_lab/select_executable.png)

1. Tick the box to add **Execute** to the *Owner* permissions, and click **Apply and Close**

    ![](./images/debug_lab/rtl_kernel_exe_properties.png)

1. If you do not see an option to set the permissions. Change permissions from a terminal:

    ```sh
    cd ~/workspace/debug/Hardware
    chmod +x debug
    ```

### Hardware Debugging

#### Build Hardware

Review the [Appendix](#steps-to-add-chipscope-debug-core-and-build-the-design) to understand how to add the ChipScope Debug bridge core and build the project. As this steps takes around two hours. A precompiled solution with the debug core is provided

#### Run the application

1. In *Assistant* view, right click on **debug_system > debug > Hardware** and select **Run > Run Configurations...**
1. Expand *OpenCL* and select *debug-Default*
1. In the *Arguments* tab, uncheck **Automatically add binary container(s) to arguments** and add *../binary_container_1.awsxclbin* as an argument

1. Click **Run**

    The host application will start executing, load the bitstream, and wait for user input (press any key to continue)

    ![](./images/debug_lab/run_first_part.png)

### Set up the Xilinx Virtual Cable (XVC)

The Xilinx Virtual Cable (XVC) is a virtual device that gives you JTAG debug capabilities over PCIe to the target device. XVC will be used to debug the design.


#### XVC for AWS
Open a new terminal window and run the following script which will manage setup of the XVC:

```sh
sudo fpga-start-virtual-jtag -P 10200 -S 0
```
The output should look like:

```
Starting Virtual JTAG XVC Server for FPGA slot id 0, listening to TCP port 10200.
Press CTRL-C to stop the service.
```

Skip the next section and continue with [Connecting Vivado to the XVC](#connecting-vivado-to-xvc)

#### XVC for Alveo U200

For an Alveo board, you need to determine the XVC device in your system. XVC is installed as part of the Vitis and XRT installation.

```sh
ls /dev/xfpga/xvc_pub*
```

This will report something similar to the output below:

```sh
/dev/xfpga/xvc_pub.u513
```

Each computer may have a different value for *xvc_pub.\** so you will need to check the name for your computer.

* In a terminal window, start a virtual jtag connection

    Run the following command (where *u513* should be the value your obtained from the previous command):

    ```sh
    debug_hw --xvc_pcie /dev/xfpga/xvc_pub.u513 --hw_server
    ```

    The Virtual JTAG XVC Server will start listening to TCP port **10200** in this case. This is the port you will need to [connect to from Vivado](#connecting-vivado-to-xvc). Note the *hw_server* is listening to TCP port 3121. See example output below

    ```sh
    launching xvc_pcie...
    xvc_pcie -d /dev/xfpga/xvc_pub.u513 -s TCP::10200
    launching hw_server...
    hw_server -sTCP::3121
    
    ****************************
    *** Press Ctrl-C to exit ***
    ****************************
    ```

### Connecting Vivado to XVC

1. Start Vivado from another terminal

    ```sh
    vivado
    ```

1. Click on **Open Hardware Manager >**
1. Click **Open Target > Auto Connect**

    ![](./images/debug_lab/hw_manager_open_target.png)

1. Right click on *localhost (0)* and select **Add Xilinx Virtual Cable (XVC)...**

   ![](./images/debug_lab/add_virtual_cable.png)

1. Enter **localhost** as the *host name*, and **10200** as the port (or the *port number* for your machine obtained previously) and click **OK**

   ![](./images/debug_lab/set_virtual_cable_port.png)

1. Right click on the *debug_bridge* and select **Refresh Device**.

    The Vivado Hardware Manager should update showing _Hardware_, _Waveform_, _Settings-hw_, _Trigger-Setup_ windows. The _Hardware_ window also shows the detected ILA cores (*hw_ila_\**), inserted in the design. The Alveo design will have one ILA, whereas, the AWS design will have two ILAs, one monitoring the AWS shell interface (*hw_ila_2*) and other the *KVAdd* kernel (*hw_ila_1*)

    ![](./images/debug_lab/hardware_manager.png)

1. Select the *debug_bridge* in the Hardware panel
1. In the *Hardware Device Properties* view, click on the browse button (...) beside **Probes file**
1. Browse to the project's **~/workspace/debug/Hardware** folder, select the **binary_container_1.ltx** file and click **OK**  
1. Select the *hw_ila_1* tab, and notice four (Slot_0 to Slot_3) probes are filled in the Waveform window
1. Click on the **Run Trigger immediate** button and observe the waveform window is fills with data showing that the four channels were _Inactive_ for the duration of the signal capture

    ![](./images/debug_lab/trigger_immediate.png)

1. Expand **slot_1 : KVAdd_1_m01_axi : Interface** , then find and expand  **slot_1 : KVAdd_1_m01_axi : W Channel** in the Waveform window
1. Select the **WVALID** signal and drag it to the Trigger Setup - hw window

   ![](./images/debug_lab/trigger_setup.png)

1. Click on drop-down button of the Value field and select trigger condition value as **1**

   ![](./images/debug_lab/trigger_value.png)

1. Click on the *Run trigger* button ![](./images/debug_lab/trigger_button.png) and observe the _hw_ila_1_ probe is waiting for the trigger condition to occur

   ![](./images/debug_lab/trigger_wait.png)

1. Switch to the Vitis GUI, and select *Console* view and press the **Enter key** to allow the program to continue executing

    Observe that the program completes displaying **INFO: Test completed successfully** in the Console window

1. Switch back to Vivado and notice that because the trigger condition was met, the waveform window has been populated with new captured data

   ![](./images/debug_lab/triggered_waveform.png)

1. Expand **Slot_0, slot_1,** and **slot_2** groups, zoom in to the region around samples *450 to 1000*, and observe the data transfers taking place on each channels. Also note the addresses from where data are read and where the results are written to

   ![](./images/debug_lab/captured_activities.png)

1. Zoom in on one of the transactions and hover your mouse at each successive sample and notice the data content changing
1. When you are finished, close Vivado Manager by selecting **File > Exit**
1. Close the jtag probe by switching to its terminal window and pressing *Ctrl-C*

### Perform Software Debugging

1. Switch to the Vitis GUI

1. In *Assistant* view, right click on **debug_system > debug > Hardware** and select **Debug > Debug Configurations...**

1. Make sure that the **Arguments** tab shows **../binary_container_1.awsxclbin**

1. Click **Apply** if needed, and then click **Debug**

1. Click **Yes** when prompted to switch to the *Debug perspective*

    The bitstream will be downloaded to the FPGA and the host application will start executing, halting at **main()** entry point

1. In *host_example.cpp* view scroll down to line ~274 and double-click on the left border to set a breakpoint  At this point, three buffers would have been created

    ![](./images/debug_lab/set_breakpoint.png)

1. Click on the **Resume** button or press **F8**

1. When prompted click in the console and press *Enter*

    The program will resume executing and stop when it reaches the breakpoint.
    At this point you can click on the various monitoring tabs (*Variables, Command Queue, Memory Buffers* etc.) and see the contents currently in scope.
    Vitis debug allows command queues and memory buffers to be examined as the program execution progresses

1. Click on the **Step Over** button or press **F6**

    The execution will progress one statement at a time

1. Continue pressing **F6** until you reach line ~332 at which point kernel will finish executing

1. Select the **Memory Buffers** tab

    Notice that three buffers are allocated, their IDs, DDR memory address, and sizes are shown. The *Mem* member may be different in your execution, but *Device Memory Address* member should be identical for each of the three buffers

    ![](./images/debug_lab/memory_buffers.png)

1. Select the **Command Queue** tab and notice that there no commands enqueued.

    ![](./images/debug_lab/command_queue.png)

    Lines ~340-344 creates commands to read the data and results

    ```C
    err |= clEnqueueReadBuffer( ... );
    ```

1. Press **F6** to execute the first `clEnqueueReadBuffer()` to create a read buffer command for reading operand *d_A*
Notice the Command Queue tab shows one command submitted

    ![](./images/debug_lab/command_queue_after_1st.png)

1. Press **F6** to execute the next `clEnqueueReadBuffer()` for *d_B*

    Notice the Command Queue tab shows two commands submitted

    ![](./images/debug_lab/command_queue_after_2nd.png)

1. Set a breakpoint at line ~400 (`clReleaseKernel()`) and press **F8** to resume the execution  

    Notice that the Command Queue tab still shows entries

1. Press **F6** to execute `clReleaseKernel()`

    Notice the Memory Buffers tab is empty as all memories are released

1. Click **F8** to complete the execution

1. To change perspective, click on *Design* in the top right corner of Vitis GUI

## Conclusion

In this lab, you used the ChipScope Debug bridge and cores to perform hardware debugging. You also performed software debugging using the Vitis GUI.

---------------------------------------

## Appendix-I

### Steps to Add ChipScope Debug core and build the design

1. Set target to *Active build configuration:* to **Hardware** in *Project Settings* view
1. In the *Assistant* view, expand **Hardware > binary_container_1 > KVadd**
1. Select **KVAdd**, right-click and select **Settings...**
1. In the **Hardware Function Settings** window, click **Refresh**, and then click on the *ChipScope Debug* option for the *KVAdd* kernel

    ![](./images/debug_lab/enable_chipscope.png)

1. Click **Apply and close**
1. In the *Explorer* view, expand **rtl_kernel > src > vitis_rtl_kernel > KVAdd** and double-click on the **host_example.cpp** to open it in the editor window
1. Around line 243 (after the *clCreateKernel* section) enter the following lines of code and save the file. This will pause the host software execution after creating kernel but before allocating buffer

    ```C
    printf("\nPress ENTER to continue after setting up ILA trigger...");
    getc(stdin);
    ```
    ![](./images/debug_lab/AddingCode.png)

1. Build the design

### Disable automatic rebuilding of the design

When you export a project, and re-import it, the file modified dates may change and cause Vitis to make the output executable and hardware kernel "out-of-date". This may cause the design to be automatically recompiled when an attempt is made to run the application from the GUI.  

* To disable automatic rebuilding, right click on the project folder, and select **C/C++ Build Settings**

* Select **C/C++ Build** and click on the **Behavior** tab

* Uncheck the following:
   * Build on resource save (Auto Build)
   * Build (Incremental build)
   * Clean

When you export a project, and re-import it, these settings stop the bitstream being automatically rebuilt.

![](./images/debug_lab/turn_off_autobuild.png)

If you need to rebuild the project, you can re-enable these settings. If you only need to update the host application, you can run the following command in a terminal in the project folder to rebuild the executable file only (where *rtl_kernel* is the name of the executable):

```sh
cd ~/workspace/rtl_kernel/Hardware
make rtl_kernel
```

-------------------------------------------------
<p align="center">Copyright&copy; 2020 Xilinx</p>