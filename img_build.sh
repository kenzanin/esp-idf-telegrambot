#!/usr/bin/env bash
set -e
arg_project=$1
arg_flashimg=$2

if [ -z "$2" ]; then
    echo "Usage: make-flash-img.sh flash_img_file"
    echo "if no param given we used default name flash_img.bin"
    arg_flashimg="flash_image.bin"
fi
rm -vf flash_image.bin
dd if=/dev/zero bs=1024 count=4096 of=${arg_flashimg}
dd if=build/bootloader/bootloader.bin bs=1 seek=$((0x1000)) of=${arg_flashimg} conv=notrunc
dd if=build/partition_table/partition-table.bin bs=1 seek=$((0x8000)) of=${arg_flashimg} conv=notrunc
dd if=build/${arg_project} bs=1 seek=$((0x10000)) of=${arg_flashimg} conv=notrunc
