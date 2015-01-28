
#include <sys/cdefs.h>

#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <rtems/rtl/rtl.h>
#include "rtl-elf.h"
#include "rtl-error.h"
#include "rtl-trace.h"

bool
rtems_rtl_elf_rel_resolve_sym (Elf_Word type)
{
  return true;
}

bool
rtems_rtl_elf_relocate_rela (const rtems_rtl_obj_t*      obj,
                             const Elf_Rela*             rela,
                             const rtems_rtl_obj_sect_t* sect,
                             const char*                 symname,
                             const Elf_Byte              syminfo,
                             const Elf_Word              symvalue)
{
  Elf_Addr *where;
  Elf_Sword tmp;

  where = (Elf_Addr *)(sect->base + rela->r_offset);

  /* Handle the not 4byte aligned address carefully */

  switch (ELF_R_TYPE(rela->r_info)) {
    case R_TYPE(NONE):
      break;

    case R_TYPE(32):
      *(uint16_t *)where = ((symvalue + rela->r_addend) >> 16) & 0xffff;
      *((uint16_t *)where + 1) = (symvalue + rela->r_addend) & 0xffff;

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC)) {
          printf("*where 0x%04x%04x\n", *((uint16_t *)where + 1), *(uint16_t *)where);
      }
      break;

    case R_TYPE(PCREL10):
      /* beq, bge, bgeu, bgt, bgtu, ble, bleu, blt, bltu, bne */
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC)) {
        printf("*where %x\n", *(uint16_t *)where);
        printf("symvalue - where %x\n", (int)(symvalue - (Elf_Word)where));
      }
      tmp = (symvalue + rela->r_addend - ((Elf_Word)where + 2)); /* pc is the next instruction */
      tmp = (Elf_Sword)tmp >> 1;
      if (((Elf32_Sword)tmp > 0x1ff) || ((Elf32_Sword)tmp < -(Elf32_Sword)0x200)){
        printf("Overflow for PCREL10: %d exceed -0x200:0x1ff\n", tmp);
        return false;
      }

      *(uint16_t *)where = (*(uint16_t *)where & 0xfc00) | (tmp & 0x3ff);

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC)) {
          printf("*where 0x%04x\n",  *(uint16_t *)where);
      }

      break;

    default:
      rtems_rtl_set_error (EINVAL, "rela type record not supported");
      printf("Unsupported reloc types\n");
      return false;
  }

  return true;
}

bool
rtems_rtl_elf_relocate_rel (const rtems_rtl_obj_t*      obj,
                            const Elf_Rel*              rel,
                            const rtems_rtl_obj_sect_t* sect,
                            const char*                 symname,
                            const Elf_Byte              syminfo,
                            const Elf_Word              symvalue)
{
  rtems_rtl_set_error (EINVAL, "rel type record not supported");
  return false;
}
