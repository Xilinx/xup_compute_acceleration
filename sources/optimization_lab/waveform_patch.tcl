#!/usr/bin/tclsh

proc printHelpNExit {} {
  puts "\n This script accepts an xclbin created using Vitis v++ for Hardware Emulation Flow and edits the waveform configuration used to show Simulation Waveform, if applicable.\n It then creates a new xclbin at given output with the new waveform configution.\n"
  puts " Usage  : ./script.tcl <input_xclbin> <output_xclbin>"
  puts " Option : --help : Print help message. "
  exit
}

# Define procedure to cleanly delete "xlnxEditWcfg" directory while exiting
proc cleanExit { dir } {
  cd $dir
  exec rm -rf xlnxEditWcfg
  exit
}

# Check arguments

if { $argc < 1 } {
  printHelpNExit
}

set first_arg [lindex $argv 0]
if { $first_arg == "--help" } {
  printHelpNExit
} elseif { $argc < 2 } {
  # User has to provide both input and output xclbin
  printHelpNExit
}

# Check whether given xclbin exists
set xclbin $first_arg
if { ![file exists $xclbin] } {
  puts "\n Given input xclbin does not exist. Please provide an xclbin created using Vitis v++ for Hardware Emulation Flow."
  exit
}

# Check whether $XILINX_XRT environment variable is set, so that path to "xclbinutil" could be found
set xrt_path $env(XILINX_XRT)
if { $xrt_path == "" } {
  puts " Environment variable XILINX_XRT is not set. Please set it and run the script again."
  exit
}

# Find xclbinutil
set xclbinutil_path "$xrt_path/bin/xclbinutil"
if { ![file exists $xclbinutil_path] } {
  puts " xclbinutil executable does not exist at $xclbinutil_path. Exiting. "
  exit
}

# Create xlnxEditWcfg local directory
if { [file exists xlnxEditWcfg] } {
  puts " Deleting already existing \"xlnxEditWcfg\"."
  exec rm -rf xlnxEditWcfg
}
set work_dir [pwd]
exec mkdir xlnxEditWcfg


# Get xclbin info to check Platform and Flow Type
exec $xclbinutil_path --info --input $xclbin >> ./xlnxEditWcfg/binInfo.txt

# Check whether input xclbin is for Hardware Emulation flow
set contentType [exec grep "Content:" ./xlnxEditWcfg/binInfo.txt]
set isHwEmu [string match "[ ]*Content:[ ]*HW Emulation Binary" $contentType]

if { $isHwEmu != 1 } {
  puts " Input xclbin is not generated for Hardware Emulation Flow and does not need waveform configuration edit. Exiting."
  cleanExit $work_dir
}

# Check Platform VBNV of input xclbin
set curr_platform [exec grep "Platform VBNV:" ./xlnxEditWcfg/binInfo.txt]

set is2RP 0

# List for all the platform types which need waveform configuration edits
set platforms2RP [ list "xilinx_aws-vu9p-f1_shell-v04261818_201920_2" ]
foreach p $platforms2RP {
  set is2RP [ regexp $p $curr_platform ]
  if { $is2RP == 1 } {
    # Current platform is 2RP platform and needs edit in the waveform configuration
    break
  }
}

if { $is2RP != 1 } {
  puts " Input xclbin does not need waveform configuration edit. Exiting."
  cleanExit $work_dir
}    


# Step 1 : Retrieve the bitstream section of the given xclbin
exec $xclbinutil_path --dump-section BITSTREAM:RAW:./xlnxEditWcfg/origBitStream.bit --input $xclbin 
if { ![file exists ./xlnxEditWcfg/origBitStream.bit] } {
  puts " Could not retrieve bitstream from the given xclbin. Please check the xclbin. Exiting."
  cleanExit $work_dir
}

cd xlnxEditWcfg
# Check whether unzip and zip tools are available
if { ![file exists /usr/bin/unzip] } {
  puts " Could not find required tool /usr/bin/unzip. Exiting."
  cleanExit $work_dir
}
if { ![file exists /usr/bin/zip] } {
  puts " Could not find required tool /usr/bin/zip. Exiting."
  cleanExit $work_dir
}

# Step 2 : Unzip bitstream from given xclbin
puts " Decompressing bitstream retrieved from input xclbin..."
exec /usr/bin/unzip origBitStream.bit
if { ![file exists behav_waveform/xsim/dr_behav.wcfg] } {
  puts " Could not find waveform configuration file at [pwd]/behav_waveform/xsim/dr_behav.wcfg. Exiting."
  cleanExit $work_dir
}

# Step 3 : Read behav_waveform/xsim/dr_behav.wcfg and generate ./dr_behav.wcfg if edits are required
set inFile [open "behav_waveform/xsim/dr_behav.wcfg" r]

#Create a dr_behav.wcfg in current directory.
set outFile [open "./dr_behav.wcfg" w] 

# read input and write to output
puts " Reading and editing waveform configuration from input xclbin..."
while { [gets $inFile line] >= 0 } {
  regsub "/emu_wrapper/emu_i/" $line "/emu_wrapper/emu_i/dynamic_region/" outLine
  puts $outFile $outLine
}
close $inFile
close $outFile

# Step 4 : For enlisted 2RP platforms, copy the newly created dr_behav.wcfg into behav_waveform/xsim/dr_behav.wcfg
#          and zip "behav_gdb/" "behav_waveform/" directories into a new binary file
#          Create a new xclbin after replacing the Bitstream section of the given xclbin

#puts "\n For the current 2RP platform, copy the edited Waveform Configuration into a new bitstream and create a new xclbin with it"
exec cp -f ./dr_behav.wcfg behav_waveform/xsim/dr_behav.wcfg
exec /usr/bin/zip -r newBitStream.bit behav_gdb/ behav_waveform/

# Change to starting work directory
cd $work_dir

set out_xclbin [lindex $argv 1]
if { [file exists $out_xclbin] } {
  puts " Output xclbin $out_xclbin already exists. Deleting it."
  exec rm -rf $out_xclbin
}
puts "\n Creating new xclbin at $out_xclbin with corrected Waveform Configuration."
exec $xclbinutil_path --replace-section BITSTREAM:RAW:./xlnxEditWcfg/newBitStream.bit --input $xclbin --output $out_xclbin

cleanExit $work_dir

