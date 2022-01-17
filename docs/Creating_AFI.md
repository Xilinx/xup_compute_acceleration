---
layout: default
--- 

# Creating an Amazon FPGA Image (AFI)

This document guides you through the steps to create an AWS Amazon FPGA Image (AFI) which can run on AWS EC2 F1 instance to verify that the design works in hardware. It assumes that a full system (Vitis project) is built which consists of an *host* application (executable file) and an FPGA binary file (.xclbin).

### Create an AFI

#### AWSEducate users follow [these steps](Creating_AFI_AWSEducate.md) to create an AFI


To execute the application on F1, the following files are needed:

- Host application (executable file)
- Amazon FPGA Image (awsxclbin)

The awsxclbin is an Amazon specific version of the FPGA binary file (xclbin) produced by the Vitis software.

The awsxclbin can be created by running the *create\_vitis\_afi.sh* script which is included in the [aws-fpga GitHub repository](https://github.com/aws/aws-fpga/tree/master/Vitis#2-create-an-amazon-fpga-image-afi).

The script can be found in the following location in the aws-fpga repository:

```sh
$VITIS_DIR/tools/create_vitis_afi.sh
```

1. Before running the commands below, make sure the Vitis setup script has been sourced (the following command assumes the aws-fpga Git repository is cloned to the user home area)

   ```sh
   source ~/aws-fpga/vitis_setup.sh
   ```

1. Set up S3 bucket region

   For instance:

   ```sh
   aws configure set region us-east-1
   ```

   Note: the region may change for your instance. This command will create/update the `~/.aws/config` file

1. Create an AFI by running the `create_vitis_afi.sh` script and wait for the completion of the AFI creation process

   ```sh
   $VITIS_DIR/tools/create_vitis_afi.sh -xclbin=<filename>.xclbin -s3_bucket=<bucket-name> -s3_dcp_key=<dcp-folder-name> -s3_logs_key=<logs-folder-name>
   ```

   In the above command, set your *xclbin* file as `<filename>`; the Amazon S3 `<bucket-name>`, `<dcp-folder-name>`, and `<logs-folder-name>` with the names you had given when running CLI script.  You can choose any valid folder name for the dcp and logs folder. The Amazon S3 bucket name should match an S3 bucket you have set up.

   Learn more about setting up S3 buckets [here](https://github.com/aws/aws-fpga/blob/master/Vitis/docs/Setup_AWS_CLI_and_S3_Bucket.md)

The `create_vitis_afi.sh` script does the following:

- Starts a background process to create the AFI
- Generates a `*_afi_id.txt` which contains the FPGA Image Identifier (or AFI ID) and Global FPGA Image Identifier (or AGFI ID) of the generated AFIs
- Creates the `*.awsxclbin` AWS FPGA binary file which is passed to the host application to determine which AFI should be loaded to the FPGA.
- Uploads the `*.xclbin` to the AWS cloud for processing.

## Check the AFI status

The AFI will become available after some time in the AWS cloud and can then be used to program the FPGA in an AWS EC2 F1 instance. To check the AFI status, the AFI ID is required.

* In the directory the `create_vitis_afi.sh` script was run, enter the following command to find the AFI ID  

```sh
cat *afi_id.txt
```

* Enter the `describe-fpga-images` API command to check the status of the AFI generation process:

```sh
aws ec2 describe-fpga-images --fpga-image-ids <AFI_ID>
```

* For example,

```sh
aws ec2 describe-fpga-images --fpga-image-ids afi-0b9167434a1c74ba9
```

Or you can use a handy shortcut to pass the AFI id directly to the command. Read the file, get the second row, remove `"` and `,` and finally remove everything before the colon included

```sh
aws ec2 describe-fpga-images --fpga-image-ids $(cat *afi_id.txt | sed -n '2p' | tr -d '",' | sed 's/.*://')
```

Note: When AFI creation is in progress, the *State* will be `pending`. When the AFI creation is finished, the output should show `available`:

```sh
   ...
   "State": {
       "Code": "available"
   },

    ...
```

Wait until the AFI becomes available before proceeding to execute on the F1 instance.

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