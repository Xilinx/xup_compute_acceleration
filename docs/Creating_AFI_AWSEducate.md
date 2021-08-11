---
layout: default
---

# Creating an Amazon FPGA Image (AFI) using AWSEducate

This document guides you through the steps to create an AWS Amazon FPGA Image (AFI), when using AWSEducate instance, which can run on AWS EC2 F1 instance to verify that the design works in hardware. It assumes that a full system (Vitis project) is built which consists of an *host* application (executable file) and an FPGA binary file (.xclbin).

### Create an AFI

1. Setup CLI and Create S3 bucket

    - Create an empty file called *credentials* using the following commands

      ```
      cd
      mkdir .aws
      cd .aws
      gedit credentials
      ```

    - Go to the Vocareum window and click on **Account Details**
    - Click on the **Show** button
    - Copy the content into the *credentials* file, save the file, and exit
    - Create a S3 bucket which will be used for registering xclbin (Note: touch command below is necessary to have non-empty folders. The files will be copied into these sub-folders

      ```
      aws s3 mb s3://<bucket-name> --region us-east-1 
      aws s3 mb s3://<bucket-name>/<dcp-folder-name>
      touch FILES_GO_HERE.txt
      aws s3 cp FILES_GO_HERE.txt s3://<bucket-name>/<dcp-folder-name>/
      aws s3 mb s3://<bucket-name>/<logs-folder-name>
      touch FILES_GO_HERE.txt
      aws s3 cp FILES_GO_HERE.txt s3://<bucket-name>/<logs-folder-name>/
      ```

1. Configure aws by executing following command and providing credentials

   Note that anytime you want to create AFI, the credentials file content must be updated. You must rerun the command every time before you run the create_visit_afi.sh script if this is a new session

   ```
   aws configure
   AWS Access Key ID [****************J5NS]:  <hit enter> 
   AWS Secret Access Key [****************N4bG]:  <hit enter>
   Default region name [None]: us-east-1 <make sure us-east-1 is displayed, otherwise change it to it>
   Default output format [None]: json <make sure json is displayed, otherwise change it to it>
   ```

2. Submit the xclbin to generate AFI (with extension awsxclbin) using the following command

   ```
   $VITIS_DIR/tools/create_vitis_afi.sh -xclbin=<path to an including.xclbin> -s3_bucket=<bucket-name> -s3_dcp_key=<dcp-folder-name> -s3_logs_key=<logs-folder-name>
   ```

     The bucket-name, dcp-folder-name, and logs-folder-name should match the one used while creating them 
3. Once submitted successfully, an *time_stamp*\_afi\_id.txt file will be created. Open the file and make a note of the afi-id. Execute the following command to see the status of the AFI:

      `aws ec2 describe-fpga-images --fpga-image-ids <afi_id>`

    Wait for about 30 minutes before the status changes from *pending* to **available**. You can log out and login back to check the status


## Regenerate .awsxclbin

You can regenerate the `.awsxclbin` file as long as you have access to `*agfi_id.txt` and `*.xclbin` files

1. Edit these variable with the corresponding names
    
   ```sh
   export xclbin=<xclbin_filename>
   export agfi_id=<*_agfi_id.txt>
   export awsxclbin=<output_name>
   ```
    
1. Generate `.awsxclbin` file

   ```sh
   xclbinutil -i $xclbin --remove-section PARTITION_METADATA --remove-section SYSTEM_METADATA --replace-section BITSTREAM:RAW:${agfi_id} -o ${awsxclbin}.awsxclbin
   ```

---------------------------------------
<p align="center">Copyright&copy; 2021 Xilinx</p>