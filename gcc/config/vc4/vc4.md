
(include "constraints.md")
(include "predicates.md")

(define_mode_iterator FIXED [QI HI SI])

;; Some output patterns want integer immediates with a prefix...
(define_mode_attr  iprefx [(QI "B") (HI "H") (SI "N")])

(define_constants
  [(R0_REGNUM         0)	; First CORE register
   (R1_REGNUM	      1)	; Second CORE register
   (LR_REGNUM         26)
   (PC_REGNUM         31)
   (LAST_ARM_REGNUM  31)	;
   (CC_REGNUM       100)	; Condition code pseudo register
  ]
)

(define_insn "jump"
  [(set (pc)
	(label_ref (match_operand 0 "" "")))]
  ""
  "jbr %l0")

;; The subtract-with-carry (sbwc) instruction only takes two operands.
(define_insn ""
  [(const_int 1)]
  "!TARGET_DUMMY"
  "nop")

(define_insn "add<mode>3"
  [(set (match_operand:FIXED 0 "nonimmediate_operand" "=r")
	(plus:FIXED (match_operand:FIXED 1 "general_operand" "")
		    (match_operand:FIXED 2 "general_operand" "")))]
  ""
  "add %0, %1, %2")

(define_insn "sub<mode>3"
  [(set (match_operand:FIXED 0 "nonimmediate_operand" "=r")
	(minus:FIXED (match_operand:FIXED 1 "general_operand" "r")
		     (match_operand:FIXED 2 "general_operand" "r")))]
  ""
  "sub %0, %1, %2")

; (define_insn "addsi3"
;   [(set (match_operand:SI 0 "nonimmediate_operand" "=r")
; 	(plus:SI (match_operand:SI 1 "general_operand" "%r")
;                  (match_operand:SI 2 "general_operand" "r")))]
;   ""
;   "add %r0, %r1, %r2")

(define_expand "call"
  [(parallel [(call (match_operand 0 "memory_operand" "m")
		    (match_operand 1 "" "i"))
             (clobber (reg:SI LR_REGNUM))
             (use (match_operand 2 "" ""))
             (use (match_operand 3 "" ""))])]
  ""
  {
    rtx addr = XEXP (operands[0], 0);

    if ((GET_CODE (addr) != REG && !CONSTANT_ADDRESS_P (addr))
        )
      XEXP (operands[0], 0) = copy_to_mode_reg (Pmode, addr);

    emit_call_insn (gen_call_internal (operands[0], operands[1],
                    gen_rtx_REG (SImode, LR_REGNUM)));
    
    DONE;
  }
)



(define_expand "call_internal"
  [(parallel [(call (match_operand 0 "memory_operand" "")
	            (match_operand 1 "general_operand" ""))
	      (use (match_operand 2 "" ""))
	      (clobber (reg:SI LR_REGNUM))])])

(define_insn "*call"
  [(call (mem (match_operand:SI 0 "" ""))
         (match_operand 1 "" ""))
   (use (match_operand 2 "" ""))
   (clobber (reg:SI LR_REGNUM))]
  ""
  "bl %0"
)

; (define_expand "return"
;   [(return)]
;   ""
;   ""
; )

