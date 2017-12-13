#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/sysctl.h>

#include "spi_lockdown.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("burrito");
MODULE_DESCRIPTION("");
MODULE_VERSION("0.1");

u32 flockdn_flag = 0;
union ich_hws_flash_status hsfsts;
u32 spi_base = 0;

static ctl_table spi_lockdown_table[] = {
  {
    .procname = "flockdn",
    .data = &flockdn_flag,
    .maxlen = sizeof(int),
    .mode = 0644,
    .proc_handler = &flockdn_sysctl_handler,
  }, {0}
};

static ctl_table spi_lockdown_dev_table[] = {
  {
    .procname = "spi_lockdown",
    .mode = 0555,
    .child = spi_lockdown_table
  }, {0}
};

static ctl_table spi_lockdown_root_table[] = {
  {
    .procname = "dev",
    .mode = 0555,
    .child = spi_lockdown_dev_table
  }, {0}
};

static struct ctl_table_header *spi_lockdown_ctl_table_header;

int flockdn_sysctl_handler(struct ctl_table *ctl, int write,
    void __user *buffer, size_t *lenp, loff_t *ppos)
{
  int ret;

  ret = proc_dointvec(ctl, write, buffer, lenp, ppos);

  if(write) {
    void * hsfsts_target = 0;

    if(!flockdn_flag){
      printk(KERN_ERR "you can't disable FLOCKDN once it is enabled\n");
      flockdn_flag = 1;
      return -1;
    }

    if(!spi_base){
      printk(KERN_ERR "Cannot set FLOCKDN because we were unable to "
          "locate SPIBAR\n");
      return -1;
    }

    printk(KERN_INFO "spi_lockdown writing FLOCKDN");

    hsfsts_target = ioremap_nocache(spi_base + SPIBASE_LPT_HSFS_OFFSET,
        sizeof(hsfsts.regval));
    hsfsts.regval = readw(hsfsts_target);
    flockdn_flag = hsfsts.hsf_status.flockdn;
    hsfsts.hsf_status.flockdn = 1;
    writew(hsfsts.regval, hsfsts_target);
    iounmap(hsfsts_target);
  }

  return 0;
}

int spi_lockdown_init(void){
  int i;

  spi_lockdown_ctl_table_header = register_sysctl_table(
      spi_lockdown_root_table);

  printk(KERN_INFO "spi_lockdown loading\n");

  for(i = 0; i < sizeof(lpc_ich_ids) / sizeof(struct pci_device_id); i++){
    struct pci_dev *dev = NULL;

    while ((dev = pci_get_device(lpc_ich_ids[i].vendor,
            lpc_ich_ids[i].device, dev))){
      u32 rcba, bcr;
      void * hsfsts_target = 0;
      struct lpc_ich_priv *priv;

      priv = pci_get_drvdata(dev);

      switch(lpc_chipset_info[priv->chipset].spi_type){
        case INTEL_SPI_LPT:
          spi_base = rcba = bcr = 0;

          printk(KERN_DEBUG "Got vendor: %d, device: %d\n", dev->vendor,
              dev->device);
          pci_read_config_dword(dev, RCBABASE, &rcba);
          printk(KERN_DEBUG "RCBA base: 0x%.8x\n", rcba);
          spi_base = round_down(rcba, SPIBASE_LPT_SZ) + SPIBASE_LPT;
          printk(KERN_DEBUG "SPI base: 0x%.8x\n", spi_base);
          hsfsts_target = ioremap_nocache(spi_base + SPIBASE_LPT_HSFS_OFFSET,
              sizeof(hsfsts.regval));
          hsfsts.regval = readw(hsfsts_target);
          flockdn_flag = hsfsts.hsf_status.flockdn;
          iounmap(hsfsts_target);

          return 0;
          break; /* unreachable */

        case INTEL_SPI_BYT:
          pci_read_config_dword(dev, SPIBASE_BYT, &spi_base);
          spi_base = spi_base & ~(SPIBASE_BYT_SZ - 1);
          // TODO: investigate if we can actually read HSFS from this.

        case INTEL_SPI_BXT:
// http://elixir.free-electrons.com/linux/latest/source/drivers/mfd/lpc_ich.c#L1128
        default:
          printk(
            KERN_ERR
            "Unsupported ICH detected. (CHIPSET: %.2x, SPI_TYPE: %.2x)\n",
            priv->chipset, lpc_chipset_info[priv->chipset].spi_type
          );
          break;
      }
    }
  }

  unregister_sysctl_table(spi_lockdown_ctl_table_header);
  return -ENXIO;
}

void spi_lockdown_exit(void){
  printk(KERN_INFO "spi_lockdown unloading\n");

  unregister_sysctl_table(spi_lockdown_ctl_table_header);
}

module_init(spi_lockdown_init);
module_exit(spi_lockdown_exit);
