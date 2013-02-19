
(define_predicate "local_symbolic_operand"
  (match_code "const,symbol_ref,label_ref")
{
  if (GET_CODE (op) == LABEL_REF)
    return 1;
  if (GET_CODE (op) == SYMBOL_REF)
    return !flag_pic || SYMBOL_REF_LOCAL_P (op);
  if (GET_CODE (XEXP (XEXP (op, 0), 0)) == LABEL_REF)
    return 1;
  return !flag_pic || SYMBOL_REF_LOCAL_P (XEXP (XEXP (op, 0), 0));
})

(define_predicate "external_symbolic_operand"
  (and (match_code "symbol_ref")
       (not (match_operand 0 "local_symbolic_operand" ""))))

