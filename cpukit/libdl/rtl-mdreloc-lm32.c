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
  Elf32_Word tmp;
  Elf32_Word insn;


  where = (Elf_Addr *)(sect->base + rela->r_offset);

  switch (ELF_R_TYPE(rela->r_info)) {
    case R_TYPE(NONE):
      break;

    case R_TYPE(HI16):
      insn = *where;
      /* orhi/mvhi instruction
       *  31--------26|25-21|20-16|15----0|
       * |0 1 1 1 1 0 |rY   |rX   |imm16  |
       */
      if (0x1e == (insn >> 26)) {
        insn &= 0xffff0000;
        insn |= ((symvalue + rela->r_addend) >> 16);
        *where = insn;
      }
      break;

    case R_TYPE(LO16):
      insn = *where;
      /* ori instruction
       *  31--------26|25-21|20-16|15----0|
       * |0 0 1 1 1 0 |rY   |rX   |imm16  |
       */
      if (0xe == (insn >> 26)) {
        insn &= 0xffff0000;
        insn |= ((symvalue + rela->r_addend) & 0xffff);
        *where = insn;
      }
      break;

    case R_TYPE(CALL):
      insn = *where;
      /*
       * calli instruction
       *  31-------26|25---0|
       * |1 1 1 1 1 0|imm26 |
       * Syntax: call imm26
       * Operation: ra = pc + 4; pc = pc + sign_extend(imm26<<2)
       */
      if (0x3e == (insn >> 26)) {
        Elf_Sword imm26 = symvalue +rela->r_addend - (Elf_Addr)where;
        imm26 = (imm26 >> 2) & 0x3ffffff;
        insn = 0xf8000000 + imm26;
        *where = insn;
      }
      break;

    case R_TYPE(BRANCH):
      insn = *where;
      tmp = symvalue + rela->r_addend - (Elf_Addr)where;
      tmp = (Elf32_Sword)tmp >> 2;
      if (((Elf32_Sword)tmp > 0x7fff) || ((Elf32_Sword)tmp < -0x8000)){
        printf("BRANCH Overflow\n");
        return false;
      }

      *where = (*where & 0xffff0000) | (tmp & 0xffff);
      break;

    case R_TYPE(32):
      *where = symvalue + rela->r_addend;
      break;

    default:
      rtems_rtl_set_error (EINVAL, "rela type record not supported");
      printf("Unsupported reloc types\n");
      return false;
  }

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC)) {
      printf("rela relocation type is %ld\n", ELF_R_TYPE(rela->r_info));
      printf("relocated address 0x%08lx\n", (Elf_Addr)where);
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
  rtems_rtl_set_error (EINVAL, "rela type record not supported");
  return false;
}
