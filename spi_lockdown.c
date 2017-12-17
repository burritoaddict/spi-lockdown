#include <linux/pci.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sysctl.h>
#include <linux/string.h>
#include <linux/kobject.h>
#include <linux/spinlock.h>

#include "spi_lockdown.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("burrito");
MODULE_DESCRIPTION("Creates sysctl interface to make SPI protected range "
    "(and other) registers immutable");
MODULE_VERSION("0.1");

static struct {
 struct kobject *kobj;
 union ich_protected_range_register pr[5];
 union ich_flash_region_access_permissions frap;
 union ich_hws_flash_status hsfsts;
 u32 spibar;
 u32 rcba;
 struct pci_dev *ich_dev;
 struct lpc_ich_priv *ich_priv;
} spi_lockdown_data;

static struct ctl_table spi_lockdown_table[] = {
  {
    .procname = "hsfsts",
    .data = &spi_lockdown_data.hsfsts.regval,
    .maxlen = sizeof(spi_lockdown_data.hsfsts),
    .mode = 0644,
    .proc_handler = &flockdn_sysctl_handler,
  },
  {
    .procname = "pr0",
    .data = &spi_lockdown_data.pr[0].regval,
    .maxlen = sizeof(spi_lockdown_data.pr[0]),
    .mode = 0644,
    .proc_handler = &pr_sysctl_handler,
  },
  {
    .procname = "pr1",
    .data = &spi_lockdown_data.pr[1].regval,
    .maxlen = sizeof(spi_lockdown_data.pr[1]),
    .mode = 0644,
    .proc_handler = &pr_sysctl_handler,
  },
  {
    .procname = "pr2",
    .data = &spi_lockdown_data.pr[2].regval,
    .maxlen = sizeof(spi_lockdown_data.pr[2]),
    .mode = 0644,
    .proc_handler = &pr_sysctl_handler,
  },
  {
    .procname = "pr3",
    .data = &spi_lockdown_data.pr[3].regval,
    .maxlen = sizeof(spi_lockdown_data.pr[3]),
    .mode = 0644,
    .proc_handler = &pr_sysctl_handler,
  },
  {
    .procname = "pr4",
    .data = &spi_lockdown_data.pr[4].regval,
    .maxlen = sizeof(spi_lockdown_data.pr[4]),
    .mode = 0644,
    .proc_handler = &pr_sysctl_handler,
  },
  {
    .procname = "frap",
    .data = &spi_lockdown_data.frap.regval,
    .maxlen = sizeof(spi_lockdown_data.frap),
    .mode = 0644,
    .proc_handler = &frap_sysctl_handler,
  }, {0}
};

static struct ctl_table spi_lockdown_dev_table[] = {
  {
    .procname = "spi_lockdown",
    .mode = 0555,
    .child = spi_lockdown_table
  }, {0}
};

static struct ctl_table spi_lockdown_root_table[] = {
  {
    .procname = "dev",
    .mode = 0555,
    .child = spi_lockdown_dev_table
  }, {0}
};

static struct ctl_table_header *spi_lockdown_ctl_table_header;

static int read_mmio_u32(u32 addr, u32 *result){
  void * read_target = 0;

  pr_debug("reading 0x%.8x\n", addr);

  read_target = ioremap_nocache(addr,
    sizeof(*result));

  if(!read_target){
    printk(KERN_ERR "ioremap_nocache failed\n");
    return -1;
  }

  *result = readl(read_target);
  iounmap(read_target);

  return 0;
}

static int write_mmio_u32(u32 addr, u32 value){
  void * write_target = 0;

  pr_debug("writing %d to 0x%.8x\n", value, addr);

  write_target = ioremap_nocache(addr,
    sizeof(value));

  if(!write_target){
    printk(KERN_ERR "ioremap_nocache failed\n");
    return -1;
  }

  writel(value, write_target);
  iounmap(write_target);

  return 0;
}

static int read_mmio_u16(u32 addr, u16 *result){
  void * read_target = 0;

  pr_debug("reading 0x%.8x\n", addr);

  read_target = ioremap_nocache(addr,
    sizeof(*result));

  if(!read_target){
    printk(KERN_ERR "ioremap_nocache failed\n");
    return -1;
  }

  *result = readw(read_target);
  iounmap(read_target);

  return 0;
}

