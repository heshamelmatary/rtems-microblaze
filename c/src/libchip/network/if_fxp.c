/*-
 * Copyright (c) 1995, David Greenman
 * Copyright (c) 2001 Jonathan Lemon <jlemon@freebsd.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice unmodified, this list of conditions, and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: src/sys/dev/fxp/if_fxp.c,v 1.118 2001/09/05 23:33:58 brooks Exp $
 */

/*
 * Intel EtherExpress Pro/100B PCI Fast Ethernet driver
 */

/*
 * RTEMS Revision Preliminary History
 *
 * July XXX, 2002     W. Eric Norum <eric.norum@usask.ca>
 *     Placed in RTEMS CVS repository.  All further modifications will be
 *     noted in the CVS log and not in this comment.
 *
 * July 11, 2002     W. Eric Norum <eric.norum@usask.ca>
 *     Minor modifications to get driver working with NIC on VersaLogic
 *     Bobcat PC-104 single-board computer.  The Bobcat has no video
 *     driver so printf/printk calls are directed to COM2:.  This
 *     arrangement seems to require delays after the printk calls or
 *     else things lock up.  Perhaps the RTEMS pc386 console code
 *     should be modified to insert these delays itself.
 *
 * June 27, 2002     W. Eric Norum <eric.norum@usask.ca>
 *     Obtained from Thomas Doerfler <Thomas.Doerfler@imd-systems.de>.
 *     A big thank-you to Thomas for making this available.
 *
 * October 01, 2001  Thomas Doerfler <Thomas.Doerfler@imd-systems.de>
 *     Original RTEMS modifications.
 */

#if defined(__i386__)

/*#define DEBUG_OUT 0*/

#include <rtems.h>
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>
#include <bsp.h>

#include <errno.h>
#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <sys/malloc.h>
#include <sys/systm.h>
#include <bsp.h>
#include <pcibios.h>
#include <bsp/irq.h>
#include <rtems/pci.h>

#ifdef NS
#include <netns/ns.h>
#include <netns/ns_if.h>
#endif

#include <net/bpf.h>

#include <vm/vm.h>		/* for vtophys */

#include <net/if_types.h>

#include "if_fxpreg.h"
#include "if_fxpvar.h"

/*
 * some adaptation replacements for RTEMS
 */
