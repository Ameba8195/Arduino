#!/bin/sh

#===============================================================================
CURRENT_UTILITY_DIR=$(pwd)
GDBSCPTFILE="../../../component/soc/realtek/8195a/misc/gcc_utility/rtl_gdb_flash_write.txt"

#===============================================================================
RLXSTS=$(ps -W | grep "rlx_probe_driver.exe" | grep -v "grep" | wc -l)
echo $RLXSTS
JLKSTS=$(ps -W | grep "JLinkGDBServer.exe" | grep -v "grep" | wc -l)
echo $JLKSTS

echo $CURRENT_UTILITY_DIR

#===============================================================================
#make the new string for being written
if [ $RLXSTS = 1 ]
then
	echo "probe get"
	
	#-------------------------------------------
	LINE_NUMBER=$(grep -n "monitor reset " $GDBSCPTFILE | awk -F":" '{print $1}')
	DEFAULT_STR=$(grep -n "monitor reset " $GDBSCPTFILE | awk -F":" '{print $2}')
	#echo $LINE_NUMBER
	echo $DEFAULT_STR
	STRLEN_DFT=$(expr length "$DEFAULT_STR")
	DEFAULT_STR="#monitor reset 1"
	echo $DEFAULT_STR               
	#-------------------------------------------
	SED_PARA="$LINE_NUMBER""c""$DEFAULT_STR"
	sed -i "$SED_PARA" $GDBSCPTFILE
	
	#===========================================
	LINE_NUMBER=$(grep -n "monitor sleep " $GDBSCPTFILE | awk -F":" '{print $1}')
	DEFAULT_STR=$(grep -n "monitor sleep " $GDBSCPTFILE | awk -F":" '{print $2}')
	#echo $LINE_NUMBER
	echo $DEFAULT_STR
	STRLEN_DFT=$(expr length "$DEFAULT_STR")
	DEFAULT_STR="#monitor sleep 20"
	echo $DEFAULT_STR               
	#-------------------------------------------
	SED_PARA="$LINE_NUMBER""c""$DEFAULT_STR"
	sed -i "$SED_PARA" $GDBSCPTFILE
else

if [ $JLKSTS = 1 ]
then
	echo "jlink get"
	
	#-------------------------------------------
	LINE_NUMBER=$(grep -n "monitor reset " $GDBSCPTFILE | awk -F":" '{print $1}')
	DEFAULT_STR=$(grep -n "monitor reset " $GDBSCPTFILE | awk -F":" '{print $2}')
	#echo $LINE_NUMBER
	echo $DEFAULT_STR
	STRLEN_DFT=$(expr length "$DEFAULT_STR")
	DEFAULT_STR="monitor reset 1"
	echo $DEFAULT_STR               
	#-------------------------------------------
	SED_PARA="$LINE_NUMBER""c""$DEFAULT_STR"
	sed -i "$SED_PARA" $GDBSCPTFILE
	
	#===========================================
	LINE_NUMBER=$(grep -n "monitor sleep " $GDBSCPTFILE | awk -F":" '{print $1}')
	DEFAULT_STR=$(grep -n "monitor sleep " $GDBSCPTFILE | awk -F":" '{print $2}')
	#echo $LINE_NUMBER
	echo $DEFAULT_STR
	STRLEN_DFT=$(expr length "$DEFAULT_STR")
	DEFAULT_STR="monitor sleep 20"
	echo $DEFAULT_STR               
	#-------------------------------------------
	SED_PARA="$LINE_NUMBER""c""$DEFAULT_STR"
	sed -i "$SED_PARA" $GDBSCPTFILE
	
fi
fi

#===============================================================================
