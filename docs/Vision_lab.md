---
layout: default
---

# Vision Application using Vitis Accelerated Libraries

This lab walks you through the steps to create a Vitis design with two kernel **image resize** and **image resize & blur** leveraging the [Vitis Accelerated Libraries](https://xilinx.github.io/Vitis_Libraries/).

## Objectives

After completing this lab, you will be able to:

* Use Vitis from command line
* Compile kernels to Xilinx objects (`*.xo`)
* Build FPGA binary file from `*.xo` files
* Compile host code to object files (`*.o`)
* Build executable host code, which includes linking `*.o` files with dynamic libraries

## Steps

### Clone Vitis Accelerated Libraries repository

Clone the open source Vitis Accelerated Libraries repository in your home directory

```sh
git clone https://github.com/Xilinx/Vitis_Libraries.git -b v2021.1_rel --depth 1 ~/Vitis_Libraries
```

### Build FPGA binary file

1. Navigate to the root directory of this example

   ```sh
   cd ~/xup_compute_acceleration/sources/vision_lab
   ```

1. Set environmental variables

	 It is crucial to define `XCL_EMULATION_MODE`. Let's start with `sw_emu`. We also need to define the target platform, this will be stored in the variable `PFM`, update this path appropriately. Finally, we are going to set up `VITIS_LIBS`. This variable will point to the `include` directory of vision library L1

    * Define EMULATION MODE it will also work as build target
    * Define AWS platform name
    * Define platform
    * Define path of Vitis accelerated libraries 

   ```sh
   export XCL_EMULATION_MODE=sw_emu
   export AWS_PFM=xilinx_aws-vu9p-f1_shell-v04261818_201920_2
   export PFM=~/aws-fpga/Vitis/aws_platform/$AWS_PFM/$AWS_PFM.xpfm
   export VITIS_LIBS=~/Vitis_Libraries/vision/L1/include
   ```

1. Create a temporary folder

   We will use `output` folder to store temporary files to avoid polluting the root directory

   ```sh
   mkdir output
   ```

1. Compile kernel and build FPGA binary

    In this step the Vitis compiler (`v++`) will be used three times:
    - Synthesize the resize kernel (`resize_accel_rgb`) to create a Xilinx object (`output/resize.xo`)
    - Synthesize the resize & blur kernel (`resize_blur_rgb`) to create a Xilinx object (`output/resize_blur.xo`)

   ```sh
   v++ -f $PFM -t $XCL_EMULATION_MODE --log_dir output -c -g --config src/vision_config.ini \
    -I$VITIS_LIBS -k resize_accel_rgb -o output/resize.xo src/hw/resize_rgb.cpp
   v++ -f $PFM -t $XCL_EMULATION_MODE --log_dir output -c -g --config src/vision_config.ini \
    -I$VITIS_LIBS -k resize_blur_rgb -o output/resize_blur.xo src/hw/resize_blur.cpp
   ```

    - Build the device binary (`vision_example.xclbin`) file

   ```sh
   v++ -f $PFM -t $XCL_EMULATION_MODE --log_dir output -l -g -j 7 --config src/connectivity_aws.ini \
    --config src/vision_config.ini -o vision_example.xclbin output/resize.xo output/resize_blur.xo
   ```

   `v++` is the Vitis compiler command. `v++` compiles and link FPGA binaries, here some of the switches are described. For more information run `v++ -h`

    - `-f` or `--platform`: specify a Platform
    - `-t` or `--target`: specify a compile target: `[sw_emu|hw_emu|hw]` if not defined default is `hw`
    - `-c` or `--compile`: run a compile mode, generate a `xo` file
    - `-l` or `--link`: run a link mode, generate a `*.xclbin` file
    - `-k` or `--kernel`: specify a kernel to compile or link. Only one `-k` option is allowed per `v++` command
    - `-g` or `--debug`: generate code for debugging
    - `-I` or `--include`: add the directory to the list of directories to be searched for header files. This option is passed to the openCL preprocessor
    - `-j` or `--jobs`: set the number of jobs
    - `-o` or `--output`: set output file name. Default: *a.xclbin* for link and build, *a.xo* for compile
    - `--log_dir`: specify a directory to copy internally generated log files to
    - `--config`: configuration file

	 Note: when compiling for hardware you need to define `__SDSVHLS__` macro, which is used to guard certain part of the code. In this example we need to define `HLS_NO_XIL_FPO_LIB` macro as well, this macro disables the use of bit-accurate, floating-point simulation models, instead using the faster (although not bit-accurate) implementation from your local system. Both macros are included in [vision_config.ini](sources/vision_lab/src/vision_config.ini) file. This macros can be also passed to `v++` as `-D__SDSVHLS__ -DHLS_NO_XIL_FPO_LIB`.

### Build host code

In order to build the host application we are going to use `g++`.

Install `opencv-devel` and `eog`, this is in case these packages are not already available.

```sh
sudo yum install opencv-devel eog -y
```

1. Create the object files `*.o` for every `*.cpp` file of the host code

   ```sh
   export CPP_FLAGS="-c -std=c++11 -D__USE_XOPEN2K8 -I$XILINX_XRT/include/"
   cd output/;g++ $CPP_FLAGS $(ls ../src/sw/*.cpp); cd ..
   ```

1. Create executable file linking object files with dynamic libraries

   ```sh
   g++ -std=c++11 -o vision_example $(ls output/*.o) `pkg-config --libs --cflags opencv` \
   -lxilinxopencl -L$XILINX_XRT/lib/ -L/usr/lib64/
   ```

### Execute the kernels (emulation only)

1. Set `LD_LIBRARY_PATH`

   Make sure you set `LD_LIBRARY_PATH` to include these two path, otherwise the execution will fail.
   ```sh
   export LD_LIBRARY_PATH=$XILINX_XRT/lib
   ```

1. If your are running either `sw_emu` or `hw_emu` an emulated platform needs to be created to simulate the hardware. Skip this step if you are running `hw`

   ```sh
   emconfigutil --platform $PFM --nd 1
   ```
   This will create a `emconfig.json` file which contains useful information for the simulation tools

1. Run the resize kernel **emulation only** using a small image to speed up emulation

    - Run resize kernel with a resize factor of 3

   ```sh
   ./vision_example vision_example.xclbin 0 src/data/fish_wallpaper_small.jpg 3
   ```

    You can run `./vision_example` to check the arguments or see the list below

    - First argument is the FPGA binary file
    - Second argument is the algorithm, `0` resize, `1` resize & blur
    - Third argument is the image
    - The last argument is the resize factor, you can used integers between 1 to 7

    When running `resize` the program will generate two output files: **resize_sw.png** (software execution of the algorithm) and **resize_hw.png** (kernel execution output).
    When running `resize & blur` the program will generate two output files **resize_blur_sw.png** and **resize_blur_hw.png**. View the output files by double-clicking on each of them in File Explorer under `sources/vision_lab/` directory. Compare that to the source input file, **fish_wallpaper.png** located at `sources/vision_lab/src/data`

### Run the kernels on Hardware

Since compilation for hardware target will take a long time, the FPGA binary is provided in the solution directory. Please follow [these steps](#build-full-hardware) if you want to generate the FPGA binary outside the workshop/webinar environment

1. Unset `XCL_EMULATION_MODE` environment variable by executing:

   ```sh
   unset XCL_EMULATION_MODE
   ```

1. The host code **does not** need to be recompiled because it **does not** depend on the emulation mode. `XRT` is going to link the executable with the actual hardware instead of the emulated platform

1. Copy precompiled device binary

   ```sh
   cp ~/xup_compute_acceleration/solutions/vision_lab/vision_example.awsxclbin .
   ```

1. Run the kernels on hardware 
    
    - Run resize kernel with a resize factor of 3

   ```sh
   ./vision_example vision_example.awsxclbin 0 src/data/fish_wallpaper.jpg 3
   ```

    - Run resize & blur kernel with a resize factor of 4 

   ```sh
   ./vision_example vision_example.awsxclbin 1 src/data/fish_wallpaper.jpg 4
   ```

    The host application `vision_example` will execute, programming the FPGA and running the host code. This will generate four output files: **resize\_sw.png**, **resize\_hw.png**, **resize_blur\_sw.png** and **resize\_blur\_hw.png** very quickly compared to `sw_emu` execution done before. Also `xclbin.run_summary` file will be generated, which can be analyzed using `vitis_analyzer`

1. Explore *Profile Summary* and *Application Timeline*

   ```sh
   vitis_analyzer xclbin.run_summary
   ```

1. Close Vitis Analyzer

### Clean Directory for a new Build

Before changing the target it is a good idea to remove temporary files

```sh
rm -rf _x/ *.log *.jou *.pb *xclbin* *.json *.png *.csv *.*summary*
```

## Vision Library Using PYNQ

Using the notebooks provided in the `sources/vision_lab/src/pynq` directory you can run:

- Hardware emulation of a L1 vision primitive using the `vision_emulation.ipynb` notebook
- On real hardware using the `vision.ipynb` notebook

## Conclusion

In this lab, you used Vitis from the command line (`v++`) to create an FPGA binary file with two kernels. You also used `g++` to compile the host application. You performed software emulation and analyzed the output images. You then run the provided solution in hardware and evaluate the output images.

---------------------------------------

## Appendix:

### Build Full Hardware

1. The previous compilation was for `sw_emu`. In order to build the FPGA binary file we need pass `hw` as target to `v++`. Execute the following command to set hardware build

   ```sh
   export XCL_EMULATION_MODE=hw
   ```

1. Compile kernels and FPGA binary by [repeating step 4 of Build FPGA binary file](#build-fpga-binary-file). The process takes about two hours to generate FPGA binary file called `vision_example.xclbin`.

1.	Note: to run in AWS you need to create an [AFI](Creating_AFI.md). Therefore, the FPGA binary file will be `vision_example.awsxclbin`. In a tutorial, this file will be provided

### Exploring the FPGA binary file

We can explore an FPGA binary file (`xclbin`) to obtain design information using `xclbinutil`, which reports xclbin content. For more information about `xclbinutil` run `xclbinutil -h`

Run the following command to save `vision_example.xclbin` content in `vision_example_xclbin.info`

```
xclbinutil --info --input vision_example.xclbin > vision_example_xclbin.info
```

With a text editor open `vision_example_xclbin.info` file, look for the kernel section.

Skip lines until you find Kernel information. Note that the `Signature` is the function prototype. There are only three ports because we used `bundle = control` for the scalar arguments. If you look at the `Instance` section, you will notice that both `image_in` and `image_out` are connected to the `bank 0 (DDR4)` as described in the [connectivity_aws.ini](sources/vision_lab/src/connectivity_aws.ini) file. There is an optimization opportunity in that regard. Observe how the scalar arguments are mapped to `S_AXI_CONTROL` and their offset. You can find a snapshot of `vision_example.xclbin` content below.

```
Kernel: resize_blur_rgb

Definition
----------
   Signature: resize_blur_rgb (ap_uint<512>* image_in, ap_uint<512>* image_out,
     int width_in, int height_in, int width_out, int height_out, float sigma)

Ports
-----
   Port:          M_AXI_IMAGE_IN_GMEM
   Mode:          master
   Range (bytes): 0xFFFFFFFF
   Data Width:    512 bits
   Port Type:     addressable

   Port:          M_AXI_IMAGE_OUT_GMEM
   Mode:          master
   Range (bytes): 0xFFFFFFFF
   Data Width:    512 bits
   Port Type:     addressable

   Port:          S_AXI_CONTROL
   Mode:          slave
   Range (bytes): 0x1000
   Data Width:    32 bits
   Port Type:     addressable

--------------------------
Instance:        resize_blur_rgb_1
   Base Address: 0x10000

   Argument:          image_in
   Register Offset:   0x10
   Port:              M_AXI_IMAGE_IN_GMEM
   Memory:            bank0 (MEM_DDR4)

   Argument:          image_out
   Register Offset:   0x1C
   Port:              M_AXI_IMAGE_OUT_GMEM
   Memory:            bank0 (MEM_DDR4)

   Argument:          width_in
   Register Offset:   0x28
   Port:              S_AXI_CONTROL
   Memory:            <not applicable>

   Argument:          height_in
   Register Offset:   0x30
   Port:              S_AXI_CONTROL
   Memory:            <not applicable>

   Argument:          width_out
   Register Offset:   0x38
   Port:              S_AXI_CONTROL
   Memory:            <not applicable>

   Argument:          height_out
   Register Offset:   0x40
   Port:              S_AXI_CONTROL
   Memory:            <not applicable>

   Argument:          sigma
   Register Offset:   0x48
   Port:              S_AXI_CONTROL
   Memory:            <not applicable>
```

---------------------------------------
<p align="center">Copyright&copy; 2021 Xilinx</p>