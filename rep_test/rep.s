.global movMov
.global repMov

.text

movMov:
  movq %rcx, %rax
  movq %rdx, %rcx
  movq $123, %rax
  retq

repMov:
  movq %rcx, %rdi
  movq %rdx, %rcx
  movb $61, %al
  rep stosq
  retq

