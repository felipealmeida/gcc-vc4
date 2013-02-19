
#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "rtl.h"
#include "regs.h"
#include "hard-reg-set.h"
#include "insn-config.h"
#include "conditions.h"
#include "insn-flags.h"
#include "output.h"
#include "insn-attr.h"
#include "flags.h"
#include "recog.h"
#include "reload.h"
#include "diagnostic-core.h"
#include "obstack.h"
#include "tree.h"
#include "expr.h"
#include "optabs.h"
#include "except.h"
#include "function.h"
#include "ggc.h"
#include "target.h"
#include "target-def.h"
#include "tm_p.h"
#include "langhooks.h"
#include "df.h"
          
/* Initialize the GCC target structure.  */
          
//#undef TARGET_COMP_TYPE_ATTRIBUTES
//#define TARGET_COMP_TYPE_ATTRIBUTES vc4_comp_type_attributes

static void
pa_globalize_label (FILE *stream, const char *name)
{
  /* /\* We only handle DATA objects here, functions are globalized in */
  /*    ASM_DECLARE_FUNCTION_NAME.  *\/ */
  /* if (! FUNCTION_NAME_P (name)) */
  /* { */
    fputs ("\t.EXPORT ", stream);
    assemble_name (stream, name);
    fputs (",DATA\n", stream);
  /* } */
}

/* This function is used to implement GO_IF_LEGITIMATE_ADDRESS.  It
   returns a nonzero value if X is a legitimate address for a memory
   operand of the indicated MODE.  STRICT is nonzero if this function
   is called during reload.  */

bool
vc4_legitimate_address_p (enum machine_mode mode, rtx x, bool strict)
{
  switch (GET_CODE (x))
    {
    case REG:
    case SUBREG:
      {
        int regno = REGNO (x);
        if (strict)
          return REGNO_OK_FOR_BASE_P (regno);
        return  regno >= FIRST_PSEUDO_REGISTER;
      }
    case PLUS:
      {
        return false;
      }
    case CONST_INT:
      {
	return true;
      }
    case CONST:
    case LABEL_REF:
    case SYMBOL_REF:
      {
	return true;
      }

    /* case UNSPEC: */
    /*   { */
    /*     if (reload_in_progress) */
    /*       df_set_regs_ever_live (PIC_OFFSET_TABLE_REGNUM, true); */
    /*     return microblaze_classify_unspec (info, x); */
    /*   } */

    default:
      return false;
    }

  return false;
}

static rtx
arm_function_value(const_tree type, const_tree func,
		   bool outgoing ATTRIBUTE_UNUSED)
{
  enum machine_mode mode;
  int unsignedp ATTRIBUTE_UNUSED;
  rtx r ATTRIBUTE_UNUSED;

  mode = SImode;
  return gen_rtx_REG (mode, R0_REGNUM);

  /* if (TARGET_AAPCS_BASED) */
  /*   return aapcs_allocate_return_reg (mode, type, func); */

  /* /\* Promote integer types.  *\/ */
  /* if (INTEGRAL_TYPE_P (type)) */
  /*   mode = arm_promote_function_mode (type, mode, &unsignedp, func, 1); */

  /* /\* Promotes small structs returned in a register to full-word size */
  /*    for big-endian AAPCS.  *\/ */
  /* if (arm_return_in_msb (type)) */
  /*   { */
  /*     HOST_WIDE_INT size = int_size_in_bytes (type); */
  /*     if (size % UNITS_PER_WORD != 0) */
  /*       { */
  /*         size += UNITS_PER_WORD - size % UNITS_PER_WORD; */
  /*         mode = mode_for_size (size * BITS_PER_UNIT, MODE_INT, 0); */
  /*       } */
  /*   } */

  /* return arm_libcall_value_1 (mode); */
}

#undef TARGET_LEGITIMATE_ADDRESS_P
#define TARGET_LEGITIMATE_ADDRESS_P 	vc4_legitimate_address_p 