static int write_mmio_u16(u32 addr, u16 value){
  void * write_target = 0;

  pr_debug("writing %d to 0x%.4x\n", value, addr);

  write_target = ioremap_nocache(addr,
    sizeof(value));

  if(!write_target){
    printk(KERN_ERR "ioremap_nocache failed\n");
    return -1;
  }

  writew(value, write_target);
  iounmap(write_target);

  return 0;
}

static int pr_sysctl_handler(struct ctl_table *ctl, int write,
    void __user *buffer, size_t *lenp, loff_t *ppos)
{
  int ret;

  u32 *reg = NULL;
  u32 offset = 0;

  if(!strcmp(ctl->procname, "pr0")){
    offset = SPIBASE_LPT_PR0_OFFSET;
    reg = &spi_lockdown_data.pr[0].regval;
  } else if(!strcmp(ctl->procname, "pr1")){
    offset = SPIBASE_LPT_PR1_OFFSET;
    reg = &spi_lockdown_data.pr[1].regval;
  } else if(!strcmp(ctl->procname, "pr2")){
    offset = SPIBASE_LPT_PR2_OFFSET;
    reg = &spi_lockdown_data.pr[2].regval;
  } else if(!strcmp(ctl->procname, "pr3")){
    offset = SPIBASE_LPT_PR3_OFFSET;
    reg = &spi_lockdown_data.pr[3].regval;
  } else if(!strcmp(ctl->procname, "pr4")){
    offset = SPIBASE_LPT_PR4_OFFSET;
    reg = &spi_lockdown_data.pr[4].regval;
  }

  if(!offset || !reg){
    return -1;
  }

  read_mmio_u32(spi_lockdown_data.spibar + offset, reg);

  ret = proc_dointvec(ctl, write, buffer, lenp, ppos);

  if(ret){
    printk(KERN_ERR "proc_dointvec failed\n");
    return -1;
  }

  if(write) {
    pr_debug("writing %s\n", ctl->procname);
    write_mmio_u32(spi_lockdown_data.spibar + offset, *reg);
  }

  read_mmio_u32(spi_lockdown_data.spibar + offset, reg);

  return ret;
}

static int frap_sysctl_handler(struct ctl_table *ctl, int write,
    void __user *buffer, size_t *lenp, loff_t *ppos)
{
  int ret;

  read_mmio_u32(spi_lockdown_data.spibar + SPIBASE_LPT_FRAP_OFFSET,
      &spi_lockdown_data.frap.regval);

  ret = proc_dointvec(ctl, write, buffer, lenp, ppos);

  if(ret){
    printk(KERN_ERR "proc_dointvec failed\n");
    return -1;
  }

  if(write) {
    write_mmio_u32(spi_lockdown_data.spibar + SPIBASE_LPT_FRAP_OFFSET,
        spi_lockdown_data.frap.regval);
  }

  read_mmio_u32(spi_lockdown_data.spibar + SPIBASE_LPT_FRAP_OFFSET,
      &spi_lockdown_data.frap.regval);

  return ret;
}

static int flockdn_sysctl_handler(struct ctl_table *ctl, int write,
    void __user *buffer, size_t *lenp, loff_t *ppos)
{
  int ret;

  read_mmio_u16(spi_lockdown_data.spibar + SPIBASE_LPT_HSFS_OFFSET,
      &spi_lockdown_data.hsfsts.regval);
  ret = proc_dointvec(ctl, write, buffer, lenp, ppos);

  if(ret){
    printk(KERN_ERR "proc_dointvec failed\n");
    return -1;
  }

  if(write) {
    pr_debug("setting FLOCKDN\n");

    if(!spi_lockdown_data.hsfsts.bits.flockdn){
      printk(KERN_ERR "you can't disable FLOCKDN once it is enabled\n");
      return -1;
    }

    if(!spi_lockdown_data.spibar){
      printk(KERN_ERR "Cannot set FLOCKDN because we were unable to "
          "locate SPIBAR\n");
      return -1;
    }

    printk(KERN_INFO "spi_lockdown writing FLOCKDN");

    read_mmio_u16(spi_lockdown_data.spibar + SPIBASE_LPT_HSFS_OFFSET,
        &spi_lockdown_data.hsfsts.regval);

    spi_lockdown_data.hsfsts.bits.flockdn = 1;

    write_mmio_u16(spi_lockdown_data.spibar + SPIBASE_LPT_HSFS_OFFSET,
        spi_lockdown_data.hsfsts.regval);
    read_mmio_u16(spi_lockdown_data.spibar + SPIBASE_LPT_HSFS_OFFSET,
        &spi_lockdown_data.hsfsts.regval);

    if(spi_lockdown_data.hsfsts.bits.flockdn != 1){
      return -1;
    }
  }

  return ret;
}

