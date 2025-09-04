# SPI Driver Configuration

This document helps you to customize SPI driver for your specific hardware setup.

## Pin Configuration

**Step 1**: locate the initialization function.
For example, the initialization function for standard SPI driver is located at `bl2/platform/common/spi_drv.c` and named `NVT_NVSPI_Init()`.

**Step 2**: configure the pin multiplexing for your target pins.
```
SET_SPI3_MOSI_PG8();
SET_SPI3_MISO_PG7();
SET_SPI3_CLK_PG6();
SET_SPI3_SS_PG5();
```

**Step 3**: enable the clock for GPIO and SPI module.
```
CLK_EnableModuleClock(GPG_MODULE);

CLK_EnableModuleClock(SPI3_MODULE);

CLK_SetModuleClock(SPI3_MODULE, CLK_CLKSEL3_SPI3SEL_PCLK0, MODULE_NoMsk);
```

**Step 4**: configure `SPI_FLASH_PORT` to correct SPI port at `bl2/platform/common/nvspi_drv.h`
```
#define SPI_FLASH_PORT          SPI3
```

**Step 5**: set GPIO properties
```
/* Enable SPI clock pin schmitt trigger */
PG->SMTEN |= GPIO_SMTEN_SMTEN6_Msk;

/* Enable SPI I/O high slew rate */
GPIO_SetSlewCtl(PG, BIT5 | BIT6 | BIT7 | BIT8, GPIO_SLEWCTL_HIGH);
```

## Using QSPI/SPIM

To use different SPI interfaces, enable the appropriate interface by setting the corresponding flag:

For QSPI: Set `USE_QSPI=1`
For SPIM: Set `USE_SPIM=1`

The configuration process remains similar across interfaces.

| Interface | Initialization Function Location |
| --------- | ----------------------------------- |
| Standard SPI | bl2/platform/common/spi_drv.c |
| QSPI | bl2/platform/common/qspi_drv.c |
| SPIM | bl2/platform/common/spim_drv.c |
