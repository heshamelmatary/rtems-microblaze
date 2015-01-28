/*
 *  This is where the real hardware setup is done. A minimal stack
 *  has been provided by the start.S code. No normal C or RTEMS
 *  functions can be called from here.
 */

#include <bsp.h>
#include <bsp/bootcard.h>

extern void _wr_vbr(uint32_t);
extern void init_main(void);

/*
 * From linkcmds
 */

extern uint8_t _INTERRUPT_VECTOR[];

extern uint8_t _clear_start[];
extern uint8_t _clear_end[];

extern uint8_t _data_src_start[];
extern uint8_t _data_dest_start[];
extern uint8_t _data_dest_end[];

void Init52235(void)
{
  register uint32_t i;
  register uint32_t *dp, *sp;
  register uint8_t *dbp, *sbp;

  /*
   * Initialize the hardware
   */
  init_main();

  /*
   * Copy the vector table to RAM
   */
  if (&_VBR != (void *) _INTERRUPT_VECTOR) {
    sp = (uint32_t *) _INTERRUPT_VECTOR;
    dp = (uint32_t *) &_VBR;
    for (i = 0; i < 256; i++) {
      *dp++ = *sp++;
    }
  }

  _wr_vbr((uint32_t) &_VBR);

  /*
   * Move initialized data from ROM to RAM.
   */
  if (_data_src_start != _data_dest_start) {
    dbp = (uint8_t *) _data_dest_start;
    sbp = (uint8_t *) _data_src_start;
    i = _data_dest_end - _data_dest_start;
    while (i--)
      *dbp++ = *sbp++;
  }

  /*
   * Zero uninitialized data
   */

  if (_clear_start != _clear_end) {
    sbp = _clear_start;
    dbp = _clear_end;
    i = dbp - sbp;
    while (i--)
      *sbp++ = 0;
  }

  /*
   * We have to call some kind of RTEMS function here!
   */

  boot_card(0);
  for (;;) ;
}
