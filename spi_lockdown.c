#include <linux/pci.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sysctl.h>
#include <linux/string.h>

#include "spi_lockdown.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("burrito");
MODULE_DESCRIPTION("Creates sysctl interface to make SPI protected range "
    "(and other) registers immutable");
MODULE_VERSION("0.1");

static u32 flockdn_flag = 0;
static u32 pr0_value = 0;
static u32 pr1_value = 0;
static u32 pr2_value = 0;
static u32 pr3_value = 0;
static u32 pr4_value = 0;
static u32 frap_value = 0;

static union ich_hws_flash_status hsfsts;
static u32 spi_base = 0;

static struct ctl_table spi_lockdown_table[] = {
  {
    .procname = "flockdn",
    .data = &flockdn_flag,
    .maxlen = sizeof(int),
    .mode = 0644,
    .proc_handler = &flockdn_sysctl_handler,
  },
  {
    .procname = "pr0",
    .data = &pr0_value,
    .maxlen = sizeof(int),
    .mode = 0644,
    .proc_handler = &pr_sysctl_handler,
  },
  {
    .procname = "pr1",
    .data = &pr1_value,
    .maxlen = sizeof(int),
    .mode = 0644,
    .proc_handler = &pr_sysctl_handler,
  },
  {
    .procname = "pr2",
    .data = &pr2_value,
    .maxlen = sizeof(int),
    .mode = 0644,
    .proc_handler = &pr_sysctl_handler,
  },
  {
    .procname = "pr3",
    .data = &pr3_value,
    .maxlen = sizeof(int),
    .mode = 0644,
    .proc_handler = &pr_sysctl_handler,
  },
  {
    .procname = "pr4",
    .data = &pr4_value,
    .maxlen = sizeof(int),
    .mode = 0644,
    .proc_handler = &pr_sysctl_handler,
  },
  {
    .procname = "frap",
    .data = &frap_value,
    .maxlen = sizeof(int),
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

  printk(KERN_DEBUG "reading 0x%.8x\n", addr);

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

  printk(KERN_DEBUG "writing %d to 0x%.8x\n", value, addr);

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

  printk(KERN_DEBUG "reading 0x%.8x\n", addr);

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

  printk(KERN_DEBUG "writing %d to 0x%.4x\n", value, addr);

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
    reg = &pr0_value;
  } else if(!strcmp(ctl->procname, "pr1")){
    offset = SPIBASE_LPT_PR1_OFFSET;
    reg = &pr1_value;
  } else if(!strcmp(ctl->procname, "pr2")){
    offset = SPIBASE_LPT_PR2_OFFSET;
    reg = &pr2_value;
  } else if(!strcmp(ctl->procname, "pr3")){
    offset = SPIBASE_LPT_PR3_OFFSET;
    reg = &pr3_value;
  } else if(!strcmp(ctl->procname, "pr4")){
    offset = SPIBASE_LPT_PR4_OFFSET;
    reg = &pr4_value;
  }

  if(!offset || !reg){
    return -1;
  }

  read_mmio_u32(spi_base + offset, reg);

  ret = proc_dointvec(ctl, write, buffer, lenp, ppos);

  if(ret){
    printk(KERN_ERR "proc_dointvec failed\n");
    return -1;
  }

  if(write) {
    printk(KERN_DEBUG "writing %s\n", ctl->procname);
    write_mmio_u32(spi_base + offset, *reg);
  }

  read_mmio_u32(spi_base + offset, reg);

  return ret;
}

static int frap_sysctl_handler(struct ctl_table *ctl, int write,
    void __user *buffer, size_t *lenp, loff_t *ppos)
{
  int ret;

  read_mmio_u32(spi_base + SPIBASE_LPT_FRAP_OFFSET, &frap_value);
  ret = proc_dointvec(ctl, write, buffer, lenp, ppos);

  if(ret){
    printk(KERN_ERR "proc_dointvec failed\n");
    return -1;
  }

  if(write) {
    write_mmio_u32(spi_base + SPIBASE_LPT_FRAP_OFFSET, frap_value);
  }

  read_mmio_u32(spi_base + SPIBASE_LPT_FRAP_OFFSET, &frap_value);

  return ret;
}

