
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
  Elf_Word tmp;

  where = (Elf_Addr *)(sect->base + rela->r_offset);

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC)) {
      printf("rela relocation type is %ld\n", ELF_R_TYPE(rela->r_info));
      printf("relocated address 0x%08lx\n", (Elf_Addr)where);
  }

  switch (ELF_R_TYPE(rela->r_info)) {
    case R_TYPE(NONE):
      break;

    case R_TYPE(HI16_S):
      tmp = (Elf_Sword)(symvalue + rela->r_addend) >> 16;
      ((uint16_t *)where)[0] = tmp & 0xffff;
      break;

    case R_TYPE(LO16):
      tmp = symvalue + rela->r_addend;
      ((uint16_t *)where)[0] = tmp & 0xffff;
      break;

    case R_TYPE(LO16_S1):
      tmp = symvalue + rela->r_addend;
      ((uint16_t *)where)[0] = tmp & 0xfffe | 0x1;
      break;

    case R_TYPE(22_PCREL):
      tmp =  symvalue + rela->r_addend - (Elf_Addr)where;
      if (((Elf_Sword)tmp > 0x1fffff) || ((Elf_Sword)tmp < -0x200000)) {
        printf("Overflow\n");
        return false;
      }

      ((uint16_t *)where)[0] = (*(uint16_t *)where & 0xffc0) |
        ((tmp >> 16) & 0x3f);
      ((uint16_t *)where)[1] = (tmp & 0xfffe);

      break;

    case R_TYPE(ABS32):
      tmp = symvalue + rela->r_addend;
      tmp += ((uint16_t *)where)[0];
      tmp += ((uint16_t *)where)[1] << 16;
      ((uint16_t *)where)[0] = tmp & 0xffff;
      ((uint16_t *)where)[1] = (tmp >> 16) & 0xffff;
      break;

    default:
      rtems_rtl_set_error (EINVAL, "rela type record not supported");
      printf("error reloc type\n");
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
