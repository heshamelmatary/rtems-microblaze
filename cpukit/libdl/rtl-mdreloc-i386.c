/*
 * Taken from NetBSD and stripped of the relocations not needed on RTEMS.
 */

/*  $NetBSD: mdreloc.c,v 1.31 2010/01/14 11:58:32 skrll Exp $  */

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
                             const Elf_Rela*             rel,
                             const rtems_rtl_obj_sect_t* sect,
                             const char*                 symname,
                             const Elf_Byte              syminfo,
                             const Elf_Word              symvalue)
{
  rtems_rtl_set_error (EINVAL, "rela type record not supported");
  return false;
}

bool
rtems_rtl_elf_relocate_rel (const rtems_rtl_obj_t*      obj,
                            const Elf_Rel*              rel,
                            const rtems_rtl_obj_sect_t* sect,
                            const char*                 symname,
                            const Elf_Byte              syminfo,
                            const Elf_Word              symvalue)
{
	Elf_Addr  target = 0;
  Elf_Addr* where;
  Elf_Addr  tmp;

  where = (Elf_Addr *)(sect->base + rel->r_offset);

  switch (ELF_R_TYPE(rel->r_info)) {
    case R_TYPE(NONE):
      break;

    case R_TYPE(PC32):
      target = (Elf_Addr) symvalue;
      *where += target - (Elf_Addr)where;

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: reloc PC32 in %s --> %p (%p @ %p) in %s\n",
                sect->name, (void*) symvalue,
                (void *)*where, where, rtems_rtl_obj_oname (obj));
      break;

    case R_TYPE(GOT32):
    case R_TYPE(32):
    case R_TYPE(GLOB_DAT):
      target = (Elf_Addr) symvalue;

      tmp = target + *where;
      if (*where != tmp)
        *where = tmp;
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: reloc 32/GLOB_DAT in %s --> %p @ %p in %s\n",
                sect->name, (void *)*where, where,
                rtems_rtl_obj_oname (obj));
      break;

    case R_TYPE(RELATIVE):
      *where += (Elf_Addr)sect->base;
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: reloc RELATIVE in %s --> %p @ %p\n",
                rtems_rtl_obj_oname (obj), (void *)*where, where);
      break;

    case R_TYPE(COPY):
      printf ("rtl: reloc COPY (please report)\n");
      break;

    default:
      printf ("rtl: reloc unknown: sym = %lu, type = %lu, offset = %p, "
              "contents = %p\n",
              ELF_R_SYM(rel->r_info), (uint32_t) ELF_R_TYPE(rel->r_info),
              (void *)rel->r_offset, (void *)*where);
      rtems_rtl_set_error (EINVAL,
                           "%s: Unsupported relocation type %ld "
                           "in non-PLT relocations",
                           sect->name, (uint32_t) ELF_R_TYPE(rel->r_info));
      return false;
  }

  return true;
}
