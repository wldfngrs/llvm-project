# NOTE: Assertions have been autogenerated by utils/update_mca_test_checks.py
# RUN: llvm-mca -mtriple=x86_64-unknown-unknown -mcpu=sapphirerapids -instruction-tables < %s | FileCheck %s

crc32b      %al, %ecx
crc32b      (%rax), %ecx

crc32l      %eax, %ecx
crc32l      (%rax), %ecx

crc32w      %ax, %ecx
crc32w      (%rax), %ecx

crc32b      %al, %rcx
crc32b      (%rax), %rcx

crc32q      %rax, %rcx
crc32q      (%rax), %rcx

pcmpestri   $1, %xmm0, %xmm2
pcmpestri   $1, (%rax), %xmm2

pcmpestrm   $1, %xmm0, %xmm2
pcmpestrm   $1, (%rax), %xmm2

pcmpistri   $1, %xmm0, %xmm2
pcmpistri   $1, (%rax), %xmm2

pcmpistrm   $1, %xmm0, %xmm2
pcmpistrm   $1, (%rax), %xmm2

pcmpgtq     %xmm0, %xmm2
pcmpgtq     (%rax), %xmm2

# CHECK:      Instruction Info:
# CHECK-NEXT: [1]: #uOps
# CHECK-NEXT: [2]: Latency
# CHECK-NEXT: [3]: RThroughput
# CHECK-NEXT: [4]: MayLoad
# CHECK-NEXT: [5]: MayStore
# CHECK-NEXT: [6]: HasSideEffects (U)

# CHECK:      [1]    [2]    [3]    [4]    [5]    [6]    Instructions:
# CHECK-NEXT:  1      3     1.00                        crc32b	%al, %ecx
# CHECK-NEXT:  2      8     1.00    *                   crc32b	(%rax), %ecx
# CHECK-NEXT:  1      3     1.00                        crc32l	%eax, %ecx
# CHECK-NEXT:  2      8     1.00    *                   crc32l	(%rax), %ecx
# CHECK-NEXT:  1      3     1.00                        crc32w	%ax, %ecx
# CHECK-NEXT:  2      8     1.00    *                   crc32w	(%rax), %ecx
# CHECK-NEXT:  1      3     1.00                        crc32b	%al, %rcx
# CHECK-NEXT:  2      8     1.00    *                   crc32b	(%rax), %rcx
# CHECK-NEXT:  1      3     1.00                        crc32q	%rax, %rcx
# CHECK-NEXT:  2      8     1.00    *                   crc32q	(%rax), %rcx
# CHECK-NEXT:  8      16    3.00                        pcmpestri	$1, %xmm0, %xmm2
# CHECK-NEXT:  8      31    3.00    *                   pcmpestri	$1, (%rax), %xmm2
# CHECK-NEXT:  9      16    3.00                        pcmpestrm	$1, %xmm0, %xmm2
# CHECK-NEXT:  9      17    3.00    *                   pcmpestrm	$1, (%rax), %xmm2
# CHECK-NEXT:  3      11    3.00                        pcmpistri	$1, %xmm0, %xmm2
# CHECK-NEXT:  4      31    3.00    *                   pcmpistri	$1, (%rax), %xmm2
# CHECK-NEXT:  3      11    3.00                        pcmpistrm	$1, %xmm0, %xmm2
# CHECK-NEXT:  4      16    3.00    *                   pcmpistrm	$1, (%rax), %xmm2
# CHECK-NEXT:  1      3     1.00                        pcmpgtq	%xmm0, %xmm2
# CHECK-NEXT:  2      10    1.00    *                   pcmpgtq	(%rax), %xmm2

# CHECK:      Resources:
# CHECK-NEXT: [0]   - SPRPort00
# CHECK-NEXT: [1]   - SPRPort01
# CHECK-NEXT: [2]   - SPRPort02
# CHECK-NEXT: [3]   - SPRPort03
# CHECK-NEXT: [4]   - SPRPort04
# CHECK-NEXT: [5]   - SPRPort05
# CHECK-NEXT: [6]   - SPRPort06
# CHECK-NEXT: [7]   - SPRPort07
# CHECK-NEXT: [8]   - SPRPort08
# CHECK-NEXT: [9]   - SPRPort09
# CHECK-NEXT: [10]  - SPRPort10
# CHECK-NEXT: [11]  - SPRPort11
# CHECK-NEXT: [12]  - SPRPortInvalid