static rtems_interval fxp_ticksPerSecond;
#define device_printf(device,format,args...) printk(format,## args)
#define DELAY(n) rtems_task_wake_after(((n)*fxp_ticksPerSecond/1000000)+1)
#ifdef DEBUG_OUT
#define DBGLVL_PRINTK(LVL,format, args...)                   \
if (DEBUG_OUT >= (LVL)) {                                    \
  printk(format, ## args);                                   \
}
#else
#define DBGLVL_PRINTK(LVL,format, args...)
#endif

/*
 * RTEMS event used by interrupt handler to signal driver tasks.
 * This must not be any of the events used by the network task synchronization.
 */
#define INTERRUPT_EVENT	RTEMS_EVENT_1

/*
 * remapping between PCI device and CPU memmory address view...
 */
#if defined(__i386)
#define vtophys(p) (u_int32_t)(p)
#else
#define vtophys(p) vtophys(p)
#endif

#define NFXPDRIVER 1
static struct fxp_softc fxp_softc[NFXPDRIVER];
static bool fxp_is_verbose = true;
/*
 * NOTE!  On the Alpha, we have an alignment constraint.  The
 * card DMAs the packet immediately following the RFA.  However,
 * the first thing in the packet is a 14-byte Ethernet header.
 * This means that the packet is misaligned.  To compensate,
 * we actually offset the RFA 2 bytes into the cluster.  This
 * alignes the packet after the Ethernet header at a 32-bit
 * boundary.  HOWEVER!  This means that the RFA is misaligned!
 */
#define	RFA_ALIGNMENT_FUDGE	2

/*
 * Set initial transmit threshold at 64 (512 bytes). This is
 * increased by 64 (512 bytes) at a time, to maximum of 192
 * (1536 bytes), if an underrun occurs.
 */
static int tx_threshold = 64;

/*
 * The configuration byte map has several undefined fields which
 * must be one or must be zero.  Set up a template for these bits
 * only, (assuming a 82557 chip) leaving the actual configuration
 * to fxp_init.
 *
 * See struct fxp_cb_config for the bit definitions.
 */
static u_char fxp_cb_config_template[] = {
	0x0, 0x0,		/* cb_status */
	0x0, 0x0,		/* cb_command */
	0x0, 0x0, 0x0, 0x0,	/* link_addr */
	0x0,	/*  0 */
	0x0,	/*  1 */
	0x0,	/*  2 */
	0x0,	/*  3 */
	0x0,	/*  4 */
	0x0,	/*  5 */
	0x32,	/*  6 */
	0x0,	/*  7 */
	0x0,	/*  8 */
	0x0,	/*  9 */
	0x6,	/* 10 */
	0x0,	/* 11 */
	0x0,	/* 12 */
	0x0,	/* 13 */
	0xf2,	/* 14 */
	0x48,	/* 15 */
	0x0,	/* 16 */
	0x40,	/* 17 */
	0xf0,	/* 18 */
	0x0,	/* 19 */
	0x3f,	/* 20 */
	0x5	/* 21 */
};

struct fxp_ident {
	u_int16_t	devid;
	char 		*name;
	int			warn;
};

#define UNTESTED 1

/*
 * Claim various Intel PCI device identifiers for this driver.  The
 * sub-vendor and sub-device field are extensively used to identify
 * particular variants, but we don't currently differentiate between
 * them.
 */
static struct fxp_ident fxp_ident_table[] = {
    { 0x1229,		"Intel Pro 10/100B/100+ Ethernet", 0 },
    { 0x2449,		"Intel Pro/100 Ethernet", UNTESTED },
    { 0x1209,		"Intel Embedded 10/100 Ethernet", 0 },
    { 0x1029,		"Intel Pro/100 Ethernet", UNTESTED },
    { 0x1030,		"Intel Pro/100 Ethernet", 0 },
    { 0x1031,		"Intel Pro/100 Ethernet", UNTESTED },
    { 0x1032,		"Intel Pro/100 Ethernet", UNTESTED },
    { 0x1033,		"Intel Pro/100 Ethernet", UNTESTED },
    { 0x1034,		"Intel Pro/100 Ethernet", UNTESTED },
    { 0x1035,		"Intel Pro/100 Ethernet", UNTESTED },
    { 0x1036,		"Intel Pro/100 Ethernet", UNTESTED },
    { 0x1037,		"Intel Pro/100 Ethernet", UNTESTED },
    { 0x1038,		"Intel Pro/100 Ethernet", UNTESTED },
    { 0x103B,		"Intel Pro/100 Ethernet (82801BD PRO/100 VM (LOM))", 0 },
    { 0,		NULL, 0 }
};

#if 0
static int		fxp_probe(device_t dev);
static int		fxp_attach(device_t dev);
static int		fxp_detach(device_t dev);
static int		fxp_shutdown(device_t dev);
#endif
int	fxp_output (struct ifnet *,
	   struct mbuf *, struct sockaddr *, struct rtentry *);


static rtems_isr        fxp_intr(rtems_vector_number v);
static void 		fxp_init(void *xsc);
static void 		fxp_tick(void *xsc);
static void 		fxp_start(struct ifnet *ifp);
static void		fxp_stop(struct fxp_softc *sc);
static void 		fxp_release(struct fxp_softc *sc);
static int		fxp_ioctl(struct ifnet *ifp, ioctl_command_t command,
			    caddr_t data);
static void 		fxp_watchdog(struct ifnet *ifp);
static int		fxp_add_rfabuf(struct fxp_softc *sc, struct mbuf *oldm);
static void		fxp_mc_setup(struct fxp_softc *sc);
static u_int16_t	fxp_eeprom_getword(struct fxp_softc *sc, int offset,
			    int autosize);
static void 		fxp_eeprom_putword(struct fxp_softc *sc, int offset,
			    u_int16_t data);
static void		fxp_autosize_eeprom(struct fxp_softc *sc);
static void		fxp_read_eeprom(struct fxp_softc *sc, u_short *data,
			    int offset, int words);
static void		fxp_write_eeprom(struct fxp_softc *sc, u_short *data,
			    int offset, int words);
#ifdef NOTUSED
static int		fxp_ifmedia_upd(struct ifnet *ifp);
static void		fxp_ifmedia_sts(struct ifnet *ifp,
			    struct ifmediareq *ifmr);
static int		fxp_serial_ifmedia_upd(struct ifnet *ifp);
static void		fxp_serial_ifmedia_sts(struct ifnet *ifp,
			    struct ifmediareq *ifmr);
static volatile int	fxp_miibus_readreg(device_t dev, int phy, int reg);
static void		fxp_miibus_writereg(device_t dev, int phy, int reg,
			    int value);
#endif
static __inline void	fxp_lwcopy(volatile u_int32_t *src,
			    volatile u_int32_t *dst);
static __inline void 	fxp_scb_wait(struct fxp_softc *sc);
static __inline void	fxp_scb_cmd(struct fxp_softc *sc, int cmd);
static __inline void	fxp_dma_wait(volatile u_int16_t *status,
			    struct fxp_softc *sc);

/*
 * Inline function to copy a 16-bit aligned 32-bit quantity.
 */
static __inline void
fxp_lwcopy(volatile u_int32_t *src, volatile u_int32_t *dst)
{
#ifdef __i386__
	*dst = *src;
#else
	volatile u_int16_t *a = (volatile u_int16_t*)src;
	volatile u_int16_t *b = (volatile u_int16_t*)dst;

	b[0] = a[0];
	b[1] = a[1];
#endif
}

/*
 * inline access functions to pci space registers
 */
static __inline u_int8_t fxp_csr_read_1(struct fxp_softc *sc,int  reg) {
  u_int8_t val;
  if (sc->pci_regs_are_io) {
    inport_byte(sc->pci_regs_base + reg,val);
  }
  else {
    val = *(u_int8_t*)(sc->pci_regs_base+reg);
  }
  return val;
}
static __inline u_int32_t fxp_csr_read_2(struct fxp_softc *sc,int  reg) {
  u_int16_t val;
  if (sc->pci_regs_are_io) {
    inport_word(sc->pci_regs_base + reg,val);
  }
  else {
    val = *(u_int16_t*)(sc->pci_regs_base+reg);
  }
  return val;
}
static __inline u_int32_t fxp_csr_read_4(struct fxp_softc *sc,int  reg) {
  u_int32_t val;
  if (sc->pci_regs_are_io) {
    inport_long(sc->pci_regs_base + reg,val);
  }
  else {
    val = *(u_int32_t*)(sc->pci_regs_base+reg);
  }
  return val;
}

/*
 * Wait for the previous command to be accepted (but not necessarily
 * completed).
 */
static __inline void
fxp_scb_wait(struct fxp_softc *sc)
{
	int i = 10000;

	while (CSR_READ_1(sc, FXP_CSR_SCB_COMMAND) && --i)
		DELAY(2);
	if (i == 0)
		device_printf(sc->dev, "SCB timeout: 0x%x 0x%x 0x%x 0x%x\n",
		    CSR_READ_1(sc, FXP_CSR_SCB_COMMAND),
		    CSR_READ_1(sc, FXP_CSR_SCB_STATACK),
		    CSR_READ_1(sc, FXP_CSR_SCB_RUSCUS),
		    CSR_READ_2(sc, FXP_CSR_FLOWCONTROL));
}

static __inline void
fxp_scb_cmd(struct fxp_softc *sc, int cmd)
{

	if (cmd == FXP_SCB_COMMAND_CU_RESUME && sc->cu_resume_bug) {
		CSR_WRITE_1(sc, FXP_CSR_SCB_COMMAND, FXP_CB_COMMAND_NOP);
		fxp_scb_wait(sc);
	}
	CSR_WRITE_1(sc, FXP_CSR_SCB_COMMAND, cmd);
}

static __inline void
fxp_dma_wait(volatile u_int16_t *status, struct fxp_softc *sc)
{
	int i = 10000;

	while (!(*status & FXP_CB_STATUS_C) && --i)
		DELAY(2);
	if (i == 0)
		device_printf(sc->dev, "DMA timeout\n");
}

static __inline unsigned int pci_get_vendor(struct fxp_softc *sc) {
  u_int16_t vendor;
  pcib_conf_read16(sc->pci_signature,0,&vendor);
  return vendor;
}

static __inline unsigned int pci_get_device(struct fxp_softc *sc) {
  u_int16_t device;
  pcib_conf_read16(sc->pci_signature,2,&device);
  return device;
}

static __inline unsigned int pci_get_subvendor(struct fxp_softc *sc) {
  u_int16_t subvendor;
  pcib_conf_read16(sc->pci_signature,0x2c,&subvendor);
  return subvendor;
}

static __inline unsigned int pci_get_subdevice(struct fxp_softc *sc) {
  u_int16_t subdevice;
  pcib_conf_read16(sc->pci_signature,0x2e,&subdevice);
  return subdevice;
}

static __inline unsigned int pci_get_revid(struct fxp_softc *sc) {
  u_int8_t revid;
  pcib_conf_read8(sc->pci_signature,0x08,&revid);
  return revid;
}

static void nopOn(const rtems_irq_connect_data* notUsed)
{
  /*
   * code should be moved from fxp_Enet_initialize_hardware
   * to this location
   */
}

static int fxpIsOn(const rtems_irq_connect_data* irq)
{
  return BSP_irq_enabled_at_i8259s (irq->name);
}

int
rtems_fxp_attach(struct rtems_bsdnet_ifconfig *config, int attaching)
{
	int error = 0;
	struct fxp_softc *sc;
	struct ifnet *ifp;
	uint16_t val16;
	uint32_t val32;
	uint16_t data;
	int i;
	int s;
	int unitNumber;
	char *unitName;
	u_int16_t dev_id;
	u_int8_t interrupt;
	int mtu;

    /*
     * Set up some timing values
     */
    rtems_clock_get(RTEMS_CLOCK_GET_TICKS_PER_SECOND, &fxp_ticksPerSecond);
	DBGLVL_PRINTK(1,"fxp_attach called\n");

	/*
 	 * Parse driver name
	 */
	if ((unitNumber = rtems_bsdnet_parse_driver_name (config, &unitName)) < 0)
		return 0;

	/*
	 * Is driver free?
	 */
	if ((unitNumber <= 0) || (unitNumber > NFXPDRIVER)) {
		device_printf(dev,"Bad FXP unit number.\n");
		return 0;
	}
	sc = &fxp_softc[unitNumber - 1];
	ifp = &sc->arpcom.ac_if;
	if (ifp->if_softc != NULL) {
		device_printf(dev,"FXP Driver already in use.\n");
		return 0;
	}

	memset(sc, 0, sizeof(*sc));
#ifdef NOTUSED
	sc->dev = dev;
	callout_handle_init(&sc->stat_ch);
	mtx_init(&sc->sc_mtx, device_get_nameunit(dev), MTX_DEF | MTX_RECURSE);
#endif
	s = splimp();

	/*
	 * find device on pci bus
	 */
    { int j; int pbus, pdev, pfun;

      for (j=0; fxp_ident_table[j].devid; j++ ) {
		    i = pci_find_device( 0x8086, fxp_ident_table[j].devid,
			       unitNumber-1, &pbus, &pdev, &pfun );
 		    sc->pci_signature =  PCIB_DEVSIG_MAKE( pbus, pdev, pfun );
		    DBGLVL_PRINTK(2,"fxp_attach: find_devid returned %d "
		      "and pci signature 0x%x\n",
		      i,sc->pci_signature);
      	if (PCIB_ERR_SUCCESS == i) {
		    if ( UNTESTED == fxp_ident_table[j].warn ) {
		  	  device_printf(dev,
"WARNING: this chip version has NOT been reported to work under RTEMS yet.\n");
			    device_printf(dev,
"         If it works OK, report it as tested in 'c/src/libchip/network/if_fxp.c'\n");
			  }
  			break;
		  }
	  }
	}

	/*
	 * FIXME: add search for more device types...
	 */
	if (i != PCIB_ERR_SUCCESS) {
	  device_printf(dev, "could not find 82559ER device\n");
	  return 0;
	}


	/*
	 * Enable bus mastering. Enable memory space too, in case
	 * BIOS/Prom forgot about it.
	 */
	pcib_conf_read16(sc->pci_signature, PCI_COMMAND,&val16);
	val16 |= (PCI_COMMAND_MEMORY|PCI_COMMAND_MASTER);
	pcib_conf_write16(sc->pci_signature, PCI_COMMAND, val16);
	DBGLVL_PRINTK(3,"fxp_attach: PCI_COMMAND_write = 0x%x\n",val16);
	pcib_conf_read16(sc->pci_signature, PCI_COMMAND,&val16);
	DBGLVL_PRINTK(4,"fxp_attach: PCI_COMMAND_read  = 0x%x\n",val16);

	/*
	 * Figure out which we should try first - memory mapping or i/o mapping?
	 * We default to memory mapping. Then we accept an override from the
	 * command line. Then we check to see which one is enabled.
	 */
#ifdef NOTUSED
	m1 = PCI_COMMAND_MEMORY;
	m2 = PCI_COMMAND_IO;
	prefer_iomap = 0;
	if (resource_int_value(device_get_name(dev), device_get_unit(dev),
	    "prefer_iomap", &prefer_iomap) == 0 && prefer_iomap != 0) {
		m1 = PCI_COMMAND_IO;
		m2 = PCI_COMMAND_MEMORY;
	}

	if (val & m1) {
		sc->rtp = ((m1 == PCI_COMMAND_MEMORY)
			   ? SYS_RES_MEMORY : SYS_RES_IOPORT);
		sc->rgd = ((m1 == PCI_COMMAND_MEMORY)
			   ? FXP_PCI_MMBA   : FXP_PCI_IOBA);
		sc->mem = bus_alloc_resource(dev, sc->rtp, &sc->rgd,
	                                     0, ~0, 1, RF_ACTIVE);
	}
	if (sc->mem == NULL && (val & m2)) {
		sc->rtp = ((m2 == PCI_COMMAND_MEMORY)
			   ? SYS_RES_MEMORY : SYS_RES_IOPORT);
		sc->rgd = ((m2 == PCI_COMMAND_MEMORY)
			   ? FXP_PCI_MMBA : FXP_PCI_IOBA);
		sc->mem = bus_alloc_resource(dev, sc->rtp, &sc->rgd,
                                            0, ~0, 1, RF_ACTIVE);
	}

	if (!sc->mem) {
		device_printf(dev, "could not map device registers\n");
		error = ENXIO;
		goto fail;
        }
	if (fxp_is_verbose) {
		device_printf(dev, "using %s space register mapping\n",
		   sc->rtp == SYS_RES_MEMORY? "memory" : "I/O");
	}

	sc->sc_st = rman_get_bustag(sc->mem);
	sc->sc_sh = rman_get_bushandle(sc->mem);

	/*
	 * Allocate our interrupt.
	 */
	rid = 0;
	sc->irq = bus_alloc_resource(dev, SYS_RES_IRQ, &rid, 0, ~0, 1,
				 RF_SHAREABLE | RF_ACTIVE);
	if (sc->irq == NULL) {
		device_printf(dev, "could not map interrupt\n");
		error = ENXIO;
		goto fail;
	}

	error = bus_setup_intr(dev, sc->irq, INTR_TYPE_NET,
			       fxp_intr, sc, &sc->ih);
	if (error) {
		device_printf(dev, "could not setup irq\n");
		goto fail;
	}
#endif

	/*
	 * get mapping and base address of registers
	 */
	pcib_conf_read16(sc->pci_signature, PCI_COMMAND,&val16);
	DBGLVL_PRINTK(4,"fxp_attach: PCI_COMMAND_read  = 0x%x\n",val16);
	if((val16 & PCI_COMMAND_IO) != 0) {
	  sc->pci_regs_are_io = true;
	  pcib_conf_read32(sc->pci_signature,
			   PCI_BASE_ADDRESS_1,
			   &val32);
	  sc->pci_regs_base = val32 & PCI_BASE_ADDRESS_IO_MASK;
	}
	else {
	  sc->pci_regs_are_io = false;
	  pcib_conf_read32(sc->pci_signature,
			   PCI_BASE_ADDRESS_0,
			   &val32);
	  sc->pci_regs_base = val32 & PCI_BASE_ADDRESS_MEM_MASK;
	}
	DBGLVL_PRINTK(3,"fxp_attach: CSR registers are mapped in %s space"
		      " at address 0x%x\n",
		      sc->pci_regs_are_io ? "I/O" : "MEM",
		      sc->pci_regs_base);

	/*
	 * get interrupt level to be used
	 */
	pcib_conf_read8(sc->pci_signature, 60, &interrupt);
	DBGLVL_PRINTK(3,"fxp_attach: interrupt = 0x%x\n",interrupt);
	sc->irqInfo.name = (rtems_irq_number)interrupt;
	/*
	 * Reset to a stable state.
	CSR_WRITE_4(sc, FXP_CSR_PORT, FXP_PORT_SELECTIVE_RESET);
	 */
	CSR_WRITE_4(sc, FXP_CSR_PORT, FXP_PORT_SOFTWARE_RESET);
	DELAY(10);

	sc->cbl_base = malloc(sizeof(struct fxp_cb_tx) * FXP_NTXCB,
	    M_DEVBUF, M_NOWAIT);
	DBGLVL_PRINTK(3,"fxp_attach: sc->cbl_base = 0x%x\n",sc->cbl_base);
	if (sc->cbl_base == NULL)
		goto failmem;
	else
	        memset(sc->cbl_base, 0, sizeof(struct fxp_cb_tx) * FXP_NTXCB);

	sc->fxp_stats = malloc(sizeof(struct fxp_stats), M_DEVBUF,
	    M_NOWAIT);
	DBGLVL_PRINTK(3,"fxp_attach: sc->fxp_stats = 0x%x\n",sc->fxp_stats);
	if (sc->fxp_stats == NULL)
		goto failmem;
	else
	        memset(sc->fxp_stats, 0, sizeof(struct fxp_stats));

	sc->mcsp = malloc(sizeof(struct fxp_cb_mcs), M_DEVBUF, M_NOWAIT);
	DBGLVL_PRINTK(3,"fxp_attach: sc->mcsp = 0x%x\n",sc->mcsp);
	if (sc->mcsp == NULL)
		goto failmem;

	/*
	 * Pre-allocate our receive buffers.
	 */
	for (i = 0; i < FXP_NRFABUFS; i++) {
		if (fxp_add_rfabuf(sc, NULL) != 0) {
			goto failmem;
		}
	}

	/*
	 * Find out how large of an SEEPROM we have.
	 */
	DBGLVL_PRINTK(3,"fxp_attach: calling fxp_autosize_eeprom\n");
	fxp_autosize_eeprom(sc);

	/*
	 * Determine whether we must use the 503 serial interface.
	 */
	fxp_read_eeprom(sc, &data, 6, 1);
	if ((data & FXP_PHY_DEVICE_MASK) != 0 &&
	    (data & FXP_PHY_SERIAL_ONLY))
		sc->flags |= FXP_FLAG_SERIAL_MEDIA;

	/*
	 * Find out the basic controller type; we currently only
	 * differentiate between a 82557 and greater.
	 */
	fxp_read_eeprom(sc, &data, 5, 1);
	if ((data >> 8) == 1)
		sc->chip = FXP_CHIP_82557;
	DBGLVL_PRINTK(3,"fxp_attach: sc->chip = %d\n",sc->chip);

	/*
	 * Enable workarounds for certain chip revision deficiencies.
	 *
	 * Systems based on the ICH2/ICH2-M chip from Intel have a defect
	 * where the chip can cause a PCI protocol violation if it receives
	 * a CU_RESUME command when it is entering the IDLE state.  The
	 * workaround is to disable Dynamic Standby Mode, so the chip never
	 * deasserts CLKRUN#, and always remains in an active state.
	 *
	 * See Intel 82801BA/82801BAM Specification Update, Errata #30.
	 */
#ifdef NOTUSED
	i = pci_get_device(dev);
#else
	pcib_conf_read16(sc->pci_signature,2,&dev_id);
	DBGLVL_PRINTK(3,"fxp_attach: device id = 0x%x\n",dev_id);
#endif
	if (dev_id == 0x2449 || (dev_id > 0x1030 && dev_id < 0x1039)) {
        device_printf(dev, "*** See Intel 82801BA/82801BAM Specification Update, Errata #30. ***\n");
		fxp_read_eeprom(sc, &data, 10, 1);
		if (data & 0x02) {			/* STB enable */
			u_int16_t cksum;
			int i;

			device_printf(dev,
		    "*** DISABLING DYNAMIC STANDBY MODE IN EEPROM ***\n");
			data &= ~0x02;
			fxp_write_eeprom(sc, &data, 10, 1);
			device_printf(dev, "New EEPROM ID: 0x%x\n", data);
			cksum = 0;
			for (i = 0; i < (1 << sc->eeprom_size) - 1; i++) {
				fxp_read_eeprom(sc, &data, i, 1);
				cksum += data;
			}
			i = (1 << sc->eeprom_size) - 1;
			cksum = 0xBABA - cksum;
			fxp_read_eeprom(sc, &data, i, 1);
			fxp_write_eeprom(sc, &cksum, i, 1);
			device_printf(dev,
			    "EEPROM checksum @ 0x%x: 0x%x -> 0x%x\n",
			    i, data, cksum);
			/*
			 * We need to do a full PCI reset here.  A software
			 * reset to the port doesn't cut it, but let's try
			 * anyway.
			 */
			CSR_WRITE_4(sc, FXP_CSR_PORT, FXP_PORT_SOFTWARE_RESET);
			DELAY(50);
			device_printf(dev,
	    "*** PLEASE REBOOT THE SYSTEM NOW FOR CORRECT OPERATION ***\n");
#if 1
			/*
			 * If the user elects to continue, try the software
			 * workaround, as it is better than nothing.
			 */
			sc->flags |= FXP_FLAG_CU_RESUME_BUG;
#endif
		}
	}

	/*
	 * If we are not a 82557 chip, we can enable extended features.
	 */
	if (sc->chip != FXP_CHIP_82557) {
	  u_int8_t tmp_val;
		/*
		 * If MWI is enabled in the PCI configuration, and there
		 * is a valid cacheline size (8 or 16 dwords), then tell
		 * the board to turn on MWI.
		 */
	        pcib_conf_read8(sc->pci_signature,
				PCI_CACHE_LINE_SIZE,&tmp_val);
		DBGLVL_PRINTK(3,"fxp_attach: CACHE_LINE_SIZE = %d\n",tmp_val);
		if (val16 & PCI_COMMAND_MEMORY &&
		    tmp_val != 0)
			sc->flags |= FXP_FLAG_MWI_ENABLE;

		/* turn on the extended TxCB feature */
		sc->flags |= FXP_FLAG_EXT_TXCB;

		/* enable reception of long frames for VLAN */
		sc->flags |= FXP_FLAG_LONG_PKT_EN;
		DBGLVL_PRINTK(3,"fxp_attach: sc->flags = 0x%x\n",
			      sc->flags);
	}

	/*
	 * Read MAC address.
	 */
	fxp_read_eeprom(sc, (u_int16_t*)sc->arpcom.ac_enaddr, 0, 3);
	if (fxp_is_verbose) {
	    device_printf(dev, "Ethernet address %x:%x:%x:%x:%x:%x %s \n",
	        ((u_int8_t*)sc->arpcom.ac_enaddr)[0],
	        ((u_int8_t*)sc->arpcom.ac_enaddr)[1],
    	    ((u_int8_t*)sc->arpcom.ac_enaddr)[2],
    	    ((u_int8_t*)sc->arpcom.ac_enaddr)[3],
    	    ((u_int8_t*)sc->arpcom.ac_enaddr)[4],
    	    ((u_int8_t*)sc->arpcom.ac_enaddr)[5],
    	    sc->flags & FXP_FLAG_SERIAL_MEDIA ? ", 10Mbps" : "");
		device_printf(dev, "PCI IDs: 0x%x 0x%x 0x%x 0x%x 0x%x\n",
		    pci_get_vendor(sc), pci_get_device(sc),
		    pci_get_subvendor(sc), pci_get_subdevice(sc),
		    pci_get_revid(sc));
		device_printf(dev, "Chip Type: %d\n", sc->chip);
	}

#ifdef NOTUSED /* do not set up interface at all... */
	/*
	 * If this is only a 10Mbps device, then there is no MII, and
	 * the PHY will use a serial interface instead.
	 *
	 * The Seeq 80c24 AutoDUPLEX(tm) Ethernet Interface Adapter
	 * doesn't have a programming interface of any sort.  The
	 * media is sensed automatically based on how the link partner
	 * is configured.  This is, in essence, manual configuration.
	 */
	if (sc->flags & FXP_FLAG_SERIAL_MEDIA) {
		ifmedia_init(&sc->sc_media, 0, fxp_serial_ifmedia_upd,
		    fxp_serial_ifmedia_sts);
		ifmedia_add(&sc->sc_media, IFM_ETHER|IFM_MANUAL, 0, NULL);
		ifmedia_set(&sc->sc_media, IFM_ETHER|IFM_MANUAL);
	} else {
		if (mii_phy_probe(dev, &sc->miibus, fxp_ifmedia_upd,
		    fxp_ifmedia_sts)) {
	                device_printf(dev, "MII without any PHY!\n");
			error = ENXIO;
			goto fail;
		}
	}
#endif
	if (config->mtu)
		mtu = config->mtu;
	else
		mtu = ETHERMTU;

	ifp->if_softc = sc;
	ifp->if_unit = unitNumber;
	ifp->if_name = unitName;
	ifp->if_mtu  = mtu;
	ifp->if_baudrate = 100000000;
	ifp->if_init = fxp_init;
	ifp->if_ioctl = fxp_ioctl;
	ifp->if_start = fxp_start;
	ifp->if_output = ether_output;
	ifp->if_watchdog = fxp_watchdog;
	ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX /*| IFF_MULTICAST*/;
	if (ifp->if_snd.ifq_maxlen == 0)
		ifp->if_snd.ifq_maxlen = ifqmaxlen;

	/*
	 * Attach the interface.
	 */
	DBGLVL_PRINTK(3,"fxp_attach: calling if_attach\n");
	if_attach (ifp);
	DBGLVL_PRINTK(3,"fxp_attach: calling ether_if_attach\n");
	ether_ifattach(ifp);
	DBGLVL_PRINTK(3,"fxp_attach: return from ether_if_attach\n");

#ifdef NOTUSED
	/*
	 * Tell the upper layer(s) we support long frames.
	 */
	ifp->if_data.ifi_hdrlen = sizeof(struct ether_vlan_header);
#endif
	/*
	 * Let the system queue as many packets as we have available
	 * TX descriptors.
	 */
	ifp->if_snd.ifq_maxlen = FXP_NTXCB - 1;

	splx(s);
	return (0);

failmem:
	device_printf(dev, "Failed to malloc memory\n");
	error = ENOMEM;
#ifdef NOTUSED
fail:
#endif
	splx(s);
	fxp_release(sc);
	return (error);
}

/*
 * release all resources
 */
static void
fxp_release(struct fxp_softc *sc)
{

#ifdef NOTUSED
	bus_generic_detach(sc->dev);
	if (sc->miibus)
		device_delete_child(sc->dev, sc->miibus);
#endif
	if (sc->cbl_base)
		free(sc->cbl_base, M_DEVBUF);
	if (sc->fxp_stats)
		free(sc->fxp_stats, M_DEVBUF);
	if (sc->mcsp)
		free(sc->mcsp, M_DEVBUF);
	if (sc->rfa_headm)
		m_freem(sc->rfa_headm);

#ifdef NOTUSED
	if (sc->ih)
		bus_teardown_intr(sc->dev, sc->irq, sc->ih);
	if (sc->irq)
		bus_release_resource(sc->dev, SYS_RES_IRQ, 0, sc->irq);
	if (sc->mem)
		bus_release_resource(sc->dev, sc->rtp, sc->rgd, sc->mem);
	mtx_destroy(&sc->sc_mtx);
#endif
}

#if NOTUSED
/*
 * Detach interface.
 */
static int
fxp_detach(device_t dev)
{
	struct fxp_softc *sc = device_get_softc(dev);
	int s;

	/* disable interrupts */
	CSR_WRITE_1(sc, FXP_CSR_SCB_INTRCNTL, FXP_SCB_INTR_DISABLE);

	s = splimp();

	/*
	 * Stop DMA and drop transmit queue.
	 */
	fxp_stop(sc);

	/*
	 * Close down routes etc.
	 */
	ether_ifdetach(&sc->arpcom.ac_if, ETHER_BPF_SUPPORTED);

	/*
	 * Free all media structures.
	 */
	ifmedia_removeall(&sc->sc_media);

	splx(s);

	/* Release our allocated resources. */
	fxp_release(sc);

	return (0);
}

/*
 * Device shutdown routine. Called at system shutdown after sync. The
 * main purpose of this routine is to shut off receiver DMA so that
 * kernel memory doesn't get clobbered during warmboot.
 */
static int
fxp_shutdown(device_t dev)
{
	/*
	 * Make sure that DMA is disabled prior to reboot. Not doing
	 * do could allow DMA to corrupt kernel memory during the
	 * reboot before the driver initializes.
	 */
	fxp_stop((struct fxp_softc *) device_get_softc(dev));
	return (0);
}
#endif

/*
 * Show interface statistics
 */
static void
fxp_stats(struct fxp_softc *sc)
{
	struct ifnet *ifp = &sc->sc_if;

	printf ("   Output packets:%-8lu", ifp->if_opackets);
	printf ("    Collisions:%-8lu", ifp->if_collisions);
	printf (" Output errors:%-8lu\n", ifp->if_oerrors);
	printf ("    Input packets:%-8lu", ifp->if_ipackets);
	printf ("  Input errors:%-8lu\n", ifp->if_ierrors);
}

static void
fxp_eeprom_shiftin(struct fxp_softc *sc, int data, int length)
{
	u_int16_t reg;
	int x;

	/*
	 * Shift in data.
	 */
	for (x = 1 << (length - 1); x; x >>= 1) {
		if (data & x)
			reg = FXP_EEPROM_EECS | FXP_EEPROM_EEDI;
		else
			reg = FXP_EEPROM_EECS;
		CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL, reg);
		DELAY(1);
		CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL, reg | FXP_EEPROM_EESK);
		DELAY(1);
		CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL, reg);
		DELAY(1);
	}
}

