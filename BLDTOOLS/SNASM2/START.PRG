;# Startup Script for SEGA Saturn and 32X SNASM2 Debugger (CartDev Supported)
;# SNBUG Version 2.10b - built 20 September, 1995.
set targ_id [getscsiname]
switch [ lindex $targ_id 2 ] {
	Mars	{
			cont 0x06 0x31						 ;# Set buffer config to default 1
			toaddr 0xe080000 1 0x01
			toaddr 0xe080001 1 0x01
			toaddr 0x60C0000 1 0xf1
			toaddr 0xe000000 1 0x02
			toaddr 0xe000001 1 0x7f
			toaddr 0xb000000 @~marsmon.bin ;# '~' specifies from .EXE directory
			toaddr 0xe000000 1 0x00
			toaddr 0xe000001 1 0x40
			toaddr 0x60C0000 1 0xf3
			sleep 5
		}
	Saturn  {
			set lc 0
			set boot 1
			todpr 0x25 4 0xFFFFFFFF
			;# For Saturn CartDev Rev A/B & monitor with system init
			cont 0x06 0x31          ;# Set buffer config to default #1
			cont 0x06 0x01          ;# Reset CartDev control registers
			cont 0x03 0x30          ;# Set mon RAM write protect
			cont 0x03 0x10          ;# Set mon RAM access to CartDev
			toaddr 0xb000000 @~satmon.bin ;# Download monitor
			cont 0x03 0x40          ;# Clear mon RAM write protect
			cont 0x03 0x20          ;# Set mon RAM access to target
			cont 0x07 0x09		;# magic short screen ON (Rev B)
    		cont 0x00 0x16    ;# System Reset A
			while { $boot != 0 && $lc <= 15 } {
				set boot [fromdpr 0x25 0x4]
				incr lc 1
				sleep 1
			}
		}
}
