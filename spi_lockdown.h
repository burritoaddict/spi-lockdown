#define SPIBASE_LPT 0x3800
#define SPIBASE_LPT_SZ 512
#define SPIBASE_LPT_HSFS_OFFSET 0x4

#define SPIBASE_BYT		0x54
#define SPIBASE_BYT_SZ		512
#define SPIBASE_BYT_EN		BIT(1)

#define RCBABASE 0xf0

union ich_hws_flash_status {
  struct ich8_hsfsts {
      u16 flcdone:1;/* bit 0 Flash Cycle Done */
      u16 flcerr:1;/* bit 1             Flash Cycle Error */
      u16 dael:1;/* bit 2 Direct Access error Log */
            u16 berasesz:2;/* bit 4:3 Sector Erase Size */
            u16 flcinprog:1;/*bit bit 5 flash cycle in Progress */
            u16 reserved1:2;/* bit 13:6 Reserved */
            u16 reserved2:6;/* bit 13:6 Reserved */
            u16 fldesvalid:1;/*reserved2 bit 14 Flash Descriptor Valid */
            u16 flockdn:1;/* bit 15 Flash Config Lock-Down */
  } hsf_status;
  u16 regval;
};

struct lpc_ich_priv {
	int chipset;

	int abase;		/* ACPI base */
	int actrl_pbase;	/* ACPI control or PMC base */
	int gbase;		/* GPIO base */
	int gctrl;		/* GPIO control */

	int abase_save;		/* Cached ACPI base value */
	int actrl_pbase_save;		/* Cached ACPI control or PMC base value */
	int gctrl_save;		/* Cached GPIO control value */
};

/* chipset related info */
enum lpc_chipsets {
	LPC_ICH = 0,	/* ICH */
	LPC_ICH0,	/* ICH0 */
	LPC_ICH2,	/* ICH2 */
	LPC_ICH2M,	/* ICH2-M */
	LPC_ICH3,	/* ICH3-S */
	LPC_ICH3M,	/* ICH3-M */
	LPC_ICH4,	/* ICH4 */
	LPC_ICH4M,	/* ICH4-M */
	LPC_CICH,	/* C-ICH */
	LPC_ICH5,	/* ICH5 & ICH5R */
	LPC_6300ESB,	/* 6300ESB */
	LPC_ICH6,	/* ICH6 & ICH6R */
	LPC_ICH6M,	/* ICH6-M */
	LPC_ICH6W,	/* ICH6W & ICH6RW */
	LPC_631XESB,	/* 631xESB/632xESB */
	LPC_ICH7,	/* ICH7 & ICH7R */
	LPC_ICH7DH,	/* ICH7DH */
	LPC_ICH7M,	/* ICH7-M & ICH7-U */
	LPC_ICH7MDH,	/* ICH7-M DH */
	LPC_NM10,	/* NM10 */
	LPC_ICH8,	/* ICH8 & ICH8R */
	LPC_ICH8DH,	/* ICH8DH */
	LPC_ICH8DO,	/* ICH8DO */
	LPC_ICH8M,	/* ICH8M */
	LPC_ICH8ME,	/* ICH8M-E */
	LPC_ICH9,	/* ICH9 */
	LPC_ICH9R,	/* ICH9R */
	LPC_ICH9DH,	/* ICH9DH */
	LPC_ICH9DO,	/* ICH9DO */
	LPC_ICH9M,	/* ICH9M */
	LPC_ICH9ME,	/* ICH9M-E */
	LPC_ICH10,	/* ICH10 */
	LPC_ICH10R,	/* ICH10R */
	LPC_ICH10D,	/* ICH10D */
	LPC_ICH10DO,	/* ICH10DO */
	LPC_PCH,	/* PCH Desktop Full Featured */
	LPC_PCHM,	/* PCH Mobile Full Featured */
	LPC_P55,	/* P55 */
	LPC_PM55,	/* PM55 */
	LPC_H55,	/* H55 */
	LPC_QM57,	/* QM57 */
	LPC_H57,	/* H57 */
	LPC_HM55,	/* HM55 */
	LPC_Q57,	/* Q57 */
	LPC_HM57,	/* HM57 */
	LPC_PCHMSFF,	/* PCH Mobile SFF Full Featured */
	LPC_QS57,	/* QS57 */
	LPC_3400,	/* 3400 */
	LPC_3420,	/* 3420 */
	LPC_3450,	/* 3450 */
	LPC_EP80579,	/* EP80579 */
	LPC_CPT,	/* Cougar Point */
	LPC_CPTD,	/* Cougar Point Desktop */
	LPC_CPTM,	/* Cougar Point Mobile */
	LPC_PBG,	/* Patsburg */
	LPC_DH89XXCC,	/* DH89xxCC */
	LPC_PPT,	/* Panther Point */
	LPC_LPT,	/* Lynx Point */
	LPC_LPT_LP,	/* Lynx Point-LP */
	LPC_WBG,	/* Wellsburg */
	LPC_AVN,	/* Avoton SoC */
	LPC_BAYTRAIL,   /* Bay Trail SoC */
	LPC_COLETO,	/* Coleto Creek */
	LPC_WPT_LP,	/* Wildcat Point-LP */
	LPC_BRASWELL,	/* Braswell SoC */
	LPC_LEWISBURG,	/* Lewisburg */
	LPC_9S,		/* 9 Series */
	LPC_APL,	/* Apollo Lake SoC */
	LPC_GLK,	/* Gemini Lake SoC */
	LPC_COUGARMOUNTAIN,/* Cougar Mountain SoC*/
};

