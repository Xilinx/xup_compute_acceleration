---
layout: default
---

# Connecting to AWS

To get started with AWS, you will need an Amazon account. You will also need AWS credit to run the tutorial. If you are a professor or a student, you may be eligible to free credit by registering with [AWS educate](https://aws.amazon.com/education/awseducate/).

## Set up an AWS instance

Use [this guide](https://docs.aws.amazon.com/efs/latest/ug/gs-step-one-create-ec2-resources.html) to setup and AWS instance. Make sure to use the [FPGA Developer AMI version 1.9.0](https://aws.amazon.com/marketplace/pp/B06VVYBLZZ/ref=portal_asin_url) which includes Xilinx Vitis 2020.1 tools that this tutorial is based on.

### Login into the AWS and starting an F1 instance

1. Once you have an account, log in to the EC2 AWS Console:

    https://console.aws.amazon.com/ec2

    This should bring you to the EC2 dashboard (Elastic Compute).

    In the EC2 dashboard, select Launch Instance. From here you should be able to start your instance.

## Additional setup

You may want to do some additional setup to allow you to VNC to your instance. You can also follow the instructions in [Setup XUP AWS Workshop](setup_xup_aws_workshop.md) to connect to your instance.

### VNC server setup

When setting up an instance for the first time, you need to install vncserver software.

#### Install VNC server
In a terminal, execute the following commands

```sh
sudo yum install -y tigervnc-server
sudo yum groupinstall -y "Server with GUI"
```

When launching vncserver, you will be prompted to set up a password that you will need later.

### Start vncserver

Each time you start an instance, you will need to start vncserver

```sh
vncserver -geometry 1920x1080
```

1. You can choose your preferred geometry (screensize)

1. You should see a status message in the terminal once *vncserver* has started.

1. Take note of the number after the “:”

1. In this case, 1. This is the port the VNC viewer will connect to on the VNC server and needs to be specified as a two digit number below: 01.

1. Connect to AWS instance from VNC viewer.

1. From VNC viewer, specify the IP address of your AWS instance, followed by the VNC port number (as identified above), in this case :1

1. When prompted, enter the VNC server password set up earlier.

1. You should then be connected to the AWS instance.


### Verify XRT and Vitis tools

Open a terminal and verify that Xilinx Vitis tools have been preinstalled and are on the path:

```sh
which vitis
```

Note that the XRT tools are installed (/opt/xilinx/xrt) but are not included on the path by default.

```sh
sudo chmod 777 /opt/xilinx/xrt/setup.sh
#Source XRT everytime a new terminal is open
echo "source /opt/xilinx/xrt/setup.sh" >> ~/.bashrc
#Reload .bashrc in the current terminal
source ~/.bashrc
```

### Clone AWS-FPGA repository and set variables

1. Open a terminal

1. If you are using the Xilinx provided instances then execute the following to clone the *aws-fpga* repository and setup the Xilinx tools. `aws-fpga` includes the AWS F1 tools, Hardware Development Kit (HDK) and documentation

   ```sh
   cd ~
   git clone https://github.com/aws/aws-fpga -b v1.4.21
   echo "export PLATFORM_REPO_PATHS=~/aws-fpga/Vitis/aws_platform/xilinx_aws-vu9p-f1_shell-v04261818_201920_2/" >> ~/.bashrc
   echo "source /opt/xilinx/xrt/setup.sh" >> ~/.bashrc
   echo "source $XILINX_VITIS/settings64.sh" >> ~/.bashrc
   git clone https://github.com/Xilinx/xup_compute_acceleration.git
   source ~/.bashrc
   source ~/aws-fpga/vitis_setup.sh
   source ~/aws-fpga/vitis_runtime_setup.sh
   ```

   The previous commands will: 
   - Clone the AWS F1 tools
   - Setup the platform directory
   - Clone this repository to get source code
   - Source XRT
   - Source AWS F1 tools

1. If you are using the AWSEducate instances then execute the following to clone the *xup\_compute\_acceleration* repository. No additional variables need to be set as the provided AMI has all necessary files including `aws-fpga` repository cloning, the AWS F1 tools, Hardware Development Kit (HDK) and documentation

   ```sh
   git clone https://github.com/Xilinx/xup_compute_acceleration.git
   ```

   The previous command will: 
   - Clone this repository to get source code and solutions

For more details see: https://github.com/aws/aws-fpga/blob/master/Vitis/README.md

---------------------------------------
<p align="center">Copyright&copy; 2021 Xilinx</p>