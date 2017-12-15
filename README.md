# x86 SPI Lockdown Kernel Module

This module is meant to be used to prevent firmware modification as part of a
[SecureBoot](https://www.rodsbooks.com/efi-bootloaders/controlling-sb.html) authenticated execution environment.
Where it is [hardened](https://pax.grsecurity.net/docs/pax-future.txt) such that there is [no /dev/mem access](https://en.wikibooks.org/wiki/Grsecurity/Appendix/Grsecurity_and_PaX_Configuration_Options#Deny_reading/writing_to_/dev/kmem,_/dev/mem,_and_/dev/port) or privileged
IO available. (because if you have those things you can just do this
from userspace)

Read [these slides](http://opensecuritytraining.info/IntroBIOS_files/Day2_03_Advanced%20x86%20-%20BIOS%20and%20SMM%20Internals%20-%20SPI%20Flash%20Protection%20Mechanisms.pdf) for a full description of how these mechanisms work.

Tested on amd64 debian, 3.16 kernel, and [ICH10R LPC Interface

Setting Flash Regions Access Permission Register controls which regions
of flash memory can be read and written.

Setting Protected Ranges Registers controls which address ranges are
read or write protected.

Enabling FLOCKDN prevents SPI protected memory range registers from being
modified. (See: Section 21.4.2 in [Intel® 8 Series PCH
Datasheet](https://www.intel.com/content/dam/www/public/us/en/documents/datasheets/8-series-chipset-pch-datasheet.pdf))

Tested on amd64 debian, 3.16 kernel, and [ICH10R LPC Interface
Controller](https://www.intel.ca/content/dam/doc/datasheet/io-controller-hub-10-family-datasheet.pdf)

## Installation and Usage

    apt-get install linux-headers-$(uname -r)
    git clone git@github.com:burritoaddict/spi-lockdown.git
    cd spi-lockdown
    make
    insmod spi_lockdown.ko
    sysctl -w dev.spi_lockdown.pr0=2415886336   # write protect entire
region (0x8fff8000)
    sysctl -w dev.spi_lockdown.flockdn=1        # prevent SPI protection
modifications

Note: flockdn cannot be disabled after running sysctl without a 'reset'.
[On some systems that means waking from sleep](https://support.apple.com/en-ca/HT204934)

### sysctl interfaces

* dev.spi_lockdown.flockdn - FLOCKDN bit in Hardware Sequencing Flash
  Status Register
* dev.spi_lockdown.frap - Flash Regions Access Permissions Register
* dev.spi_lockdown.pr0 - Protected Range 0 Register
* dev.spi_lockdown.pr1 - Protected Range 1 Register
* dev.spi_lockdown.pr2 - Protected Range 2 Register
* dev.spi_lockdown.pr3 - Protected Range 3 Register
* dev.spi_lockdown.pr4 - Protected Range 4 Register

## TODO

* switch to use sysfs instead of sysctl
* Review datasheets for all the chipsets dumped into lpc_chipsets to
  determine spibar calculation method... :(
* ~~Error handling and cleanup~~
* ~~Expose SPI protected memory ranges~~
* ~~Expose Flash Region Access Permissions Register~~
* Expose Software Sequencing Flash Control Register
* Expose flash descriptor region and mark unwritable
* Re-write register values on wakeup from sleep?

## References

* https://www.intel.com/content/dam/www/public/us/en/documents/datasheets/8-series-chipset-pch-datasheet.pdf
* http://opensecuritytraining.info/IntroBIOS_files/Day2_03_Advanced%20x86%20-%20BIOS%20and%20SMM%20Internals%20-%20SPI%20Flash%20Protection%20Mechanisms.pdf
* https://github.com/chipsec/chipsec/
* https://www.kernel.org/doc/Documentation/PCI/pci.txt
* https://static.lwn.net/images/pdf/LDD3/ch09.pdf
* https://github.com/torvalds/linux/blob/master/drivers/mfd/lpc_ich.c
* http://www.intelsecurity.com/advanced-threat-research/content/AttackingAndDefendingBIOS-RECon2015.pdf