/*
 * Read from the serial EEPROM. Basically, you manually shift in
 * the read opcode (one bit at a time) and then shift in the address,
 * and then you shift out the data (all of this one bit at a time).
 * The word size is 16 bits, so you have to provide the address for
 * every 16 bits of data.
 */
static u_int16_t
fxp_eeprom_getword(struct fxp_softc *sc, int offset, int autosize)
{
	u_int16_t reg, data;
	int x;

	CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL, FXP_EEPROM_EECS);
	/*
	 * Shift in read opcode.
	 */
	fxp_eeprom_shiftin(sc, FXP_EEPROM_OPC_READ, 3);
	/*
	 * Shift in address.
	 */
	data = 0;
	for (x = 1 << (sc->eeprom_size - 1); x; x >>= 1) {
		if (offset & x)
			reg = FXP_EEPROM_EECS | FXP_EEPROM_EEDI;
		else
			reg = FXP_EEPROM_EECS;
		CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL, reg);
		DELAY(1);
		CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL, reg | FXP_EEPROM_EESK);
		DELAY(1);
		CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL, reg);
		DELAY(1);
		reg = CSR_READ_2(sc, FXP_CSR_EEPROMCONTROL) & FXP_EEPROM_EEDO;
		data++;
		if (autosize && reg == 0) {
			sc->eeprom_size = data;
			break;
		}
	}
	/*
	 * Shift out data.
	 */
	data = 0;
	reg = FXP_EEPROM_EECS;
	for (x = 1 << 15; x; x >>= 1) {
		CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL, reg | FXP_EEPROM_EESK);
		DELAY(1);
		if (CSR_READ_2(sc, FXP_CSR_EEPROMCONTROL) & FXP_EEPROM_EEDO)
			data |= x;
		CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL, reg);
		DELAY(1);
	}
	CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL, 0);
	DELAY(1);

	return (data);
}