static int spi_lockdown_init(void){
  int i;
  printk(KERN_INFO "spi_lockdown loading\n");

  memset(&spi_lockdown_data, '\0', sizeof(spi_lockdown_data));


  spi_lockdown_ctl_table_header = register_sysctl_table(
      spi_lockdown_root_table);

  if(spi_lockdown_ctl_table_header < 0){
    printk(KERN_ERR "Failed to install sysctl handler\n");
    return -1;
  }


  for(i = 0; i < sizeof(lpc_ich_ids) / sizeof(struct pci_device_id); i++){
    while (( spi_lockdown_data.ich_dev = pci_get_device(lpc_ich_ids[i].vendor,
            lpc_ich_ids[i].device, spi_lockdown_data.ich_dev)))
    {
      spi_lockdown_data.ich_priv = pci_get_drvdata(spi_lockdown_data.ich_dev);

      if(!spi_lockdown_data.ich_priv){
        printk(KERN_ERR "Failed pci_get_drvdata on vendor: %d, device: %d\n",
             spi_lockdown_data.ich_dev->vendor,  spi_lockdown_data.ich_dev->device);
        continue;
      }

      switch(lpc_chipset_info[spi_lockdown_data.ich_priv->chipset].spi_type){
        case INTEL_SPI_LPT:
          pr_debug("Got vendor: %d, device: %d\n",
              spi_lockdown_data.ich_dev->vendor, spi_lockdown_data.ich_dev->device);

          if(pci_read_config_dword(spi_lockdown_data.ich_dev, RCBA_OFFSET,
                &spi_lockdown_data.rcba) || !spi_lockdown_data.rcba)
          {
            pr_debug("Failed to read RCBA\n");
            break;
          }

          pr_debug("RCBA base: 0x%.8x\n", spi_lockdown_data.rcba);
          spi_lockdown_data.spibar = round_down(spi_lockdown_data.rcba,
              SPIBASE_LPT_SZ) + SPIBASE_LPT;

          pr_debug("SPI base: 0x%.8x\n", spi_lockdown_data.spibar);
          read_mmio_u16(spi_lockdown_data.spibar + SPIBASE_LPT_HSFS_OFFSET, &spi_lockdown_data.hsfsts.regval);

          read_mmio_u32(spi_lockdown_data.spibar + SPIBASE_LPT_FRAP_OFFSET,
              &spi_lockdown_data.frap.regval);

          read_mmio_u32(spi_lockdown_data.spibar + SPIBASE_LPT_PR0_OFFSET,
              &spi_lockdown_data.pr[0].regval);
          read_mmio_u32(spi_lockdown_data.spibar + SPIBASE_LPT_PR1_OFFSET,
              &spi_lockdown_data.pr[1].regval);
          read_mmio_u32(spi_lockdown_data.spibar + SPIBASE_LPT_PR2_OFFSET,
              &spi_lockdown_data.pr[2].regval);
          read_mmio_u32(spi_lockdown_data.spibar + SPIBASE_LPT_PR3_OFFSET,
              &spi_lockdown_data.pr[3].regval);
          read_mmio_u32(spi_lockdown_data.spibar + SPIBASE_LPT_PR4_OFFSET,
              &spi_lockdown_data.pr[4].regval);

          goto initialized_success;

        case INTEL_SPI_BYT:
        case INTEL_SPI_BXT:
        default:
          printk(
            KERN_ERR
            "Unsupported ICH detected. (CHIPSET: %.2x, SPI_TYPE: %.2x)\n",
            spi_lockdown_data.ich_priv->chipset,
            lpc_chipset_info[spi_lockdown_data.ich_priv->chipset].spi_type
          );
          break;
      }
    }
  }

  unregister_sysctl_table(spi_lockdown_ctl_table_header);
  return -ENXIO;

initialized_success:
  return 0;
}

static void spi_lockdown_exit(void){
  printk(KERN_INFO "spi_lockdown unloading\n");

  unregister_sysctl_table(spi_lockdown_ctl_table_header);
}

module_init(spi_lockdown_init);
module_exit(spi_lockdown_exit);
