  ; nasmfunc.asm

  bits 32
  global io_hlt
  global io_cli
  global io_sti
  global io_stihlt
  global io_in8
  global io_in16
  global io_in32
  global io_out8
  global io_out16
  global io_out32
  global io_load_eflags
  global io_store_eflags
  global load_gdtr
  global load_idtr
  global asm_inthandler0c
  global asm_inthandler0d
  global asm_inthandler20
  global asm_inthandler21
  global asm_inthandler27
  global asm_inthandler2c
  global load_cr0
  global store_cr0
  global load_tr
  global farjmp
  global farcall
  global asm_cons_putchar
  global asm_hrb_api
  global asm_end_app
  global start_app

  extern inthandler0c
  extern inthandler0d
  extern inthandler20
  extern inthandler21
  extern inthandler27
  extern inthandler2c
  extern cons_putchar
  extern hrb_api

  section .text

io_hlt: ; void io_hlt(void);
  HLT
  RET

io_cli: ; void io_cli(void);
  CLI ; set interrupt flag to 0
  RET

io_sti: ; void io_sti(void);
  STI ; Set interrupt flag to 1
  RET

io_stihlt: ; void io_stihlt(void);
  STI
  HLT
  RET

io_in8: ; int io_in8(int port);
  MOV EDX, [ESP+4] ; Port
  MOV EAX, 0
  IN AL, DX
  RET

io_in16: ; int io_in16(int port);
  MOV EDX, [ESP+4] ; Port
  MOV EAX, 0
  IN AX, DX
  RET

io_in32: ; int io_in32(int port);
  MOV EDX, [ESP+4] ; Port
  IN EAX, DX
  RET

io_out8: ; void io_out8(int port, int data);
  MOV EDX, [ESP+4] ; Port
  MOV AL, [ESP+8] ; Data
  OUT DX, AL
  RET

io_out16: ; void io_out16(int port, int data);
  MOV EDX, [ESP+4] ; Port
  MOV AX, [ESP+8] ; Data
  OUT DX, AX
  RET

io_out32: ; void io_out32(int port, int data);
  MOV EDX, [ESP+4] ; Port
  MOV EAX, [ESP+8] ; Data
  OUT DX, EAX
  RET

io_load_eflags: ; int io_load_eflags(void);
  PUSHFD ; Push flags double-word
  POP EAX
  RET

io_store_eflags: ; void io_store_eflags(int eflags);
  MOV EAX, [ESP+4]
  PUSH EAX
  POPFD ; Pop flags double-word
  RET ; Return EAX value when RET is called

load_gdtr: ; void load_gdtr(int limit, int addr);
  MOV AX, [ESP+4] ; limit
  MOV [ESP+6], AX
  LGDT [ESP+6] ; Set limit and number to GDTR register
  RET

load_idtr: ; void load_idtr(int limit, int addr);
  MOV AX, [ESP+4] ; limit
  MOV [ESP+6], AX
  LIDT [ESP+6]
  RET

asm_inthandler0c:
  STI
  PUSH ES
  PUSH DS
  PUSHAD
  MOV EAX, ESP
  PUSH EAX
  MOV AX, SS
  MOV DS, AX
  MOV ES, AX
  CALL inthandler0c
  CMP EAX, 0
  JNE asm_end_app
  POP EAX
  POPAD
  POP DS
  POP ES
  ADD ESP, 4 ; INT 0x0c
  IRETD

asm_inthandler0d:
  STI
  PUSH ES
  PUSH DS
  PUSHAD
  MOV EAX, ESP
  PUSH EAX
  MOV AX, SS
  MOV DS, AX
  MOV ES, AX
  CALL inthandler0d
  CMP EAX,0
  JNE asm_end_app
  POP EAX
  POPAD
  POP DS
  POP ES
  ADD ESP,4
  IRETD

