###!/bin/sh

chr() {
  [ ${1} -lt 256 ] || return 1
  printf \\$(printf '%03o' $1)
}

# Another version doing the octal conversion with arithmetic
# faster as it avoids a subshell
chr () {
  [ ${1} -lt 256 ] || return 1
  printf \\$(($1/64*100+$1%64/8*10+$1%8))
}

# Another version using a temporary variable to avoid subshell.
# This one requires bash 3.1.
chr() {
  local tmp
  [ ${1} -lt 256 ] || return 1
  printf -v tmp '%03o' "$1"
  printf \\"$tmp"
}


ord() {
  LC_CTYPE=C printf '%d' "'$1"
}


SOURCE="${BASH_SOURCE[0]}"

# resolve $SOURCE until the file is no longer a symlink
# if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located

while [ -h "$SOURCE" ]; do 
  TARGET="$(readlink "$SOURCE")"
  if [[ $SOURCE == /* ]]; then
#    echo "SOURCE '$SOURCE' is an absolute symlink to '$TARGET'"
    SOURCE="$TARGET"
  else
    DIR="$( dirname "$SOURCE" )"
#    echo "SOURCE '$SOURCE' is a relative symlink to '$TARGET' (relative to '$DIR')"
    SOURCE="$DIR/$TARGET" 
  fi
done

RDIR="$( dirname "$SOURCE" )"
DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

cd "$DIR"

cp -f ../../../build/target.axf .
#cp -f ../../../build/ram_1.bin .
#cp -f ../../../build/ram_2.bin .

TOOLCHAIN=../gcc-arm-none-eabi-4.8.3-2014q1/bin/arm-none-eabi


chmod 777 ./target.axf
cp ./target.axf ./target_pure.axf
${TOOLCHAIN}-strip ./target_pure.axf

${TOOLCHAIN}-objcopy -j .ram.start.table -j .ram_image1.text \
-Obinary  ./target_pure.axf ./ram_1.bin

${TOOLCHAIN}-objcopy -j .image2.start.table -j .ram_image2.text \
-Obinary  ./target_pure.axf ./ram_2.bin

${TOOLCHAIN}-objcopy -j .image3 -j .ARM.exidx -j .sdr_data \
-Obinary  ./target_pure.axf ./ram_3.bin

${TOOLCHAIN}-nm.exe target.axf | sort > target.map

${TOOLCHAIN}-objdump.exe -d target.axf > target.asm

rm -f ./ram_1_prepend.bin
rm -f ./ram_2_prepend.bin
rm -f ./ram_3_prepend.bin

./prepend_header.sh ./ram_1.bin __ram_image1_text_start__ ./target.map 44
./prepend_header.sh ./ram_2.bin __ram_image2_text_start__ ./target.map
./prepend_header.sh ./ram_3.bin __sdram_image3_text_start__ ./target.map

#make pad
rm -f ./ram_1_prepend.bin.pad
./imgpad ./ram_1_prepend.bin 45056
cat ./ram_1_prepend.bin.pad ./ram_2_prepend.bin ./ram_3_prepend.bin > ./ram_all.bin


mbed_disk=100
while [[ $mbed_disk -le 122 ]] 
do
    
    diskname=`chr $mbed_disk`
    filename="/cygdrive/$diskname/mbed.htm"
    if [ -f $filename ];
    then
        break
    fi
    mbed_disk=$((mbed_disk+1))
done


if [[ $mbed_disk -le 122 ]];
then 
  echo cp ./ram_all.bin /cygdrive/$diskname/
  cp ./ram_all.bin /cygdrive/$diskname/
else
  echo mbed usb disk not found
fi