# CHECK:      Resource pressure per iteration:
# CHECK-NEXT: [0]    [1]    [2]    [3]    [4]    [5]    [6]    [7]    [8]    [9]    [10]   [11]   [12]
# CHECK-NEXT: 28.67  16.67  3.33   3.33    -     8.67   2.00    -      -      -     3.33    -      -

# CHECK:      Resource pressure by instruction:
# CHECK-NEXT: [0]    [1]    [2]    [3]    [4]    [5]    [6]    [7]    [8]    [9]    [10]   [11]   [12]   Instructions:
# CHECK-NEXT:  -     1.00    -      -      -      -      -      -      -      -      -      -      -     crc32b	%al, %ecx
# CHECK-NEXT:  -     1.00   0.33   0.33    -      -      -      -      -      -     0.33    -      -     crc32b	(%rax), %ecx
# CHECK-NEXT:  -     1.00    -      -      -      -      -      -      -      -      -      -      -     crc32l	%eax, %ecx
# CHECK-NEXT:  -     1.00   0.33   0.33    -      -      -      -      -      -     0.33    -      -     crc32l	(%rax), %ecx
# CHECK-NEXT:  -     1.00    -      -      -      -      -      -      -      -      -      -      -     crc32w	%ax, %ecx
# CHECK-NEXT:  -     1.00   0.33   0.33    -      -      -      -      -      -     0.33    -      -     crc32w	(%rax), %ecx
# CHECK-NEXT:  -     1.00    -      -      -      -      -      -      -      -      -      -      -     crc32b	%al, %rcx
# CHECK-NEXT:  -     1.00   0.33   0.33    -      -      -      -      -      -     0.33    -      -     crc32b	(%rax), %rcx
# CHECK-NEXT:  -     1.00    -      -      -      -      -      -      -      -      -      -      -     crc32q	%rax, %rcx
# CHECK-NEXT:  -     1.00   0.33   0.33    -      -      -      -      -      -     0.33    -      -     crc32q	(%rax), %rcx
# CHECK-NEXT: 4.17   1.67    -      -      -     1.67   0.50    -      -      -      -      -      -     pcmpestri	$1, %xmm0, %xmm2
# CHECK-NEXT: 3.83   1.33   0.33   0.33    -     1.33   0.50    -      -      -     0.33    -      -     pcmpestri	$1, (%rax), %xmm2
# CHECK-NEXT: 4.50   2.00    -      -      -     2.00   0.50    -      -      -      -      -      -     pcmpestrm	$1, %xmm0, %xmm2
# CHECK-NEXT: 4.17   1.67   0.33   0.33    -     1.67   0.50    -      -      -     0.33    -      -     pcmpestrm	$1, (%rax), %xmm2
# CHECK-NEXT: 3.00    -      -      -      -      -      -      -      -      -      -      -      -     pcmpistri	$1, %xmm0, %xmm2
# CHECK-NEXT: 3.00    -     0.33   0.33    -      -      -      -      -      -     0.33    -      -     pcmpistri	$1, (%rax), %xmm2
# CHECK-NEXT: 3.00    -      -      -      -      -      -      -      -      -      -      -      -     pcmpistrm	$1, %xmm0, %xmm2
# CHECK-NEXT: 3.00    -     0.33   0.33    -      -      -      -      -      -     0.33    -      -     pcmpistrm	$1, (%rax), %xmm2
# CHECK-NEXT:  -      -      -      -      -     1.00    -      -      -      -      -      -      -     pcmpgtq	%xmm0, %xmm2
# CHECK-NEXT:  -      -     0.33   0.33    -     1.00    -      -      -      -     0.33    -      -     pcmpgtq	(%rax), %xmm2
