# x86 SPI Lockdown Kernel Module

This kernel module exposes the FLOCKDN bit on x86 systems via sysctl.

Enabling FLOCKDN prevents SPI protected memory range registers from being
modified. (See: Section 21.4.2 in Intel® 8 Series/C220 Series Chipset
Family Platform Controller Hub (PCH) - Datasheet)

This module is meant to be used as part of a SecureBoot environment.
Where it is hardened such that there is no /dev/mem access or privileged
IO available. (because if you have those things you can just do this
from userspace)

Tested on debian, 3.16 kernel, and ICH10R LPC Interface
Controller

## Installation and Usage

    apt-get install linux-headers-$(uname -r)
    git clone git@github.com:burritoaddict/spi-lockdown.git
    cd spi-lockdown
    make
    insmod spi_lockdown.ko
    sysctl -w dev.spi_lockdown.flockdn=1

Note: flockdn cannot be disabled after running sysctl without a 'reset'.
On some systems that means waking from sleep ( For example:
https://support.apple.com/en-ca/HT204934)

## TODO

* Review datasheets for all the chipsets dumped into lpc_chipsets :(
* Error handling and cleanup
* Support more ICH
* Expose SPI protected memory ranges
* Expose Flash Region Access Permissions Register
* Expose Software Sequencing Flash Control Register
* Expose flash descriptor region and mark unwritable
* Re-write register values on wakeup from sleep
* Instructions for statically linking into kernel

## References

* https://www.intel.com/content/dam/www/public/us/en/documents/datasheets/8-series-chipset-pch-datasheet.pdf
* http://opensecuritytraining.info/IntroBIOS_files/Day2_03_Advanced%20x86%20-%20BIOS%20and%20SMM%20Internals%20-%20SPI%20Flash%20Protection%20Mechanisms.pdf
* https://github.com/chipsec/chipsec/
* https://www.kernel.org/doc/Documentation/PCI/pci.txt
* https://static.lwn.net/images/pdf/LDD3/ch09.pdf
* https://github.com/torvalds/linux/blob/master/drivers/mfd/lpc_ich.c
* http://www.intelsecurity.com/advanced-threat-research/content/AttackingAndDefendingBIOS-RECon2015.pdf