enum {
	ICH_I3100_GPIO,
	ICH_V5_GPIO,
	ICH_V6_GPIO,
	ICH_V7_GPIO,
	ICH_V9_GPIO,
	ICH_V10CORP_GPIO,
	ICH_V10CONS_GPIO,
	AVOTON_GPIO,
};

enum intel_spi_type {
  INTEL_SPI_BYT = 1,
  INTEL_SPI_LPT,
  INTEL_SPI_BXT,
};

struct lpc_ich_info {
  char name[32];
  unsigned int iTCO_version;
  unsigned int gpio_version;
  enum intel_spi_type spi_type;
  u8 use_gpio;
};

static struct lpc_ich_info lpc_chipset_info[] = {
	[LPC_ICH] = {
		.name = "ICH",
		.iTCO_version = 1,
	},
	[LPC_ICH0] = {
		.name = "ICH0",
		.iTCO_version = 1,
	},
	[LPC_ICH2] = {
		.name = "ICH2",
		.iTCO_version = 1,
	},
	[LPC_ICH2M] = {
		.name = "ICH2-M",
		.iTCO_version = 1,
	},
	[LPC_ICH3] = {
		.name = "ICH3-S",
		.iTCO_version = 1,
	},
	[LPC_ICH3M] = {
		.name = "ICH3-M",
		.iTCO_version = 1,
	},
	[LPC_ICH4] = {
		.name = "ICH4",
		.iTCO_version = 1,
	},
	[LPC_ICH4M] = {
		.name = "ICH4-M",
		.iTCO_version = 1,
	},
	[LPC_CICH] = {
		.name = "C-ICH",
		.iTCO_version = 1,
	},
	[LPC_ICH5] = {
		.name = "ICH5 or ICH5R",
		.iTCO_version = 1,
	},
	[LPC_6300ESB] = {
		.name = "6300ESB",
		.iTCO_version = 1,
	},
	[LPC_ICH6] = {
		.name = "ICH6 or ICH6R",
		.iTCO_version = 2,
		.gpio_version = ICH_V6_GPIO,
	},
	[LPC_ICH6M] = {
		.name = "ICH6-M",
		.iTCO_version = 2,
		.gpio_version = ICH_V6_GPIO,
	},
	[LPC_ICH6W] = {
		.name = "ICH6W or ICH6RW",
		.iTCO_version = 2,
		.gpio_version = ICH_V6_GPIO,
	},
	[LPC_631XESB] = {
		.name = "631xESB/632xESB",
		.iTCO_version = 2,
		.gpio_version = ICH_V6_GPIO,
	},
	[LPC_ICH7] = {
		.name = "ICH7 or ICH7R",
		.iTCO_version = 2,
		.gpio_version = ICH_V7_GPIO,
	},
	[LPC_ICH7DH] = {
		.name = "ICH7DH",
		.iTCO_version = 2,
		.gpio_version = ICH_V7_GPIO,
	},
	[LPC_ICH7M] = {
		.name = "ICH7-M or ICH7-U",
		.iTCO_version = 2,
		.gpio_version = ICH_V7_GPIO,
	},
	[LPC_ICH7MDH] = {
		.name = "ICH7-M DH",
		.iTCO_version = 2,
		.gpio_version = ICH_V7_GPIO,
	},
	[LPC_NM10] = {
		.name = "NM10",
		.iTCO_version = 2,
		.gpio_version = ICH_V7_GPIO,
	},
	[LPC_ICH8] = {
		.name = "ICH8 or ICH8R",
		.iTCO_version = 2,
		.gpio_version = ICH_V7_GPIO,
	},
	[LPC_ICH8DH] = {
		.name = "ICH8DH",
		.iTCO_version = 2,
		.gpio_version = ICH_V7_GPIO,
	},
	[LPC_ICH8DO] = {
		.name = "ICH8DO",
		.iTCO_version = 2,
		.gpio_version = ICH_V7_GPIO,
	},
	[LPC_ICH8M] = {
		.name = "ICH8M",
		.iTCO_version = 2,
		.gpio_version = ICH_V7_GPIO,
	},
	[LPC_ICH8ME] = {
		.name = "ICH8M-E",
		.iTCO_version = 2,
		.gpio_version = ICH_V7_GPIO,
	},
	[LPC_ICH9] = {
		.name = "ICH9",
		.iTCO_version = 2,
		.gpio_version = ICH_V9_GPIO,
	},
	[LPC_ICH9R] = {
		.name = "ICH9R",
		.iTCO_version = 2,
		.gpio_version = ICH_V9_GPIO,
	},
	[LPC_ICH9DH] = {
		.name = "ICH9DH",
		.iTCO_version = 2,
		.gpio_version = ICH_V9_GPIO,
	},
	[LPC_ICH9DO] = {
		.name = "ICH9DO",
		.iTCO_version = 2,
		.gpio_version = ICH_V9_GPIO,
	},
	[LPC_ICH9M] = {
		.name = "ICH9M",
		.iTCO_version = 2,
		.gpio_version = ICH_V9_GPIO,
	},
	[LPC_ICH9ME] = {
		.name = "ICH9M-E",
		.iTCO_version = 2,
		.gpio_version = ICH_V9_GPIO,
	},
	[LPC_ICH10] = {
		.name = "ICH10",
		.iTCO_version = 2,
		.gpio_version = ICH_V10CONS_GPIO,
	},
	[LPC_ICH10R] = {
		.name = "ICH10R",
		.iTCO_version = 2,
		.gpio_version = ICH_V10CONS_GPIO,
    .spi_type = INTEL_SPI_LPT,
	},
	[LPC_ICH10D] = {
		.name = "ICH10D",
		.iTCO_version = 2,
		.gpio_version = ICH_V10CORP_GPIO,
	},
	[LPC_ICH10DO] = {
		.name = "ICH10DO",
		.iTCO_version = 2,
		.gpio_version = ICH_V10CORP_GPIO,
	},
	[LPC_PCH] = {
		.name = "PCH Desktop Full Featured",
		.iTCO_version = 2,
		.gpio_version = ICH_V5_GPIO,
	},
	[LPC_PCHM] = {
		.name = "PCH Mobile Full Featured",
		.iTCO_version = 2,
		.gpio_version = ICH_V5_GPIO,
	},
	[LPC_P55] = {
		.name = "P55",
		.iTCO_version = 2,
		.gpio_version = ICH_V5_GPIO,
	},
	[LPC_PM55] = {
		.name = "PM55",
		.iTCO_version = 2,
		.gpio_version = ICH_V5_GPIO,
	},
	[LPC_H55] = {
		.name = "H55",
		.iTCO_version = 2,
		.gpio_version = ICH_V5_GPIO,
	},
	[LPC_QM57] = {
		.name = "QM57",
		.iTCO_version = 2,
		.gpio_version = ICH_V5_GPIO,
	},
	[LPC_H57] = {
		.name = "H57",
		.iTCO_version = 2,
		.gpio_version = ICH_V5_GPIO,
	},
	[LPC_HM55] = {
		.name = "HM55",
		.iTCO_version = 2,
		.gpio_version = ICH_V5_GPIO,
	},
	[LPC_Q57] = {
		.name = "Q57",
		.iTCO_version = 2,
		.gpio_version = ICH_V5_GPIO,
	},
	[LPC_HM57] = {
		.name = "HM57",
		.iTCO_version = 2,
		.gpio_version = ICH_V5_GPIO,
	},
	[LPC_PCHMSFF] = {
		.name = "PCH Mobile SFF Full Featured",
		.iTCO_version = 2,
		.gpio_version = ICH_V5_GPIO,
	},
	[LPC_QS57] = {
		.name = "QS57",
		.iTCO_version = 2,
		.gpio_version = ICH_V5_GPIO,
	},
	[LPC_3400] = {
		.name = "3400",
		.iTCO_version = 2,
		.gpio_version = ICH_V5_GPIO,
	},
	[LPC_3420] = {
		.name = "3420",
		.iTCO_version = 2,
		.gpio_version = ICH_V5_GPIO,
	},
	[LPC_3450] = {
		.name = "3450",
		.iTCO_version = 2,
		.gpio_version = ICH_V5_GPIO,
	},
	[LPC_EP80579] = {
		.name = "EP80579",
		.iTCO_version = 2,
	},
	[LPC_CPT] = {
		.name = "Cougar Point",
		.iTCO_version = 2,
		.gpio_version = ICH_V5_GPIO,
	},
	[LPC_CPTD] = {
		.name = "Cougar Point Desktop",
		.iTCO_version = 2,
		.gpio_version = ICH_V5_GPIO,
	},
	[LPC_CPTM] = {
		.name = "Cougar Point Mobile",
		.iTCO_version = 2,
		.gpio_version = ICH_V5_GPIO,
	},
	[LPC_PBG] = {
		.name = "Patsburg",
		.iTCO_version = 2,
	},
	[LPC_DH89XXCC] = {
		.name = "DH89xxCC",
		.iTCO_version = 2,
	},
	[LPC_PPT] = {
		.name = "Panther Point",
		.iTCO_version = 2,
		.gpio_version = ICH_V5_GPIO,
	},
	[LPC_LPT] = {
		.name = "Lynx Point",
		.iTCO_version = 2,
		.gpio_version = ICH_V5_GPIO,
		.spi_type = INTEL_SPI_LPT,
	},
	[LPC_LPT_LP] = {
		.name = "Lynx Point_LP",
		.iTCO_version = 2,
		.spi_type = INTEL_SPI_LPT,
	},
	[LPC_WBG] = {
		.name = "Wellsburg",
		.iTCO_version = 2,
    .spi_type = INTEL_SPI_LPT,
	},
	[LPC_AVN] = {
		.name = "Avoton SoC",
		.iTCO_version = 3,
		.gpio_version = AVOTON_GPIO,
		.spi_type = INTEL_SPI_BYT,
	},
	[LPC_BAYTRAIL] = {
		.name = "Bay Trail SoC",
		.iTCO_version = 3,
		.spi_type = INTEL_SPI_BYT,
	},
	[LPC_COLETO] = {
		.name = "Coleto Creek",
		.iTCO_version = 2,
	},
	[LPC_WPT_LP] = {
		.name = "Wildcat Point_LP",
		.iTCO_version = 2,
		.spi_type = INTEL_SPI_LPT,
	},
	[LPC_BRASWELL] = {
		.name = "Braswell SoC",
		.iTCO_version = 3,
		.spi_type = INTEL_SPI_BYT,
	},
	[LPC_LEWISBURG] = {
		.name = "Lewisburg",
		.iTCO_version = 2,
	},
	[LPC_9S] = {
		.name = "9 Series",
		.iTCO_version = 2,
		.gpio_version = ICH_V5_GPIO,
	},
	[LPC_APL] = {
		.name = "Apollo Lake SoC",
		.iTCO_version = 5,
		.spi_type = INTEL_SPI_BXT,
	},
	[LPC_GLK] = {
		.name = "Gemini Lake SoC",
		.spi_type = INTEL_SPI_BXT,
	},
	[LPC_COUGARMOUNTAIN] = {
		.name = "Cougar Mountain SoC",
		.iTCO_version = 3,
	},
};

