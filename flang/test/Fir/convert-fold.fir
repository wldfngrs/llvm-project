// RUN: fir-opt --canonicalize %s | FileCheck %s

// CHECK-LABEL: @ftest
func.func @ftest(%x : i1) -> i1 {
  // this pair of converts should be folded and DCEd
  %1 = fir.convert %x : (i1) -> !fir.logical<1>
  %2 = fir.convert %1 : (!fir.logical<1>) -> i1
  // CHECK-NEXT: return %{{.*}} : i1
  return %2 : i1
}

// CHECK-LABEL: @gtest
func.func @gtest(%x : !fir.logical<2>) -> !fir.logical<2> {
  // this pair of converts should be folded and DCEd
  %1 = fir.convert %x : (!fir.logical<2>) -> i1
  %2 = fir.convert %1 : (i1) -> !fir.logical<2>
  // CHECK-NEXT: return %{{.*}} : !fir.logical<2>
  return %2 : !fir.logical<2>
}

// CHECK-LABEL: @htest
func.func @htest(%x : !fir.int<4>) -> !fir.int<4> {
  // these converts are NOPs and should be folded away
  %1 = fir.convert %x : (!fir.int<4>) -> !fir.int<4>
  %2 = fir.convert %1 : (!fir.int<4>) -> !fir.int<4>
  // CHECK-NEXT: return %{{.*}} : !fir.int<4>
  return %2 : !fir.int<4>
}

// CHECK-LABEL: @ctest
func.func @ctest() -> index {
  %1 = arith.constant 10 : i32
  %2 = fir.convert %1 : (i32) -> index
  // CHECK-NEXT: %{{.*}} = arith.constant 10 : index
  // CHECK-NEXT: return %{{.*}} : index
  return %2 : index
}

// CHECK-LABEL:   func.func @ptrtest(
// CHECK-SAME:                       %[[VAL_0:.*]]: !fir.heap<!fir.array<2xf32>>) -> !fir.heap<!fir.array<2xf32>> {
func.func @ptrtest(%0 : !fir.heap<!fir.array<2xf32>>) -> !fir.heap<!fir.array<2xf32>> {
  %1 = fir.convert %0 : (!fir.heap<!fir.array<2xf32>>) -> !fir.ref<!fir.array<2xf32>>
  %2 = fir.convert %1 : (!fir.ref<!fir.array<2xf32>>) -> !fir.heap<!fir.array<2xf32>>
// CHECK:           return %[[VAL_0]] : !fir.heap<!fir.array<2xf32>>
  return %2 : !fir.heap<!fir.array<2xf32>>
}