static void
fxp_eeprom_putword(struct fxp_softc *sc, int offset, u_int16_t data)
{
	int i;

	/*
	 * Erase/write enable.
	 */
	CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL, FXP_EEPROM_EECS);
	fxp_eeprom_shiftin(sc, 0x4, 3);
	fxp_eeprom_shiftin(sc, 0x03 << (sc->eeprom_size - 2), sc->eeprom_size);
	CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL, 0);
	DELAY(1);
	/*
	 * Shift in write opcode, address, data.
	 */
	CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL, FXP_EEPROM_EECS);
	fxp_eeprom_shiftin(sc, FXP_EEPROM_OPC_WRITE, 3);
	fxp_eeprom_shiftin(sc, offset, sc->eeprom_size);
	fxp_eeprom_shiftin(sc, data, 16);
	CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL, 0);
	DELAY(1);
	/*
	 * Wait for EEPROM to finish up.
	 */
	CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL, FXP_EEPROM_EECS);
	DELAY(1);
	for (i = 0; i < 1000; i++) {
		if (CSR_READ_2(sc, FXP_CSR_EEPROMCONTROL) & FXP_EEPROM_EEDO)
			break;
		DELAY(50);
	}
	CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL, 0);
	DELAY(1);
	/*
	 * Erase/write disable.
	 */
	CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL, FXP_EEPROM_EECS);
	fxp_eeprom_shiftin(sc, 0x4, 3);
	fxp_eeprom_shiftin(sc, 0, sc->eeprom_size);
	CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL, 0);
	DELAY(1);
}

/*
 * From NetBSD:
 *
 * Figure out EEPROM size.
 *
 * 559's can have either 64-word or 256-word EEPROMs, the 558
 * datasheet only talks about 64-word EEPROMs, and the 557 datasheet
 * talks about the existance of 16 to 256 word EEPROMs.
 *
 * The only known sizes are 64 and 256, where the 256 version is used
 * by CardBus cards to store CIS information.
 *
 * The address is shifted in msb-to-lsb, and after the last
 * address-bit the EEPROM is supposed to output a `dummy zero' bit,
 * after which follows the actual data. We try to detect this zero, by
 * probing the data-out bit in the EEPROM control register just after
 * having shifted in a bit. If the bit is zero, we assume we've
 * shifted enough address bits. The data-out should be tri-state,
 * before this, which should translate to a logical one.
 */
static void
fxp_autosize_eeprom(struct fxp_softc *sc)
{

	/* guess maximum size of 256 words */
	sc->eeprom_size = 8;

	/* autosize */
	(void) fxp_eeprom_getword(sc, 0, 1);
}

static void
fxp_read_eeprom(struct fxp_softc *sc, u_short *data, int offset, int words)
{
	int i;

	for (i = 0; i < words; i++) {
		data[i] = fxp_eeprom_getword(sc, offset + i, 0);
		DBGLVL_PRINTK(4,"fxp_eeprom_read(off=0x%x)=0x%x\n",
			      offset+i,data[i]);
	}
}

static void
fxp_write_eeprom(struct fxp_softc *sc, u_short *data, int offset, int words)
{
	int i;

	for (i = 0; i < words; i++)
		fxp_eeprom_putword(sc, offset + i, data[i]);
		DBGLVL_PRINTK(4,"fxp_eeprom_write(off=0x%x,0x%x)\n",
			      offset+i,data[i]);
}

/*
 * Start packet transmission on the interface.
 */
