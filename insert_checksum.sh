#!/bin/bash

set -e

binary=$1

if [ ! -f "${binary}" ]; then
    exit 1
fi

text_flash_off="0x$(objdump -h "${binary}" | awk '$2 == ".text" { print $4 }')"
text_elf_off="0x$(objdump -h "${binary}" | awk '$2 == ".text" { print $6 }')"
header_flash_off="0x$(objdump -t "${binary}" | awk '$NF == "bootloader_info" { print $1 }')"
header_elf_off="$(printf "0x%08X\n" $(( text_elf_off + header_flash_off - text_flash_off )))"
checksum_member_off="0x00000010"
checksum_flash_off="$(printf "0x%08X\n" $(( header_flash_off + checksum_member_off )))"
checksum_elf_off="$(printf "0x%08X\n" $(( header_elf_off + checksum_member_off )))"

temp_bin="$(mktemp)"
arm-none-eabi-objcopy -O binary ${binary} ${temp_bin}
checksum="$(printf "0x%08X\n" $(cksum -a crc32b ${temp_bin} | awk '{ print $1 }'))"
checksum_binary="$(printf "%08X\n" ${checksum} | fold -w2 | tac | tr -d "\n" | sed -e 's/\(..\)/\\x\1/g')"
rm ${temp_bin}

printf "${checksum_binary}" | dd of=${binary} bs=1 seek=$((checksum_elf_off)) conv=notrunc status=none

exit 0
