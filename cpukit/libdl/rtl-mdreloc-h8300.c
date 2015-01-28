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
  Elf_Addr	*where;
  Elf_Word	tmp;

  where = (Elf_Addr *)(sect->base + rela->r_offset);

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC)) {
      printf("rela relocation type is %ld\n", ELF_R_TYPE(rela->r_info));
      printf("relocated address 0x%08lx\n", (Elf_Addr)where);
  }

  tmp = symvalue;
  switch (ELF_R_TYPE(rela->r_info)) {
    case R_TYPE(NONE):
      break;

    case R_TYPE(DIR16):
      *(uint16_t *)where += symvalue + rela->r_addend;
      break;

    case R_TYPE(DIR32):
    case R_TYPE(DIR32A16):
      *where += symvalue + rela->r_addend;
      break;

    case R_TYPE(DIR24A8):
      if (ELF32_R_SYM(rela->r_info))
        *where += symvalue + rela->r_addend;
      break;

    case R_TYPE(DIR24R8):
      where = (uint32_t *)((uint32_t)where - 1);
      *where = (*where & 0xff000000) | ((*where & 0xffffff) + symvalue + rela->r_addend);
      break;

    case R_TYPE(PCREL8):
      /* bcc instruction */
      tmp = symvalue + rela->r_addend - (Elf_Addr)where - 1;
      if (((Elf32_Sword)tmp > 0x7f) || ((Elf32_Sword)tmp < -(Elf32_Sword)0x80)){
        printf("PCREL8 overflow\n");
          return false;
      } else {
        *(uint8_t *)where = tmp;
      }
      break;

    case R_TYPE(PCREL16):
      /* bcc instruction */
      tmp = symvalue + rela->r_addend - (Elf_Addr)where - 2;
      if (((Elf32_Sword)tmp > 0x7fff) || ((Elf32_Sword)tmp < -(Elf32_Sword)0x8000)){
        printf("PCREL16 overflow\n");
       return false;
      } else {
       *(uint16_t *)where = tmp;
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
