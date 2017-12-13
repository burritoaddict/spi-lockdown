# SPI Lockdown Kernel Module

This kernel module enables FLOCKDN on supported SPI controllers.

Enabling FLOCKDN prevents SPI protected memory ranges from being
modified, and is recommended to prevent unwanted firmware modifications.

This module is meant to be used as part of a SecureBoot environment.

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

## references

* http://opensecuritytraining.info/IntroBIOS_files/Day2_03_Advanced%20x86%20-%20BIOS%20and%20SMM%20Internals%20-%20SPI%20Flash%20Protection%20Mechanisms.pdf
* https://github.com/chipsec/chipsec/