static void
fxp_start(struct ifnet *ifp)
{
	struct fxp_softc *sc = ifp->if_softc;
	struct fxp_cb_tx *txp;

	DBGLVL_PRINTK(3,"fxp_start called\n");

	/*
	 * See if we need to suspend xmit until the multicast filter
	 * has been reprogrammed (which can only be done at the head
	 * of the command chain).
	 */
	if (sc->need_mcsetup) {
		return;
	}

	txp = NULL;

	/*
	 * We're finished if there is nothing more to add to the list or if
	 * we're all filled up with buffers to transmit.
	 * NOTE: One TxCB is reserved to guarantee that fxp_mc_setup() can add
	 *       a NOP command when needed.
	 */
	while (ifp->if_snd.ifq_head != NULL && sc->tx_queued < FXP_NTXCB - 1) {
		struct mbuf *m, *mb_head;
		int segment;

		/*
		 * Grab a packet to transmit.
		 */
		IF_DEQUEUE(&ifp->if_snd, mb_head);

		/*
		 * Get pointer to next available tx desc.
		 */
		txp = sc->cbl_last->next;

		/*
		 * Go through each of the mbufs in the chain and initialize
		 * the transmit buffer descriptors with the physical address
		 * and size of the mbuf.
		 */
tbdinit:
		for (m = mb_head, segment = 0; m != NULL; m = m->m_next) {
			if (m->m_len != 0) {
				if (segment == FXP_NTXSEG)
					break;
				txp->tbd[segment].tb_addr =
				    vtophys(mtod(m, vm_offset_t));
				txp->tbd[segment].tb_size = m->m_len;
				segment++;
			}
		}
		if (m != NULL) {
			struct mbuf *mn;

			/*
			 * We ran out of segments. We have to recopy this
			 * mbuf chain first. Bail out if we can't get the
			 * new buffers.
			 */
			MGETHDR(mn, M_DONTWAIT, MT_DATA);
			if (mn == NULL) {
				m_freem(mb_head);
				break;
			}
			if (mb_head->m_pkthdr.len > MHLEN) {
				MCLGET(mn, M_DONTWAIT);
				if ((mn->m_flags & M_EXT) == 0) {
					m_freem(mn);
					m_freem(mb_head);
					break;
				}
			}
			m_copydata(mb_head, 0, mb_head->m_pkthdr.len,
			    mtod(mn, caddr_t));
			mn->m_pkthdr.len = mn->m_len = mb_head->m_pkthdr.len;
			m_freem(mb_head);
			mb_head = mn;
			goto tbdinit;
		}

		txp->tbd_number = segment;
		txp->mb_head = mb_head;
		txp->cb_status = 0;
		if (sc->tx_queued != FXP_CXINT_THRESH - 1) {
			txp->cb_command =
			    FXP_CB_COMMAND_XMIT | FXP_CB_COMMAND_SF |
			    FXP_CB_COMMAND_S;
		} else {
			txp->cb_command =
			    FXP_CB_COMMAND_XMIT | FXP_CB_COMMAND_SF |
			    FXP_CB_COMMAND_S | FXP_CB_COMMAND_I;
			/*
			 * Set a 5 second timer just in case we don't hear
			 * from the card again.
			 */
			ifp->if_timer = 5;
		}
		txp->tx_threshold = tx_threshold;

		/*
		 * Advance the end of list forward.
		 */

#ifdef __alpha__
		/*
		 * On platforms which can't access memory in 16-bit
		 * granularities, we must prevent the card from DMA'ing
		 * up the status while we update the command field.
		 * This could cause us to overwrite the completion status.
		 */
		atomic_clear_short(&sc->cbl_last->cb_command,
		    FXP_CB_COMMAND_S);
#else
		sc->cbl_last->cb_command &= ~FXP_CB_COMMAND_S;
#endif /*__alpha__*/
		sc->cbl_last = txp;

		/*
		 * Advance the beginning of the list forward if there are
		 * no other packets queued (when nothing is queued, cbl_first
		 * sits on the last TxCB that was sent out).
		 */
		if (sc->tx_queued == 0)
			sc->cbl_first = txp;

		sc->tx_queued++;

#ifdef NOTUSED
		/*
		 * Pass packet to bpf if there is a listener.
		 */
		if (ifp->if_bpf)
			bpf_mtap(ifp, mb_head);
#endif
	}

	/*
	 * We're finished. If we added to the list, issue a RESUME to get DMA
	 * going again if suspended.
	 */
	if (txp != NULL) {
		fxp_scb_wait(sc);
		fxp_scb_cmd(sc, FXP_SCB_COMMAND_CU_RESUME);
	}
}

/*
 * Process interface interrupts.
 */
static rtems_isr fxp_intr(rtems_vector_number v)
{
  /*
   * FIXME: currently only works with one interface...
   */
  struct fxp_softc *sc = &(fxp_softc[0]);

  /*
   * disable interrupts
   */
  CSR_WRITE_1(sc, FXP_CSR_SCB_INTRCNTL, FXP_SCB_INTR_DISABLE);
  /*
   * send event to deamon
   */
  rtems_bsdnet_event_send (sc->daemonTid, INTERRUPT_EVENT);
}

static void fxp_daemon(void *xsc)
{
	struct fxp_softc *sc = xsc;
	struct ifnet *ifp = &sc->sc_if;
	u_int8_t statack;
	rtems_event_set events;
	rtems_interrupt_level level;

#ifdef NOTUSED
	if (sc->suspended) {
		return;
	}
#endif
	for (;;) {

	DBGLVL_PRINTK(4,"fxp_daemon waiting for event\n");
	  /*
	   * wait for event to receive from interrupt function
	   */
	  rtems_bsdnet_event_receive (INTERRUPT_EVENT,
				      RTEMS_WAIT|RTEMS_EVENT_ANY,
				      RTEMS_NO_TIMEOUT,
				      &events);
	  while ((statack = CSR_READ_1(sc, FXP_CSR_SCB_STATACK)) != 0) {
	    DBGLVL_PRINTK(4,"fxp_daemon: processing event, statack = 0x%x\n",
			  statack);
#ifdef NOTUSED
		/*
		 * It should not be possible to have all bits set; the
		 * FXP_SCB_INTR_SWI bit always returns 0 on a read.  If
		 * all bits are set, this may indicate that the card has
		 * been physically ejected, so ignore it.
		 */
		if (statack == 0xff)
			return;
#endif

		/*
		 * First ACK all the interrupts in this pass.
		 */
		CSR_WRITE_1(sc, FXP_CSR_SCB_STATACK, statack);

		/*
		 * Free any finished transmit mbuf chains.
		 *
		 * Handle the CNA event likt a CXTNO event. It used to
		 * be that this event (control unit not ready) was not
		 * encountered, but it is now with the SMPng modifications.
		 * The exact sequence of events that occur when the interface
		 * is brought up are different now, and if this event
		 * goes unhandled, the configuration/rxfilter setup sequence
		 * can stall for several seconds. The result is that no
		 * packets go out onto the wire for about 5 to 10 seconds
		 * after the interface is ifconfig'ed for the first time.
		 */
		if (statack & (FXP_SCB_STATACK_CXTNO | FXP_SCB_STATACK_CNA)) {
			struct fxp_cb_tx *txp;

			for (txp = sc->cbl_first; sc->tx_queued &&
			    (txp->cb_status & FXP_CB_STATUS_C) != 0;
			    txp = txp->next) {
				if (txp->mb_head != NULL) {
					m_freem(txp->mb_head);
					txp->mb_head = NULL;
				}
				sc->tx_queued--;
			}
			sc->cbl_first = txp;
			ifp->if_timer = 0;
			if (sc->tx_queued == 0) {
				if (sc->need_mcsetup)
					fxp_mc_setup(sc);
			}
			/*
			 * Try to start more packets transmitting.
			 */
			if (ifp->if_snd.ifq_head != NULL)
				fxp_start(ifp);
		}
		/*
		 * Process receiver interrupts. If a no-resource (RNR)
		 * condition exists, get whatever packets we can and
		 * re-start the receiver.
		 */
		if (statack & (FXP_SCB_STATACK_FR | FXP_SCB_STATACK_RNR)) {
			struct mbuf *m;
			struct fxp_rfa *rfa;
rcvloop:
			m = sc->rfa_headm;
			rfa = (struct fxp_rfa *)(m->m_ext.ext_buf +
			    RFA_ALIGNMENT_FUDGE);

			if (rfa->rfa_status & FXP_RFA_STATUS_C) {
				/*
				 * Remove first packet from the chain.
				 */
				sc->rfa_headm = m->m_next;
				m->m_next = NULL;

				/*
				 * Add a new buffer to the receive chain.
				 * If this fails, the old buffer is recycled
				 * instead.
				 */
				if (fxp_add_rfabuf(sc, m) == 0) {
					struct ether_header *eh;
					int total_len;

					total_len = rfa->actual_size &
					    (MCLBYTES - 1);
					if (total_len <
					    sizeof(struct ether_header)) {
						m_freem(m);
						goto rcvloop;
					}

					/*
					 * Drop the packet if it has CRC
					 * errors.  This test is only needed
					 * when doing 802.1q VLAN on the 82557
					 * chip.
					 */
					if (rfa->rfa_status &
					    FXP_RFA_STATUS_CRC) {
						m_freem(m);
						goto rcvloop;
					}

					m->m_pkthdr.rcvif = ifp;
					m->m_pkthdr.len = m->m_len = total_len;
					eh = mtod(m, struct ether_header *);
					m->m_data +=
					    sizeof(struct ether_header);
					m->m_len -=
					    sizeof(struct ether_header);
					m->m_pkthdr.len = m->m_len;
					ether_input(ifp, eh, m);
				}
				goto rcvloop;
			}
			if (statack & FXP_SCB_STATACK_RNR) {
				fxp_scb_wait(sc);
				CSR_WRITE_4(sc, FXP_CSR_SCB_GENERAL,
				    vtophys(sc->rfa_headm->m_ext.ext_buf) +
					RFA_ALIGNMENT_FUDGE);
				fxp_scb_cmd(sc, FXP_SCB_COMMAND_RU_START);
			}
		}
	  }
	  /*
	   * reenable interrupts
	   */
	  rtems_interrupt_disable (level);
	  CSR_WRITE_1(sc, FXP_CSR_SCB_INTRCNTL,0);
	  rtems_interrupt_enable (level);
	}
}

/*
 * Update packet in/out/collision statistics. The i82557 doesn't
 * allow you to access these counters without doing a fairly
 * expensive DMA to get _all_ of the statistics it maintains, so
 * we do this operation here only once per second. The statistics
 * counters in the kernel are updated from the previous dump-stats
 * DMA and then a new dump-stats DMA is started. The on-chip
 * counters are zeroed when the DMA completes. If we can't start
 * the DMA immediately, we don't wait - we just prepare to read
 * them again next time.
 */
