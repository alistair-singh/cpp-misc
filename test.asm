EXTRN GetStdHandle: PROC
EXTRN WriteFile:    PROC
EXTRN lstrlen:      PROC
EXTRN ExitProcess:  PROC

.DATA

    hFile        QWORD 0
    msglen       DWORD 0
    BytesWritten DWORD 0
    msg          BYTE  "Hello x64 World!", 13, 10, 0

.CODE

    _test PROC

      and   ecx, -2147483647
      jge   SHORT $LN3@test
      dec   ecx
      or    ecx, -2
      inc   ecx
      $LN3@test:
      test  ecx, ecx
      mov   eax, 2
      mov   edx, 4
      cmovne eax, edx
      ret   0

    _test ENDP

    _test2 PROC

      and   ecx, 1
      lea   eax, DWORD PTR [rcx*2+2]
      ret   0

    _test2 ENDP


    main PROC

      ;int 3              ; breakpoint for debugger

      sub rsp, 28h

      mov ecx, 3
      call _test
      mov ecx, 3
      call _test2
      mov ecx, 2
      call _test2

      lea rcx, msg
      call lstrlen
      mov msglen, eax

      mov ecx, -11        ; STD_OUTPUT
      call GetStdHandle
      mov hFile, rax

      lea r9, BytesWritten
      mov r8d, msglen
      lea rdx, msg
      mov rcx, hFile
      call WriteFile

      xor ecx, ecx        ; exit code = 0
      call ExitProcess

    main ENDP

END