asm_inthandler20:
  PUSH ES
  PUSH DS
  PUSHAD
  MOV EAX, ESP
  PUSH EAX
  MOV AX, SS
  MOV DS, AX
  MOV ES, AX
  CALL inthandler20
  POP EAX
  POPAD
  POP DS
  POP ES
  IRETD ; Execute IRETD when the end of interruption instead of RET(=return)

asm_inthandler21:
  PUSH ES
  PUSH DS
  PUSHAD
  MOV EAX, ESP
  PUSH EAX
  MOV AX, SS
  MOV DS, AX
  MOV ES, AX
  CALL inthandler21
  POP EAX
  POPAD
  POP DS
  POP ES
  IRETD ; Execute IRETD when the end of interruption instead of RET(=return)

asm_inthandler27:
  PUSH ES
  PUSH DS
  PUSHAD
  MOV EAX, ESP
  PUSH EAX
  MOV AX, SS
  MOV DS, AX
  MOV ES, AX
  CALL inthandler27
  POP EAX
  POPAD
  POP DS
  POP ES
  IRETD

asm_inthandler2c:
  PUSH ES
  PUSH DS
  PUSHAD
  MOV EAX, ESP
  PUSH EAX
  MOV AX, SS
  MOV DS, AX
  MOV ES, AX
  CALL inthandler2c
  POP EAX
  POPAD
  POP DS
  POP ES
  IRETD

load_cr0: ; int load_cr0(void);
  MOV EAX, CR0
  RET

store_cr0: ; void store_cr0(int cr0);
  MOV EAX, [ESP+4]
  MOV CR0, EAX
  RET

load_tr: ; void load_tr(int tr);
  LTR [ESP+4] ; tr
  RET

farjmp: ; void farjmp(int eip, int cs);
  JMP FAR [ESP+4] ; sip, cs
  RET

farcall: ; void farcall(int eip, int cs);
  CALL FAR [ESP+4] ; eip, cs
  RET

asm_cons_putchar:
  STI ; Workaround to prevent CLI
  PUSHAD ; Push general-purpose registers to avoid changing exc register.
  PUSH 1
  AND EAX, 0xff ; Fill char code in EAX
  PUSH EAX
  PUSH DWORD [0x0fec] ; Read content in a memory and push it.
  CALL cons_putchar
  ADD ESP, 12 ; Dump data of stack.
  POPAD
  IRETD ; Return from interruption

asm_hrb_api:
  STI
  PUSH DS
  PUSH ES
  PUSHAD ; Store general-purpose registers.
  PUSHAD ; Pass hrb_api.
  MOV AX, SS
  MOV DS, AX ; Store the segment for OS into DS and ES.
  MOV ES, AX
  CALL hrb_api
  CMP EAX, 0 ; Quit an app if EAX is not 0.
  JNE asm_end_app
  ADD ESP, 32
  POPAD
  POP ES
  POP DS
  IRETD

asm_end_app:
  ; EAX is the address of tss.esp0
  MOV ESP, [EAX]
  MOV DWORD [EAX+4], 0
  POPAD
  RET ; Return cmd_app().

start_app: ; void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);
  PUSHAD
  MOV EAX, [ESP+36] ; EIP for an app
  MOV ECX, [ESP+40] ; CS for an app
  MOV EDX, [ESP+44] ; ESP for an app
  MOV EBX, [ESP+48] ; DS/SS for an app
  MOV EBP, [ESP+52] ; tss.esp0 's address
  MOV [EBP], ESP ; Store ESP of OS
  MOV [EBP+4], SS ; Store SS of OS
  MOV ES, BX
  MOV DS, BX
  MOV FS, BX
  MOV GS, BX

  ; Call app's code by RETF
  OR ECX, 3 ; OR the segment number of an app
  OR EBX, 3 ; OR the segment number of an app
  PUSH EBX ; SS for an app
  PUSH EDX ; ESP for an app
  PUSH ECX ; CS for an app
  PUSH EAX ; EIP for an app
  RETF
  ; No app is here after finish it.