static void
fxp_tick(void *xsc)
{
	struct fxp_softc *sc = xsc;
	struct ifnet *ifp = &sc->sc_if;
	struct fxp_stats *sp = sc->fxp_stats;
	struct fxp_cb_tx *txp;
	int s;

	DBGLVL_PRINTK(4,"fxp_tick called\n");

	ifp->if_opackets += sp->tx_good;
	ifp->if_collisions += sp->tx_total_collisions;
	if (sp->rx_good) {
		ifp->if_ipackets += sp->rx_good;
		sc->rx_idle_secs = 0;
	} else {
		/*
		 * Receiver's been idle for another second.
		 */
		sc->rx_idle_secs++;
	}
	ifp->if_ierrors +=
	    sp->rx_crc_errors +
	    sp->rx_alignment_errors +
	    sp->rx_rnr_errors +
	    sp->rx_overrun_errors;
	/*
	 * If any transmit underruns occured, bump up the transmit
	 * threshold by another 512 bytes (64 * 8).
	 */
	if (sp->tx_underruns) {
		ifp->if_oerrors += sp->tx_underruns;
		if (tx_threshold < 192)
			tx_threshold += 64;
	}
	s = splimp();
	/*
	 * Release any xmit buffers that have completed DMA. This isn't
	 * strictly necessary to do here, but it's advantagous for mbufs
	 * with external storage to be released in a timely manner rather
	 * than being defered for a potentially long time. This limits
	 * the delay to a maximum of one second.
	 */
	for (txp = sc->cbl_first; sc->tx_queued &&
	    (txp->cb_status & FXP_CB_STATUS_C) != 0;
	    txp = txp->next) {
		if (txp->mb_head != NULL) {
			m_freem(txp->mb_head);
			txp->mb_head = NULL;
		}
		sc->tx_queued--;
	}
	sc->cbl_first = txp;
	/*
	 * If we haven't received any packets in FXP_MAC_RX_IDLE seconds,
	 * then assume the receiver has locked up and attempt to clear
	 * the condition by reprogramming the multicast filter. This is
	 * a work-around for a bug in the 82557 where the receiver locks
	 * up if it gets certain types of garbage in the syncronization
	 * bits prior to the packet header. This bug is supposed to only
	 * occur in 10Mbps mode, but has been seen to occur in 100Mbps
	 * mode as well (perhaps due to a 10/100 speed transition).
	 */
	if (sc->rx_idle_secs > FXP_MAX_RX_IDLE) {
		sc->rx_idle_secs = 0;
		fxp_mc_setup(sc);
	}
	/*
	 * If there is no pending command, start another stats
	 * dump. Otherwise punt for now.
	 */
	if (CSR_READ_1(sc, FXP_CSR_SCB_COMMAND) == 0) {
		/*
		 * Start another stats dump.
		 */
		fxp_scb_cmd(sc, FXP_SCB_COMMAND_CU_DUMPRESET);
	} else {
		/*
		 * A previous command is still waiting to be accepted.
		 * Just zero our copy of the stats and wait for the
		 * next timer event to update them.
		 */
		sp->tx_good = 0;
		sp->tx_underruns = 0;
		sp->tx_total_collisions = 0;

		sp->rx_good = 0;
		sp->rx_crc_errors = 0;
		sp->rx_alignment_errors = 0;
		sp->rx_rnr_errors = 0;
		sp->rx_overrun_errors = 0;
	}
#ifdef NOTUSED
	if (sc->miibus != NULL)
		mii_tick(device_get_softc(sc->miibus));
#endif
	splx(s);
	/*
	 * Schedule another timeout one second from now.
	 */
	if (sc->stat_ch == fxp_timeout_running) {
	  timeout(fxp_tick, sc, hz);
	}
	else if (sc->stat_ch == fxp_timeout_stop_rq) {
	  sc->stat_ch = fxp_timeout_stopped;
	}
}

/*
 * Stop the interface. Cancels the statistics updater and resets
 * the interface.
 */
static void
fxp_stop(struct fxp_softc *sc)
{
	struct ifnet *ifp = &sc->sc_if;
	struct fxp_cb_tx *txp;
	int i;

	DBGLVL_PRINTK(2,"fxp_stop called\n");

	ifp->if_flags &= ~(IFF_RUNNING | IFF_OACTIVE);
	ifp->if_timer = 0;

	/*
	 * stop stats updater.
	 */
	if (sc->stat_ch == fxp_timeout_running) {
	  DBGLVL_PRINTK(3,"fxp_stop: trying to stop stat update tick\n");
	  sc->stat_ch = fxp_timeout_stop_rq;
	  while(sc->stat_ch != fxp_timeout_stopped) {
	    rtems_bsdnet_semaphore_release();
	    rtems_task_wake_after(fxp_ticksPerSecond);
	    rtems_bsdnet_semaphore_obtain();
	  }
	  DBGLVL_PRINTK(3,"fxp_stop: stat update tick stopped\n");
	}
	/*
	 * Issue software reset
	 */
	DBGLVL_PRINTK(3,"fxp_stop: issue software reset\n");
	CSR_WRITE_4(sc, FXP_CSR_PORT, FXP_PORT_SELECTIVE_RESET);
	DELAY(10);

	/*
	 * Release any xmit buffers.
	 */
	DBGLVL_PRINTK(3,"fxp_stop: releasing xmit buffers\n");
	txp = sc->cbl_base;
	if (txp != NULL) {
		for (i = 0; i < FXP_NTXCB; i++) {
			if (txp[i].mb_head != NULL) {
				m_freem(txp[i].mb_head);
				txp[i].mb_head = NULL;
			}
		}
	}
	sc->tx_queued = 0;

	/*
	 * Free all the receive buffers then reallocate/reinitialize
	 */
	DBGLVL_PRINTK(3,"fxp_stop: free and reinit all receive buffers\n");
	if (sc->rfa_headm != NULL)
		m_freem(sc->rfa_headm);
	sc->rfa_headm = NULL;
	sc->rfa_tailm = NULL;
	for (i = 0; i < FXP_NRFABUFS; i++) {
		if (fxp_add_rfabuf(sc, NULL) != 0) {
			/*
			 * This "can't happen" - we're at splimp()
			 * and we just freed all the buffers we need
			 * above.
			 */
			panic("fxp_stop: no buffers!");
		}
	}
	DBGLVL_PRINTK(2,"fxp_stop: finished\n");
}

/*
 * Watchdog/transmission transmit timeout handler. Called when a
 * transmission is started on the interface, but no interrupt is
 * received before the timeout. This usually indicates that the
 * card has wedged for some reason.
 */
static void
fxp_watchdog(struct ifnet *ifp)
{
	struct fxp_softc *sc = ifp->if_softc;

	device_printf(sc->dev, "device timeout\n");
	ifp->if_oerrors++;

	fxp_init(sc);
}

