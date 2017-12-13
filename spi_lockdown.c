#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include "spi_lockdown.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("burrito");
MODULE_DESCRIPTION("");
MODULE_VERSION("0.1");

int spi_lockdown_init(void){
  int i;

  printk(KERN_INFO "spi_lockdown attempting to lock spi_flash permissions\n");

  printk(KERN_INFO "Searching for input controller hub\n");

  for(i = 0; i < sizeof(lpc_ich_ids) / sizeof(struct pci_device_id); i++){
    struct pci_dev *dev = NULL;

    while ((dev = pci_get_device(lpc_ich_ids[i].vendor,
            lpc_ich_ids[i].device, dev))){
      u32 spi_base, rcba, bcr;
      union ich_hws_flash_status hsfsts;
      void * hsfsts_target;

      struct lpc_ich_priv *priv;

      priv = pci_get_drvdata(dev);

      if(lpc_chipset_info[priv->chipset].spi_type != INTEL_SPI_LPT) {
        printk(
            KERN_INFO
            "Unsupported ICH detected. (CHIPSET: %.2x, SPI_TYPE: %.2x)\n",
            priv->chipset, lpc_chipset_info[priv->chipset].spi_type
        );
        continue;
      }

      spi_base = rcba = bcr = 0;

      printk(KERN_INFO "Got vendor: %d, device: %d\n", dev->vendor,
          dev->device);

      // Okay, we found an input controller hub. try to grab SPIBAR!
      pci_read_config_dword(dev, RCBABASE, &rcba);

      printk(KERN_INFO "RCBA base: 0x%.8x\n", rcba);

      spi_base = round_down(rcba, SPIBASE_LPT_SZ) + SPIBASE_LPT;

      printk(KERN_INFO "SPI base: 0x%.8x\n", spi_base);

      hsfsts_target = ioremap_nocache(spi_base + 0x4,
          sizeof(hsfsts.regval));

      hsfsts.regval = readw(hsfsts_target);

      printk(KERN_INFO "hsfsts.flcdone = %.1x\n",
          hsfsts.hsf_status.flcdone);
      printk(KERN_INFO "hsfsts.dael = %.1x\n",
          hsfsts.hsf_status.dael);
      printk(KERN_INFO "hsfsts.berasesz = %.1x\n",
          hsfsts.hsf_status.berasesz);
      printk(KERN_INFO "hsfsts.flcinprog = %.1x\n",
          hsfsts.hsf_status.flcinprog);
      printk(KERN_INFO "hsfsts.fldesvalid = %.1x\n",
          hsfsts.hsf_status.fldesvalid);
      printk(KERN_INFO "hsfsts.flockdn = %.1x\n",
          hsfsts.hsf_status.flockdn);

      hsfsts.hsf_status.flockdn = 1;

      writew(hsfsts.regval, hsfsts_target);

      iounmap(hsfsts_target);
    }
  }

  return 0;
}

void spi_lockdown_exit(void){
  printk(KERN_INFO "spi_lockdown unloading\n");
}

module_init(spi_lockdown_init);
module_exit(spi_lockdown_exit);
