; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc -mtriple=hexagon < %s | FileCheck %s

; Check that we generate zero-extends, instead of just shifting and oring
; registers (which can contain sign-extended negative values).

define i32 @fred(i8 %a0, i8 %a1, i8 %a2, i8 %a3) #0 {
; CHECK-LABEL: fred:
; CHECK:       // %bb.0: // %b4
; CHECK-NEXT:    {
; CHECK-NEXT:     r1 = and(r1,#255)
; CHECK-NEXT:     r3 = and(r3,#255)
; CHECK-NEXT:    }
; CHECK-NEXT:    {
; CHECK-NEXT:     r0 = insert(r1,#24,#8)
; CHECK-NEXT:     r2 = insert(r3,#24,#8)
; CHECK-NEXT:    }
; CHECK-NEXT:    {
; CHECK-NEXT:     r0 = combine(r2.l,r0.l)
; CHECK-NEXT:     jumpr r31
; CHECK-NEXT:    }
b4:
  %v5 = insertelement <4 x i8> undef, i8 %a0, i32 0
  %v6 = insertelement <4 x i8> %v5, i8 %a1, i32 1
  %v7 = insertelement <4 x i8> %v6, i8 %a2, i32 2
  %v8 = insertelement <4 x i8> %v7, i8 %a3, i32 3
  %v9 = bitcast <4 x i8> %v8 to i32
  ret i32 %v9
}

attributes #0 = { nounwind readnone }