static const struct pci_device_id lpc_ich_ids[] = {
	{ PCI_VDEVICE(INTEL, 0x0f1c), LPC_BAYTRAIL},
	{ PCI_VDEVICE(INTEL, 0x1c41), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c42), LPC_CPTD},
	{ PCI_VDEVICE(INTEL, 0x1c43), LPC_CPTM},
	{ PCI_VDEVICE(INTEL, 0x1c44), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c45), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c46), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c47), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c48), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c49), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c4a), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c4b), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c4c), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c4d), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c4e), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c4f), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c50), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c51), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c52), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c53), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c54), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c55), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c56), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c57), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c58), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c59), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c5a), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c5b), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c5c), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c5d), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c5e), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1c5f), LPC_CPT},
	{ PCI_VDEVICE(INTEL, 0x1d40), LPC_PBG},
	{ PCI_VDEVICE(INTEL, 0x1d41), LPC_PBG},
	{ PCI_VDEVICE(INTEL, 0x1e40), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e41), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e42), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e43), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e44), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e45), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e46), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e47), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e48), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e49), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e4a), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e4b), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e4c), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e4d), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e4e), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e4f), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e50), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e51), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e52), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e53), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e54), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e55), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e56), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e57), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e58), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e59), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e5a), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e5b), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e5c), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e5d), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e5e), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1e5f), LPC_PPT},
	{ PCI_VDEVICE(INTEL, 0x1f38), LPC_AVN},
	{ PCI_VDEVICE(INTEL, 0x1f39), LPC_AVN},
	{ PCI_VDEVICE(INTEL, 0x1f3a), LPC_AVN},
	{ PCI_VDEVICE(INTEL, 0x1f3b), LPC_AVN},
	{ PCI_VDEVICE(INTEL, 0x229c), LPC_BRASWELL},
	{ PCI_VDEVICE(INTEL, 0x2310), LPC_DH89XXCC},
	{ PCI_VDEVICE(INTEL, 0x2390), LPC_COLETO},
	{ PCI_VDEVICE(INTEL, 0x2410), LPC_ICH},
	{ PCI_VDEVICE(INTEL, 0x2420), LPC_ICH0},
	{ PCI_VDEVICE(INTEL, 0x2440), LPC_ICH2},
	{ PCI_VDEVICE(INTEL, 0x244c), LPC_ICH2M},
	{ PCI_VDEVICE(INTEL, 0x2450), LPC_CICH},
	{ PCI_VDEVICE(INTEL, 0x2480), LPC_ICH3},
	{ PCI_VDEVICE(INTEL, 0x248c), LPC_ICH3M},
	{ PCI_VDEVICE(INTEL, 0x24c0), LPC_ICH4},
	{ PCI_VDEVICE(INTEL, 0x24cc), LPC_ICH4M},
	{ PCI_VDEVICE(INTEL, 0x24d0), LPC_ICH5},
	{ PCI_VDEVICE(INTEL, 0x25a1), LPC_6300ESB},
	{ PCI_VDEVICE(INTEL, 0x2640), LPC_ICH6},
	{ PCI_VDEVICE(INTEL, 0x2641), LPC_ICH6M},
	{ PCI_VDEVICE(INTEL, 0x2642), LPC_ICH6W},
	{ PCI_VDEVICE(INTEL, 0x2670), LPC_631XESB},
	{ PCI_VDEVICE(INTEL, 0x2671), LPC_631XESB},
	{ PCI_VDEVICE(INTEL, 0x2672), LPC_631XESB},
	{ PCI_VDEVICE(INTEL, 0x2673), LPC_631XESB},
	{ PCI_VDEVICE(INTEL, 0x2674), LPC_631XESB},
	{ PCI_VDEVICE(INTEL, 0x2675), LPC_631XESB},
	{ PCI_VDEVICE(INTEL, 0x2676), LPC_631XESB},
	{ PCI_VDEVICE(INTEL, 0x2677), LPC_631XESB},
	{ PCI_VDEVICE(INTEL, 0x2678), LPC_631XESB},
	{ PCI_VDEVICE(INTEL, 0x2679), LPC_631XESB},
	{ PCI_VDEVICE(INTEL, 0x267a), LPC_631XESB},
	{ PCI_VDEVICE(INTEL, 0x267b), LPC_631XESB},
	{ PCI_VDEVICE(INTEL, 0x267c), LPC_631XESB},
	{ PCI_VDEVICE(INTEL, 0x267d), LPC_631XESB},
	{ PCI_VDEVICE(INTEL, 0x267e), LPC_631XESB},
	{ PCI_VDEVICE(INTEL, 0x267f), LPC_631XESB},
	{ PCI_VDEVICE(INTEL, 0x27b0), LPC_ICH7DH},
	{ PCI_VDEVICE(INTEL, 0x27b8), LPC_ICH7},
	{ PCI_VDEVICE(INTEL, 0x27b9), LPC_ICH7M},
	{ PCI_VDEVICE(INTEL, 0x27bc), LPC_NM10},
	{ PCI_VDEVICE(INTEL, 0x27bd), LPC_ICH7MDH},
	{ PCI_VDEVICE(INTEL, 0x2810), LPC_ICH8},
	{ PCI_VDEVICE(INTEL, 0x2811), LPC_ICH8ME},
	{ PCI_VDEVICE(INTEL, 0x2812), LPC_ICH8DH},
	{ PCI_VDEVICE(INTEL, 0x2814), LPC_ICH8DO},
	{ PCI_VDEVICE(INTEL, 0x2815), LPC_ICH8M},
	{ PCI_VDEVICE(INTEL, 0x2912), LPC_ICH9DH},
	{ PCI_VDEVICE(INTEL, 0x2914), LPC_ICH9DO},
	{ PCI_VDEVICE(INTEL, 0x2916), LPC_ICH9R},
	{ PCI_VDEVICE(INTEL, 0x2917), LPC_ICH9ME},
	{ PCI_VDEVICE(INTEL, 0x2918), LPC_ICH9},
	{ PCI_VDEVICE(INTEL, 0x2919), LPC_ICH9M},
	{ PCI_VDEVICE(INTEL, 0x3197), LPC_GLK},
	{ PCI_VDEVICE(INTEL, 0x2b9c), LPC_COUGARMOUNTAIN},
	{ PCI_VDEVICE(INTEL, 0x3a14), LPC_ICH10DO},
	{ PCI_VDEVICE(INTEL, 0x3a16), LPC_ICH10R},
	{ PCI_VDEVICE(INTEL, 0x3a18), LPC_ICH10},
	{ PCI_VDEVICE(INTEL, 0x3a1a), LPC_ICH10D},
	{ PCI_VDEVICE(INTEL, 0x3b00), LPC_PCH},
	{ PCI_VDEVICE(INTEL, 0x3b01), LPC_PCHM},
	{ PCI_VDEVICE(INTEL, 0x3b02), LPC_P55},
	{ PCI_VDEVICE(INTEL, 0x3b03), LPC_PM55},
	{ PCI_VDEVICE(INTEL, 0x3b06), LPC_H55},
	{ PCI_VDEVICE(INTEL, 0x3b07), LPC_QM57},
	{ PCI_VDEVICE(INTEL, 0x3b08), LPC_H57},
	{ PCI_VDEVICE(INTEL, 0x3b09), LPC_HM55},
	{ PCI_VDEVICE(INTEL, 0x3b0a), LPC_Q57},
	{ PCI_VDEVICE(INTEL, 0x3b0b), LPC_HM57},
	{ PCI_VDEVICE(INTEL, 0x3b0d), LPC_PCHMSFF},
	{ PCI_VDEVICE(INTEL, 0x3b0f), LPC_QS57},
	{ PCI_VDEVICE(INTEL, 0x3b12), LPC_3400},
	{ PCI_VDEVICE(INTEL, 0x3b14), LPC_3420},
	{ PCI_VDEVICE(INTEL, 0x3b16), LPC_3450},
	{ PCI_VDEVICE(INTEL, 0x5031), LPC_EP80579},
	{ PCI_VDEVICE(INTEL, 0x5ae8), LPC_APL},
	{ PCI_VDEVICE(INTEL, 0x8c40), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c41), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c42), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c43), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c44), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c45), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c46), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c47), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c48), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c49), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c4a), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c4b), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c4c), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c4d), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c4e), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c4f), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c50), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c51), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c52), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c53), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c54), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c55), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c56), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c57), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c58), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c59), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c5a), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c5b), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c5c), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c5d), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c5e), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8c5f), LPC_LPT},
	{ PCI_VDEVICE(INTEL, 0x8cc1), LPC_9S},
	{ PCI_VDEVICE(INTEL, 0x8cc2), LPC_9S},
	{ PCI_VDEVICE(INTEL, 0x8cc3), LPC_9S},
	{ PCI_VDEVICE(INTEL, 0x8cc4), LPC_9S},
	{ PCI_VDEVICE(INTEL, 0x8cc6), LPC_9S},
	{ PCI_VDEVICE(INTEL, 0x8d40), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d41), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d42), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d43), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d44), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d45), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d46), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d47), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d48), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d49), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d4a), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d4b), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d4c), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d4d), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d4e), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d4f), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d50), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d51), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d52), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d53), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d54), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d55), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d56), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d57), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d58), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d59), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d5a), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d5b), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d5c), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d5d), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d5e), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x8d5f), LPC_WBG},
	{ PCI_VDEVICE(INTEL, 0x9c40), LPC_LPT_LP},
	{ PCI_VDEVICE(INTEL, 0x9c41), LPC_LPT_LP},
	{ PCI_VDEVICE(INTEL, 0x9c42), LPC_LPT_LP},
	{ PCI_VDEVICE(INTEL, 0x9c43), LPC_LPT_LP},
	{ PCI_VDEVICE(INTEL, 0x9c44), LPC_LPT_LP},
	{ PCI_VDEVICE(INTEL, 0x9c45), LPC_LPT_LP},
	{ PCI_VDEVICE(INTEL, 0x9c46), LPC_LPT_LP},
	{ PCI_VDEVICE(INTEL, 0x9c47), LPC_LPT_LP},
	{ PCI_VDEVICE(INTEL, 0x9cc1), LPC_WPT_LP},
	{ PCI_VDEVICE(INTEL, 0x9cc2), LPC_WPT_LP},
	{ PCI_VDEVICE(INTEL, 0x9cc3), LPC_WPT_LP},
	{ PCI_VDEVICE(INTEL, 0x9cc5), LPC_WPT_LP},
	{ PCI_VDEVICE(INTEL, 0x9cc6), LPC_WPT_LP},
	{ PCI_VDEVICE(INTEL, 0x9cc7), LPC_WPT_LP},
	{ PCI_VDEVICE(INTEL, 0x9cc9), LPC_WPT_LP},
	{ PCI_VDEVICE(INTEL, 0xa1c1), LPC_LEWISBURG},
	{ PCI_VDEVICE(INTEL, 0xa1c2), LPC_LEWISBURG},
	{ PCI_VDEVICE(INTEL, 0xa1c3), LPC_LEWISBURG},
	{ PCI_VDEVICE(INTEL, 0xa1c4), LPC_LEWISBURG},
	{ PCI_VDEVICE(INTEL, 0xa1c5), LPC_LEWISBURG},
	{ PCI_VDEVICE(INTEL, 0xa1c6), LPC_LEWISBURG},
	{ PCI_VDEVICE(INTEL, 0xa1c7), LPC_LEWISBURG},
	{ PCI_VDEVICE(INTEL, 0xa242), LPC_LEWISBURG},
	{ PCI_VDEVICE(INTEL, 0xa243), LPC_LEWISBURG},
	{ 0, },			/* End of list */
};

static int flockdn_sysctl_handler(struct ctl_table *ctl, int write,
    void __user *buffer, size_t *lenp, loff_t *ppos);
