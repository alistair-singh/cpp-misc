.global movMov
.global repMov

.text

movMov:
  movq $123, %rax
  retq

repMov:
  movq $789, %rax
  retq

