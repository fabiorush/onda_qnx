@echo off

REM #!\bin\sh
REM # script to build a binary IPL and boot image for the OMAP3730 Beagle board

echo Converting ELF IPL file to binary

set QNX_HOST=c:\QNX650\host\win32\x86

REM # Convert IPL into Binary format
%QNX_HOST%\usr\bin\ntoarm-objcopy --input-format=elf32-littlearm --output-format=binary ..\src\hardware\ipl\boards\omap3730_beagle_xm\arm\le.v7\ipl-omap3730_beagle_xm .\tmp-ipl-omap3730_beagle_xm.bin

REM # Pad Binary IPL to 24K image, this is the image used by boot from UART
mkrec -s24k -ffull -r .\tmp-ipl-omap3730_beagle_xm.bin > .\ipl-omap3730_beagle_xm.bin

echo Skipping NAND items as not needed for Beagleboard xM

REM # Convert IPL header into Binary format
REM %QNX_HOST%\usr\bin\ntoarm-objcopy --input-format=elf32-littlearm --output-format=binary ..\src\hardware\ipl\boards\omap3730_beagle_xm\arm\le.v7\boot_header.o .\tmp-boot-header.bin

REM # Cat boot header and ipl together
REM cat .\tmp-boot-header.bin .\tmp-ipl-omap3730_beagle_xm.bin > .\tmp-header-ipl-omap3730_beagle_xm.bin

REM # Pad Binary IPL with Header to 24K image, this is the image to put on NAND and boot from NAND
REM mkrec -s24k -ffull -r .\tmp-header-ipl-omap3730_beagle_xm.bin > .\nand-ipl-omap3730_beagle_xm.bin

REM # clean up temporary files
rm -f tmp*.bin

echo Done!
@echo on
