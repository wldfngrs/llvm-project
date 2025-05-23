; RUN: llvm-mc -triple avr -mattr=sram -show-encoding < %s | FileCheck %s
; RUN: llvm-mc -filetype=obj -triple avr -mattr=sram < %s | llvm-objdump --no-print-imm-hex -dr --mattr=sram - | FileCheck -check-prefix=CHECK-INST %s

foo:
  lds r16, 241
  lds r29, 190
  lds r22, 172
  lds r27, 92
  lds r4,  SYMBOL+12
  lds r4,  r25
  lds r4,  x+2

; CHECK: lds r16, 241                 ; encoding: [0x00,0x91,0xf1,0x00]
; CHECK: lds r29, 190                 ; encoding: [0xd0,0x91,0xbe,0x00]
; CHECK: lds r22, 172                 ; encoding: [0x60,0x91,0xac,0x00]
; CHECK: lds r27, 92                  ; encoding: [0xb0,0x91,0x5c,0x00]
; CHECK: lds r4,  SYMBOL+12           ; encoding: [0x40,0x90,A,A]
; CHECK: lds r4,  r25                 ; encoding: [0x40,0x90,A,A]
; CHECK: lds r4,  x+2                 ; encoding: [0x40,0x90,A,A]

; CHECK-INST: lds r16, 241
; CHECK-INST: lds r29, 190
; CHECK-INST: lds r22, 172
; CHECK-INST: lds r27, 92
; CHECK-INST: lds r4,  0
; CHECK-INST:          R_AVR_16 SYMBOL+0xc
; CHECK-INST: lds r4,  0
; CHECK-INST:          R_AVR_16 r25
; CHECK-INST: lds r4,  0
; CHECK-INST:          R_AVR_16 x+0x2