; (define_insn "*return"
;   [(return
;    (set (reg:SI PC_REGNUM)
;         (mem:SI (post_inc:SI (match_operand:SI 0 "" "r"))))]
;   ""
;   "b lr"
; )

; (define_expand "epilogue"
;   [(clobber (const_int 0))]
;   ""
;   "
;     emit_jump_insn (gen_rtx_UNSPEC_VOLATILE (VOIDmode,
;                      gen_rtvec (1, ret_rtx), VUNSPEC_EPILOGUE));
;     DONE;
;   "
; )

(define_expand "call_value"
  [(parallel [(set (match_operand       0 "" "")
	           (call (match_operand 1 "memory_operand" "")
		         (match_operand 2 "general_operand" "")))
	      (use (match_operand 3 "" ""))])]
  ""
  "
  {
    rtx callee;
    
    callee = XEXP (operands[1], 0);

    XEXP (operands[1], 0) = force_reg (Pmode, callee);

    DONE;
  }"
)

(define_expand "call_internal"
  [(parallel [(call (match_operand 0 "memory_operand" "")
	            (match_operand 1 "general_operand" ""))
	      (use (match_operand 2 "" ""))
	      (clobber (reg:SI LR_REGNUM))])])

(define_insn "movsi"
  [(set (match_operand:SI 0 "general_operand" "=r,r,m")
        (match_operand:SI 1 "general_operand" "ri,m,r"))]
  ""
  "@
  mov %0, %1
  ld %0, %1
  st %1, %0")

(define_insn "movhi"
  [(set (match_operand:HI 0 "nonimmediate_operand" "")
        (match_operand:HI 1 "general_operand" ""))]
  ""
  "mov %0, %1")

(define_insn "movqi"
  [(set (match_operand:QI 0 "nonimmediate_operand" "")
        (match_operand:QI 1 "general_operand" ""))]
  ""
  "mov %0, %1")

(define_insn "movbi"
  [(set (match_operand:BI 0 "nonimmediate_operand" "")
        (match_operand:BI 1 "general_operand" ""))]
  ""
  "mov %0, %1")

(define_insn "nop"
  [(const_int 0)]
  ""
  "nop")

(define_constraint "T"
    "@internal satisfies CONSTANT_P and, if pic is enabled, is not a SYMBOL_REF, LABEL_REF, or CONST."
   (ior (not (match_code "const,symbol_ref,label_ref"))
	(match_test "!flag_pic")))

;; 'F' constraint means type CONST_DOUBLE
(define_insn ""
  [(set (match_operand:DI 0 "nonimmediate_operand" "=g")
	(plus:DI
	 (mult:DI (sign_extend:DI
		   (match_operand:SI 1 "nonimmediate_operand" "nrmT"))
		  (sign_extend:DI
		   (match_operand:SI 2 "nonimmediate_operand" "nrmT")))
	 (match_operand:DI 3 "immediate_operand" "F")))]
  "GET_CODE (operands[3]) == CONST_DOUBLE
    && CONST_DOUBLE_HIGH (operands[3]) == (CONST_DOUBLE_LOW (operands[3]) >> 31)"
  "*
{
  if (CONST_DOUBLE_HIGH (operands[3]))
    operands[3] = GEN_INT (CONST_DOUBLE_LOW (operands[3]));
  return \"emul %1,%2,%3,%0\";
}")

;; This had a wider constraint once, and it had trouble.
;; If you are tempted to try `g', please don't--it's not worth
;; the risk we will reopen the same bug.
(define_insn "indirect_jump"
  [(set (pc) (match_operand:SI 0 "register_operand" "r"))]
  ""
  "jmp (%0)")

;; 'F' constraint means type CONST_DOUBLE
(define_insn ""
  [(set (match_operand:DI 0 "nonimmediate_operand" "=g")
	(minus:DI
	 (mult:DI (sign_extend:DI
		   (match_operand:SI 1 "nonimmediate_operand" "J"))
		  (sign_extend:DI
		   (match_operand:SI 2 "nonimmediate_operand" "J")))
	 (match_operand:DI 3 "immediate_operand" "F")))]
  ""
  "")

; (define_insn ""
;   [(set (match_operand:SI 0 "nonimmediate_operand" "=g")
; 	(sign_extract:SI (match_operand:SI 1 "register_operand" "ro")
; 			 (match_operand:QI 2 "general_operand" "g")
; 			 (match_operand:SI 3 "general_operand" "nrmT")))]
;   ""
;   "*
; {
;   if (! CONST_INT_P (operands[3]) || ! CONST_INT_P (operands[2])
;       || ! REG_P (operands[0])
;       || (INTVAL (operands[2]) != 8 && INTVAL (operands[2]) != 16))
;     return \"extv %3,%2,%1,%0\";
;   if (INTVAL (operands[2]) == 8)
;     return \"rotl %R3,%1,%0\;cvtbl %0,%0\";
;   return \"rotl %R3,%1,%0\;cvtwl %0,%0\";
; }")

; (define_insn ""
;   [(set (match_operand:SI 0 "nonimmediate_operand" "=g")
; 	(sign_extract:SI (match_operand:SI 1 "register_operand" "ro")
; 			 (match_operand:QI 2 "general_operand" "n")
; 			 (match_operand:SI 3 "general_operand" "nrmT")))]
;   ""
;   "*
; {
;   if (! CONST_INT_P (operands[3]) || ! CONST_INT_P (operands[2])
;       || ! REG_P (operands[0])
;       || (INTVAL (operands[2]) != 8 && INTVAL (operands[2]) != 16))
;     return \"extv %3,%2,%1,%0\";
;   if (INTVAL (operands[2]) == 8)
;     return \"rotl %R3,%1,%0\;cvtbl %0,%0\";
;   return \"rotl %R3,%1,%0\;cvtwl %0,%0\";
; }")

