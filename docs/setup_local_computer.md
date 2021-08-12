---
layout: default
---

# Setup Vitis on your own computer

To run (or build) these labs on your own computer, install Vitis. For non-commercial/academic use, Vitis licenses are free.

[Download Vitis](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vitis.html) and install the tools. Make sure you install the version targeted in this tutorial.

[Download XRT and the U200 package](https://www.xilinx.com/products/boards-and-kits/alveo/u200.html#gettingStarted) for your computer, and install both packages.

## Setup the tools

Add the following to your environment setup.

```sh
source /opt/xilinx/xrt/setup.(c)sh
source $XILINX_VITIS/settings64.(c)sh
export PLATFORM_REPO_PATHS=$ALVEO_PLATFORM_INSTALLATION_DIRECTORY
```

---------------------------------------
<p align="center">Copyright&copy; 2021 Xilinx</p>