static int flockdn_sysctl_handler(struct ctl_table *ctl, int write,
    void __user *buffer, size_t *lenp, loff_t *ppos)
{
  int ret;

  read_mmio_u16(spi_base + SPIBASE_LPT_HSFS_OFFSET, &hsfsts.regval);
  ret = proc_dointvec(ctl, write, buffer, lenp, ppos);

  if(ret){
    printk(KERN_ERR "proc_dointvec failed\n");
    return -1;
  }

  if(write) {
    printk(KERN_DEBUG "setting FLOCKDN\n");

    if(!flockdn_flag){
      printk(KERN_ERR "you can't disable FLOCKDN once it is enabled\n");
      flockdn_flag = 1;
      return -1;
    }

    if(!spi_base){
      printk(KERN_ERR "Cannot set FLOCKDN because we were unable to "
          "locate SPIBAR\n");
      flockdn_flag = 0;
      return -1;
    }

    printk(KERN_INFO "spi_lockdown writing FLOCKDN");
    read_mmio_u16(spi_base + SPIBASE_LPT_HSFS_OFFSET, &hsfsts.regval);
    hsfsts.hsf_status.flockdn = 1;
    write_mmio_u16(spi_base + SPIBASE_LPT_HSFS_OFFSET, hsfsts.regval);
    read_mmio_u16(spi_base + SPIBASE_LPT_HSFS_OFFSET, &hsfsts.regval);

    if(hsfsts.hsf_status.flockdn != 1){
      return -1;
    }
  }

  return ret;
}

static int spi_lockdown_init(void){
  int i;

  printk(KERN_INFO "spi_lockdown loading\n");

  spi_lockdown_ctl_table_header = register_sysctl_table(
      spi_lockdown_root_table);

  if(spi_lockdown_ctl_table_header < 0){
    printk(KERN_ERR "Failed to install sysctl handler\n");
    return -1;
  }


  for(i = 0; i < sizeof(lpc_ich_ids) / sizeof(struct pci_device_id); i++){
    struct pci_dev *dev = NULL;

    while ((dev = pci_get_device(lpc_ich_ids[i].vendor,
            lpc_ich_ids[i].device, dev))){
      u32 rcba, bcr;
      struct lpc_ich_priv *priv;

      priv = pci_get_drvdata(dev);

      if(!priv){
        printk(KERN_ERR "Failed pci_get_drvdata on vendor: %d, device: %d\n",
            dev->vendor, dev->device);
        continue;
      }

      switch(lpc_chipset_info[priv->chipset].spi_type){
        case INTEL_SPI_LPT:
          spi_base = rcba = bcr = 0;

          printk(KERN_DEBUG "Got vendor: %d, device: %d\n", dev->vendor,
              dev->device);

          if(pci_read_config_dword(dev, RCBABASE, &rcba) || !rcba){
            printk(KERN_DEBUG "Failed to read RCBA\n");
            break;
          }

          printk(KERN_DEBUG "RCBA base: 0x%.8x\n", rcba);
          spi_base = round_down(rcba, SPIBASE_LPT_SZ) + SPIBASE_LPT;
          printk(KERN_DEBUG "SPI base: 0x%.8x\n", spi_base);
          read_mmio_u16(spi_base + SPIBASE_LPT_HSFS_OFFSET, &hsfsts.regval);
          flockdn_flag = hsfsts.hsf_status.flockdn;

          read_mmio_u32(spi_base + SPIBASE_LPT_FRAP_OFFSET, &frap_value);

          read_mmio_u32(spi_base + SPIBASE_LPT_PR0_OFFSET, &pr0_value);
          read_mmio_u32(spi_base + SPIBASE_LPT_PR1_OFFSET, &pr1_value);
          read_mmio_u32(spi_base + SPIBASE_LPT_PR2_OFFSET, &pr2_value);
          read_mmio_u32(spi_base + SPIBASE_LPT_PR3_OFFSET, &pr3_value);
          read_mmio_u32(spi_base + SPIBASE_LPT_PR4_OFFSET, &pr4_value);

          return 0;
          break; /* unreachable */

        case INTEL_SPI_BYT:
          if(pci_read_config_dword(dev, SPIBASE_BYT, &spi_base) || !spi_base){
            printk(KERN_DEBUG "Failed to read SPIBAR\n");
            break;
          }

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

static void spi_lockdown_exit(void){
  printk(KERN_INFO "spi_lockdown unloading\n");

  unregister_sysctl_table(spi_lockdown_ctl_table_header);
}

module_init(spi_lockdown_init);
module_exit(spi_lockdown_exit);