static void
fxp_init(void *xsc)
{
	struct fxp_softc *sc = xsc;
	struct ifnet *ifp = &sc->sc_if;
	struct fxp_cb_config *cbp;
	struct fxp_cb_ias *cb_ias;
	struct fxp_cb_tx *txp;
	int i, prm, s, rv;

rtems_task_wake_after(100);
	DBGLVL_PRINTK(2,"fxp_init called\n");

	s = splimp();
	/*
	 * Cancel any pending I/O
	 */
	/*
	 * E. Norum 2004-10-11
	 * Add line suggested by "Eugene Denisov" <dea@sendmail.ru>.
	 * Prevents lockup at initialization.
	 */
	sc->stat_ch = fxp_timeout_stopped;
	fxp_stop(sc);

	prm = (ifp->if_flags & IFF_PROMISC) ? 1 : 0;

	DBGLVL_PRINTK(5,"fxp_init: Initializing base of CBL and RFA memory\n");
	/*
	 * Initialize base of CBL and RFA memory. Loading with zero
	 * sets it up for regular linear addressing.
	 */
	CSR_WRITE_4(sc, FXP_CSR_SCB_GENERAL, 0);
	fxp_scb_cmd(sc, FXP_SCB_COMMAND_CU_BASE);

	fxp_scb_wait(sc);
	fxp_scb_cmd(sc, FXP_SCB_COMMAND_RU_BASE);

	/*
	 * Initialize base of dump-stats buffer.
	 */
	DBGLVL_PRINTK(5,"fxp_init: Initializing base of dump-stats buffer\n");
	fxp_scb_wait(sc);
	CSR_WRITE_4(sc, FXP_CSR_SCB_GENERAL, vtophys(sc->fxp_stats));
	fxp_scb_cmd(sc, FXP_SCB_COMMAND_CU_DUMP_ADR);

	/*
	 * We temporarily use memory that contains the TxCB list to
	 * construct the config CB. The TxCB list memory is rebuilt
	 * later.
	 */
	cbp = (struct fxp_cb_config *) sc->cbl_base;
	DBGLVL_PRINTK(5,"fxp_init: cbp = 0x%x\n",cbp);

	/*
	 * This memcpy is kind of disgusting, but there are a bunch of must be
	 * zero and must be one bits in this structure and this is the easiest
	 * way to initialize them all to proper values.
	 */
	memcpy(	(void *)(u_int32_t*)(volatile void *)&cbp->cb_status,
		fxp_cb_config_template,
		sizeof(fxp_cb_config_template));

	cbp->cb_status =	0;
	cbp->cb_command =	FXP_CB_COMMAND_CONFIG | FXP_CB_COMMAND_EL;
	cbp->link_addr =	-1;	/* (no) next command */
	cbp->byte_count =	22;	/* (22) bytes to config */
	cbp->rx_fifo_limit =	8;	/* rx fifo threshold (32 bytes) */
	cbp->tx_fifo_limit =	0;	/* tx fifo threshold (0 bytes) */
	cbp->adaptive_ifs =	0;	/* (no) adaptive interframe spacing */
	cbp->mwi_enable =	sc->flags & FXP_FLAG_MWI_ENABLE ? 1 : 0;
	cbp->type_enable =	0;	/* actually reserved */
	cbp->read_align_en =	sc->flags & FXP_FLAG_READ_ALIGN ? 1 : 0;
	cbp->end_wr_on_cl =	sc->flags & FXP_FLAG_WRITE_ALIGN ? 1 : 0;
	cbp->rx_dma_bytecount =	0;	/* (no) rx DMA max */
	cbp->tx_dma_bytecount =	0;	/* (no) tx DMA max */
	cbp->dma_mbce =		0;	/* (disable) dma max counters */
	cbp->late_scb =		0;	/* (don't) defer SCB update */
	cbp->direct_dma_dis =	1;	/* disable direct rcv dma mode */
	cbp->tno_int_or_tco_en =0;	/* (disable) tx not okay interrupt */
	cbp->ci_int =		1;	/* interrupt on CU idle */
	cbp->ext_txcb_dis = 	sc->flags & FXP_FLAG_EXT_TXCB ? 0 : 1;
	cbp->ext_stats_dis = 	1;	/* disable extended counters */
	cbp->keep_overrun_rx = 	0;	/* don't pass overrun frames to host */
	cbp->save_bf =		sc->chip == FXP_CHIP_82557 ? 1 : prm;
	cbp->disc_short_rx =	!prm;	/* discard short packets */
	cbp->underrun_retry =	1;	/* retry mode (once) on DMA underrun */
	cbp->two_frames =	0;	/* do not limit FIFO to 2 frames */
	cbp->dyn_tbd =		0;	/* (no) dynamic TBD mode */
	cbp->mediatype =	sc->flags & FXP_FLAG_SERIAL_MEDIA ? 0 : 1;
	cbp->csma_dis =		0;	/* (don't) disable link */
	cbp->tcp_udp_cksum =	0;	/* (don't) enable checksum */
	cbp->vlan_tco =		0;	/* (don't) enable vlan wakeup */
	cbp->link_wake_en =	0;	/* (don't) assert PME# on link change */
	cbp->arp_wake_en =	0;	/* (don't) assert PME# on arp */
	cbp->mc_wake_en =	0;	/* (don't) enable PME# on mcmatch */
	cbp->nsai =		1;	/* (don't) disable source addr insert */
	cbp->preamble_length =	2;	/* (7 byte) preamble */
	cbp->loopback =		0;	/* (don't) loopback */
	cbp->linear_priority =	0;	/* (normal CSMA/CD operation) */
	cbp->linear_pri_mode =	0;	/* (wait after xmit only) */
	cbp->interfrm_spacing =	6;	/* (96 bits of) interframe spacing */
	cbp->promiscuous =	prm;	/* promiscuous mode */
	cbp->bcast_disable =	0;	/* (don't) disable broadcasts */
	cbp->wait_after_win =	0;	/* (don't) enable modified backoff alg*/
	cbp->ignore_ul =	0;	/* consider U/L bit in IA matching */
	cbp->crc16_en =		0;	/* (don't) enable crc-16 algorithm */
	cbp->crscdt =		sc->flags & FXP_FLAG_SERIAL_MEDIA ? 1 : 0;

	cbp->stripping =	!prm;	/* truncate rx packet to byte count */
	cbp->padding =		1;	/* (do) pad short tx packets */
	cbp->rcv_crc_xfer =	0;	/* (don't) xfer CRC to host */
	cbp->long_rx_en =	sc->flags & FXP_FLAG_LONG_PKT_EN ? 1 : 0;
	cbp->ia_wake_en =	0;	/* (don't) wake up on address match */
	cbp->magic_pkt_dis =	0;	/* (don't) disable magic packet */
					/* must set wake_en in PMCSR also */
	cbp->force_fdx =	0;	/* (don't) force full duplex */
	cbp->fdx_pin_en =	1;	/* (enable) FDX# pin */
	cbp->multi_ia =		0;	/* (don't) accept multiple IAs */
	cbp->mc_all =		sc->flags & FXP_FLAG_ALL_MCAST ? 1 : 0;

	DBGLVL_PRINTK(5,"fxp_init: cbp initialized\n");
	if (sc->chip == FXP_CHIP_82557) {
		/*
		 * The 82557 has no hardware flow control, the values
		 * below are the defaults for the chip.
		 */
		cbp->fc_delay_lsb =	0;
		cbp->fc_delay_msb =	0x40;
		cbp->pri_fc_thresh =	3;
		cbp->tx_fc_dis =	0;
		cbp->rx_fc_restop =	0;
		cbp->rx_fc_restart =	0;
		cbp->fc_filter =	0;
		cbp->pri_fc_loc =	1;
	} else {
		cbp->fc_delay_lsb =	0x1f;
		cbp->fc_delay_msb =	0x01;
		cbp->pri_fc_thresh =	3;
		cbp->tx_fc_dis =	0;	/* enable transmit FC */
		cbp->rx_fc_restop =	1;	/* enable FC restop frames */
		cbp->rx_fc_restart =	1;	/* enable FC restart frames */
		cbp->fc_filter =	!prm;	/* drop FC frames to host */
		cbp->pri_fc_loc =	1;	/* FC pri location (byte31) */
	}

	/*
	 * Start the config command/DMA.
	 */
	DBGLVL_PRINTK(5,"fxp_init: starting config command/DMA\n");
	fxp_scb_wait(sc);
	CSR_WRITE_4(sc, FXP_CSR_SCB_GENERAL, vtophys(&cbp->cb_status));
	fxp_scb_cmd(sc, FXP_SCB_COMMAND_CU_START);
	/* ...and wait for it to complete. */
	fxp_dma_wait(&cbp->cb_status, sc);

	/*
	 * Now initialize the station address. Temporarily use the TxCB
	 * memory area like we did above for the config CB.
	 */
	DBGLVL_PRINTK(5,"fxp_init: initialize station address\n");
	cb_ias = (struct fxp_cb_ias *) sc->cbl_base;
	cb_ias->cb_status = 0;
	cb_ias->cb_command = FXP_CB_COMMAND_IAS | FXP_CB_COMMAND_EL;
	cb_ias->link_addr = -1;
	memcpy((void *)(u_int32_t*)(volatile void *)cb_ias->macaddr,
	    sc->arpcom.ac_enaddr,
	    sizeof(sc->arpcom.ac_enaddr));

	/*
	 * Start the IAS (Individual Address Setup) command/DMA.
	 */
	DBGLVL_PRINTK(5,"fxp_init: start IAS command/DMA\n");
	fxp_scb_wait(sc);
	fxp_scb_cmd(sc, FXP_SCB_COMMAND_CU_START);
	/* ...and wait for it to complete. */
	fxp_dma_wait(&cb_ias->cb_status, sc);

	/*
	 * Initialize transmit control block (TxCB) list.
	 */

	DBGLVL_PRINTK(5,"fxp_init: initialize TxCB list\n");
	txp = sc->cbl_base;
	memset(txp, 0, sizeof(struct fxp_cb_tx) * FXP_NTXCB);
	for (i = 0; i < FXP_NTXCB; i++) {
		txp[i].cb_status = FXP_CB_STATUS_C | FXP_CB_STATUS_OK;
		txp[i].cb_command = FXP_CB_COMMAND_NOP;
		txp[i].link_addr =
		    vtophys(&txp[(i + 1) & FXP_TXCB_MASK].cb_status);
		if (sc->flags & FXP_FLAG_EXT_TXCB)
			txp[i].tbd_array_addr = vtophys(&txp[i].tbd[2]);
		else
			txp[i].tbd_array_addr = vtophys(&txp[i].tbd[0]);
		txp[i].next = &txp[(i + 1) & FXP_TXCB_MASK];
	}
	/*
	 * Set the suspend flag on the first TxCB and start the control
	 * unit. It will execute the NOP and then suspend.
	 */
	DBGLVL_PRINTK(5,"fxp_init: setup suspend flag\n");
	txp->cb_command = FXP_CB_COMMAND_NOP | FXP_CB_COMMAND_S;
	sc->cbl_first = sc->cbl_last = txp;
	sc->tx_queued = 1;

	fxp_scb_wait(sc);
	fxp_scb_cmd(sc, FXP_SCB_COMMAND_CU_START);

	/*
	 * Initialize receiver buffer area - RFA.
	 */
	DBGLVL_PRINTK(5,"fxp_init: initialize RFA\n");
	fxp_scb_wait(sc);
	CSR_WRITE_4(sc, FXP_CSR_SCB_GENERAL,
	    vtophys(sc->rfa_headm->m_ext.ext_buf) + RFA_ALIGNMENT_FUDGE);
	fxp_scb_cmd(sc, FXP_SCB_COMMAND_RU_START);

#ifdef NOTUSED
	/*
	 * Set current media.
	 */
	if (sc->miibus != NULL)
		mii_mediachg(device_get_softc(sc->miibus));
#endif

	ifp->if_flags |= IFF_RUNNING;
	ifp->if_flags &= ~IFF_OACTIVE;

	if (sc->daemonTid == 0) {
		/*
		 * Start driver task
		 */
		sc->daemonTid = rtems_bsdnet_newproc ("FXPd", 4096, fxp_daemon, sc);

		/*
		 * Set up interrupts
		 */
		sc->irqInfo.hdl = (rtems_irq_hdl)fxp_intr;
		sc->irqInfo.on  = nopOn;
		sc->irqInfo.off = nopOn;
		sc->irqInfo.isOn = fxpIsOn;
		rv = BSP_install_rtems_irq_handler (&sc->irqInfo);
		if (rv != 1) {
		  rtems_panic ("Can't attach fxp interrupt handler for irq %d\n",
			       sc->irqInfo.name);
		}
	}

	/*
	 * Enable interrupts.
	 */
	CSR_WRITE_1(sc, FXP_CSR_SCB_INTRCNTL, 0);
	splx(s);

	/*
	 * Start stats updater.
	 */
	sc->stat_ch = fxp_timeout_running;
	DBGLVL_PRINTK(2,"fxp_init: stats updater timeout called with hz=%d\n", hz);
	timeout(fxp_tick, sc, hz);
	DBGLVL_PRINTK(2,"fxp_init finished\n");
}

#ifdef NOTUSED
static int
fxp_serial_ifmedia_upd(struct ifnet *ifp)
{

	return (0);
}

static void
fxp_serial_ifmedia_sts(struct ifnet *ifp, struct ifmediareq *ifmr)
{

	ifmr->ifm_active = IFM_ETHER|IFM_MANUAL;
}

/*
 * Change media according to request.
 */
static int
fxp_ifmedia_upd(struct ifnet *ifp)
{
	struct fxp_softc *sc = ifp->if_softc;
	struct mii_data *mii;

	mii = device_get_softc(sc->miibus);
	mii_mediachg(mii);
	return (0);
}

/*
 * Notify the world which media we're using.
 */
static void
fxp_ifmedia_sts(struct ifnet *ifp, struct ifmediareq *ifmr)
{
	struct fxp_softc *sc = ifp->if_softc;
	struct mii_data *mii;

	mii = device_get_softc(sc->miibus);
	mii_pollstat(mii);
	ifmr->ifm_active = mii->mii_media_active;
	ifmr->ifm_status = mii->mii_media_status;

	if (ifmr->ifm_status & IFM_10_T && sc->flags & FXP_FLAG_CU_RESUME_BUG)
		sc->cu_resume_bug = 1;
	else
		sc->cu_resume_bug = 0;
}
#endif

/*
 * Add a buffer to the end of the RFA buffer list.
 * Return 0 if successful, 1 for failure. A failure results in
 * adding the 'oldm' (if non-NULL) on to the end of the list -
 * tossing out its old contents and recycling it.
 * The RFA struct is stuck at the beginning of mbuf cluster and the
 * data pointer is fixed up to point just past it.
 */