#undef  TARGET_FUNCTION_VALUE
#define TARGET_FUNCTION_VALUE arm_function_value

#undef TARGET_PRINT_OPERAND
#define TARGET_PRINT_OPERAND vc4_print_operand
#undef TARGET_PRINT_OPERAND_ADDRESS
#define TARGET_PRINT_OPERAND_ADDRESS vc4_print_operand_address
#undef TARGET_PRINT_OPERAND_PUNCT_VALID_P
#define TARGET_PRINT_OPERAND_PUNCT_VALID_P vc4_print_operand_punct_valid_p

static void
vc4_print_condition (FILE *stream)
{
  fputs("condition code", stream);
}

static void
vc4_print_operand (FILE *stream, rtx x, int code)
{
  switch(code)
  {
  /* An integer that we want to print in HEX.  */
  /* case 'x': */
  /*   switch (GET_CODE (x)) */
  /*   { */

  /*   default: */
  /*     output_operand_lossage ("Unsupported operand for code '%c'", code); */
  /*   } */
  /*   return; */
  case '?':
    vc4_print_condition (stream);
    return;
  default:
    if (x == 0)
    {
      
      output_operand_lossage ("missing operand code: %d", code);
      return;
    }

    switch(GET_CODE(x))
    {
    case CONST_INT:
      fprintf (stream, HOST_WIDE_INT_PRINT_HEX, INTVAL (x));
      break;

    case REG:
      fprintf (stream, "r%d", REGNO (x));
      break;

    case MEM:
      /* output_memory_reference_mode = GET_MODE (x); */
      output_address (XEXP (x, 0));
      break;

    case CONST_DOUBLE:
      fprintf (stream, "const_double");
      break;

    case SYMBOL_REF:
      fprintf (stream, "%s", XSTR (x, 0));
      break;

    default:
      fprintf(stream, "print_operand %d %d", code, GET_CODE(x));

      /* gcc_assert (GET_CODE (x) != NEG); */
      /* fputc ('#', stream); */
      /* if (GET_CODE (x) == HIGH) */
      /*   { */
      /*     fputs (":lower16:", stream); */
      /*     x = XEXP (x, 0); */
      /*   } */
      
      /* output_addr_const (stream, x); */
      break;
    }
  }
    
}

static void
vc4_print_operand_address (FILE *stream, rtx x)
{
  if(REG_P(x))
  {
    fprintf(stream, "(r%d)", REGNO(x));
  }
  else if(GET_CODE(x) == PLUS)
  {
    fputs("is plus", stream);
  }
  else
    fputs("print_operand_address", stream);
}

static bool
vc4_print_operand_punct_valid_p (unsigned char code)
{
  return true;
}

static rtx
vc4_function_arg (cumulative_args_t pcum_v, enum machine_mode mode,
		  const_tree type, bool named)
{
  return NULL_RTX;
}

static void
vc4_output_function_epilogue (FILE *stream,
			      HOST_WIDE_INT frame_size ATTRIBUTE_UNUSED)
{

  const char* fnname;
  fnname = XSTR (XEXP (DECL_RTL (current_function_decl), 0), 0);

  fprintf(stream, "\tb lr\n");
  //  fputs ("\t.end\t", stream);
  /* assemble_name (stream, fnname); */
  /* fputs ("\n", stream); */
}


#undef TARGET_FUNCTION_ARG
#define TARGET_FUNCTION_ARG vc4_function_arg

/* #undef  TARGET_ASM_FUNCTION_PROLOGUE */
/* #define TARGET_ASM_FUNCTION_PROLOGUE vc4_output_function_prologue */

#undef  TARGET_ASM_FUNCTION_EPILOGUE
#define TARGET_ASM_FUNCTION_EPILOGUE vc4_output_function_epilogue

struct gcc_target targetm = TARGET_INITIALIZER;

