# SPI Lockdown Kernel Module

This kernel module exposes the FLOCKDN on x86 systems bit via sysctl.

Enabling FLOCKDN prevents SPI protected memory ranges from being
modified, and is recommended to prevent unwanted firmware modifications.

This module is meant to be used as part of a SecureBoot environment.
Where it is hardened such that there is no /dev/mem access or privileged
IO available.

Note: tested on debian, 3.16 kernel, and ICH10R LPC Interface
Controller

## Usage

* apt-get install linux-headers-$(uname -r)
* make
* insmod spi_lockdown.ko
* sysctl -w dev.spi_lockdown.flockdn=1

## TODO

* Error handling and cleanup
* Support more ICH
* Expose SPI protected memory ranges
* Expose Flash Region Access Permissions Register
* Expose Software Sequencing Flash Control Register
* Expose flash descriptor region and mark unwritable
* Instructions for statically linking into kernel

## References

* http://opensecuritytraining.info/IntroBIOS_files/Day2_03_Advanced%20x86%20-%20BIOS%20and%20SMM%20Internals%20-%20SPI%20Flash%20Protection%20Mechanisms.pdf
* https://github.com/chipsec/chipsec/
* https://www.kernel.org/doc/Documentation/PCI/pci.txt
* https://static.lwn.net/images/pdf/LDD3/ch09.pdf