static int
fxp_add_rfabuf(struct fxp_softc *sc, struct mbuf *oldm)
{
	u_int32_t v;
	struct mbuf *m;
	struct fxp_rfa *rfa, *p_rfa;

	DBGLVL_PRINTK(4,"fxp_add_rfabuf called\n");

	MGETHDR(m, M_DONTWAIT, MT_DATA);
	if (m != NULL) {
		MCLGET(m, M_DONTWAIT);
		if ((m->m_flags & M_EXT) == 0) {
			m_freem(m);
			if (oldm == NULL)
				return 1;
			m = oldm;
			m->m_data = m->m_ext.ext_buf;
		}
	} else {
		if (oldm == NULL)
			return 1;
		m = oldm;
		m->m_data = m->m_ext.ext_buf;
	}

	/*
	 * Move the data pointer up so that the incoming data packet
	 * will be 32-bit aligned.
	 */
	m->m_data += RFA_ALIGNMENT_FUDGE;

	/*
	 * Get a pointer to the base of the mbuf cluster and move
	 * data start past it.
	 */
	rfa = mtod(m, struct fxp_rfa *);
	m->m_data += sizeof(struct fxp_rfa);
	rfa->size = (u_int16_t)(MCLBYTES - sizeof(struct fxp_rfa) - RFA_ALIGNMENT_FUDGE);

	/*
	 * Initialize the rest of the RFA.  Note that since the RFA
	 * is misaligned, we cannot store values directly.  Instead,
	 * we use an optimized, inline copy.
	 */

	rfa->rfa_status = 0;
	rfa->rfa_control = FXP_RFA_CONTROL_EL;
	rfa->actual_size = 0;

	v = -1;
	fxp_lwcopy(&v, (volatile u_int32_t*) rfa->link_addr);
	fxp_lwcopy(&v, (volatile u_int32_t*) rfa->rbd_addr);

	/*
	 * If there are other buffers already on the list, attach this
	 * one to the end by fixing up the tail to point to this one.
	 */
	if (sc->rfa_headm != NULL) {
		p_rfa = (struct fxp_rfa *) (sc->rfa_tailm->m_ext.ext_buf +
		    RFA_ALIGNMENT_FUDGE);
		sc->rfa_tailm->m_next = m;
		v = vtophys(rfa);
		fxp_lwcopy(&v, (volatile u_int32_t*) p_rfa->link_addr);
		p_rfa->rfa_control = 0;
	} else {
		sc->rfa_headm = m;
	}
	sc->rfa_tailm = m;

	return (m == oldm);
}

#ifdef NOTUSED
static volatile int
fxp_miibus_readreg(device_t dev, int phy, int reg)
{
	struct fxp_softc *sc = device_get_softc(dev);
	int count = 10000;
	int value;

	CSR_WRITE_4(sc, FXP_CSR_MDICONTROL,
	    (FXP_MDI_READ << 26) | (reg << 16) | (phy << 21));

	while (((value = CSR_READ_4(sc, FXP_CSR_MDICONTROL)) & 0x10000000) == 0
	    && count--)
		DELAY(10);

	if (count <= 0)
		device_printf(dev, "fxp_miibus_readreg: timed out\n");

	return (value & 0xffff);
}

static void
fxp_miibus_writereg(device_t dev, int phy, int reg, int value)
{
	struct fxp_softc *sc = device_get_softc(dev);
	int count = 10000;

	CSR_WRITE_4(sc, FXP_CSR_MDICONTROL,
	    (FXP_MDI_WRITE << 26) | (reg << 16) | (phy << 21) |
	    (value & 0xffff));

	while ((CSR_READ_4(sc, FXP_CSR_MDICONTROL) & 0x10000000) == 0 &&
	    count--)
		DELAY(10);

	if (count <= 0)
		device_printf(dev, "fxp_miibus_writereg: timed out\n");
}
#endif

static int
fxp_ioctl(struct ifnet *ifp, ioctl_command_t command, caddr_t data)
{
	struct fxp_softc *sc = ifp->if_softc;
#ifdef NOTUSED
	struct ifreq *ifr = (struct ifreq *)data;
	struct mii_data *mii;
#endif
	int s, error = 0;

	DBGLVL_PRINTK(2,"fxp_ioctl called\n");

	s = splimp();

	switch (command) {
	case SIOCSIFADDR:
	case SIOCGIFADDR:
	case SIOCSIFMTU:
		error = ether_ioctl(ifp, command, data);
		break;

	case SIOCSIFFLAGS:
		if (ifp->if_flags & IFF_ALLMULTI)
			sc->flags |= FXP_FLAG_ALL_MCAST;
		else
			sc->flags &= ~FXP_FLAG_ALL_MCAST;

		/*
		 * If interface is marked up and not running, then start it.
		 * If it is marked down and running, stop it.
		 * XXX If it's up then re-initialize it. This is so flags
		 * such as IFF_PROMISC are handled.
		 */
		if (ifp->if_flags & IFF_UP) {
			fxp_init(sc);
		} else {
			if (ifp->if_flags & IFF_RUNNING)
				fxp_stop(sc);
		}
		break;

	case SIOCADDMULTI:
	case SIOCDELMULTI:
		if (ifp->if_flags & IFF_ALLMULTI)
			sc->flags |= FXP_FLAG_ALL_MCAST;
		else
			sc->flags &= ~FXP_FLAG_ALL_MCAST;
		/*
		 * Multicast list has changed; set the hardware filter
		 * accordingly.
		 */
		if ((sc->flags & FXP_FLAG_ALL_MCAST) == 0)
			fxp_mc_setup(sc);
		/*
		 * fxp_mc_setup() can set FXP_FLAG_ALL_MCAST, so check it
		 * again rather than else {}.
		 */
		if (sc->flags & FXP_FLAG_ALL_MCAST)
			fxp_init(sc);
		error = 0;
		break;

#ifdef NOTUSED
	case SIOCSIFMEDIA:
	case SIOCGIFMEDIA:
		if (sc->miibus != NULL) {
			mii = device_get_softc(sc->miibus);
                        error = ifmedia_ioctl(ifp, ifr,
                            &mii->mii_media, command);
		} else {
                        error = ifmedia_ioctl(ifp, ifr, &sc->sc_media, command);
		}
		break;
#endif

    case SIO_RTEMS_SHOW_STATS:
        fxp_stats(sc);
        break;

	default:
		error = EINVAL;
	}
	splx(s);
	return (error);
}

/*
 * Program the multicast filter.
 *
 * We have an artificial restriction that the multicast setup command
 * must be the first command in the chain, so we take steps to ensure
 * this. By requiring this, it allows us to keep up the performance of
 * the pre-initialized command ring (esp. link pointers) by not actually
 * inserting the mcsetup command in the ring - i.e. its link pointer
 * points to the TxCB ring, but the mcsetup descriptor itself is not part
 * of it. We then can do 'CU_START' on the mcsetup descriptor and have it
 * lead into the regular TxCB ring when it completes.
 *
 * This function must be called at splimp.
 */
static void
fxp_mc_setup(struct fxp_softc *sc)
{
	struct fxp_cb_mcs *mcsp = sc->mcsp;
	struct ifnet *ifp = &sc->sc_if;
#ifdef NOTUSED
	struct ifmultiaddr *ifma;
#endif
	int nmcasts;
	int count;

	DBGLVL_PRINTK(2,"fxp_mc_setup called\n");

	/*
	 * If there are queued commands, we must wait until they are all
	 * completed. If we are already waiting, then add a NOP command
	 * with interrupt option so that we're notified when all commands
	 * have been completed - fxp_start() ensures that no additional
	 * TX commands will be added when need_mcsetup is true.
	 */
	if (sc->tx_queued) {
		struct fxp_cb_tx *txp;

		/*
		 * need_mcsetup will be true if we are already waiting for the
		 * NOP command to be completed (see below). In this case, bail.
		 */
		if (sc->need_mcsetup)
			return;
		sc->need_mcsetup = 1;

		/*
		 * Add a NOP command with interrupt so that we are notified when all
		 * TX commands have been processed.
		 */
		txp = sc->cbl_last->next;
		txp->mb_head = NULL;
		txp->cb_status = 0;
		txp->cb_command = FXP_CB_COMMAND_NOP |
		    FXP_CB_COMMAND_S | FXP_CB_COMMAND_I;
		/*
		 * Advance the end of list forward.
		 */
		sc->cbl_last->cb_command &= ~FXP_CB_COMMAND_S;
		sc->cbl_last = txp;
		sc->tx_queued++;
		/*
		 * Issue a resume in case the CU has just suspended.
		 */
		fxp_scb_wait(sc);
		fxp_scb_cmd(sc, FXP_SCB_COMMAND_CU_RESUME);
		/*
		 * Set a 5 second timer just in case we don't hear from the
		 * card again.
		 */
		ifp->if_timer = 5;

		return;
	}
	sc->need_mcsetup = 0;

	/*
	 * Initialize multicast setup descriptor.
	 */
	mcsp->next = sc->cbl_base;
	mcsp->mb_head = NULL;
	mcsp->cb_status = 0;
	mcsp->cb_command = FXP_CB_COMMAND_MCAS |
	    FXP_CB_COMMAND_S | FXP_CB_COMMAND_I;
	mcsp->link_addr = vtophys(&sc->cbl_base->cb_status);

	nmcasts = 0;
#ifdef NOTUSED /* FIXME: Multicast not supported? */
	if ((sc->flags & FXP_FLAG_ALL_MCAST) == 0) {
#if __FreeBSD_version < 500000
		LIST_FOREACH(ifma, &ifp->if_multiaddrs, ifma_link) {
#else
		TAILQ_FOREACH(ifma, &ifp->if_multiaddrs, ifma_link) {
#endif
			if (ifma->ifma_addr->sa_family != AF_LINK)
				continue;
			if (nmcasts >= MAXMCADDR) {
				sc->flags |= FXP_FLAG_ALL_MCAST;
				nmcasts = 0;
				break;
			}
			memcpy((void *)(uintptr_t)(volatile void *)
				&sc->mcsp->mc_addr[nmcasts][0],
				LLADDR((struct sockaddr_dl *)ifma->ifma_addr), 6);
			nmcasts++;
		}
	}
#endif
	mcsp->mc_cnt = nmcasts * 6;
	sc->cbl_first = sc->cbl_last = (struct fxp_cb_tx *) mcsp;
	sc->tx_queued = 1;

	/*
	 * Wait until command unit is not active. This should never
	 * be the case when nothing is queued, but make sure anyway.
	 */
	count = 100;
	while ((CSR_READ_1(sc, FXP_CSR_SCB_RUSCUS) >> 6) ==
	    FXP_SCB_CUS_ACTIVE && --count)
		DELAY(10);
	if (count == 0) {
		device_printf(sc->dev, "command queue timeout\n");
		return;
	}

	/*
	 * Start the multicast setup command.
	 */
	fxp_scb_wait(sc);
	CSR_WRITE_4(sc, FXP_CSR_SCB_GENERAL, vtophys(&mcsp->cb_status));
	fxp_scb_cmd(sc, FXP_SCB_COMMAND_CU_START);

	ifp->if_timer = 2;
	return;
	}

#endif /* defined(__i386__) */
