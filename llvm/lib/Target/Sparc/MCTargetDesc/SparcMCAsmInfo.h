//===- SparcMCAsmInfo.h - Sparc asm properties -----------------*- C++ -*--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the SparcMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SPARC_MCTARGETDESC_SPARCMCASMINFO_H
#define LLVM_LIB_TARGET_SPARC_MCTARGETDESC_SPARCMCASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {

class Triple;

class SparcELFMCAsmInfo : public MCAsmInfoELF {
  void anchor() override;

public:
  explicit SparcELFMCAsmInfo(const Triple &TheTriple);

  const MCExpr*
  getExprForPersonalitySymbol(const MCSymbol *Sym, unsigned Encoding,
                              MCStreamer &Streamer) const override;
  const MCExpr* getExprForFDESymbol(const MCSymbol *Sym,
                                    unsigned Encoding,
                                    MCStreamer &Streamer) const override;

  void printSpecifierExpr(raw_ostream &OS,
                          const MCSpecifierExpr &Expr) const override;
};

namespace Sparc {
uint16_t parseSpecifier(StringRef name);
StringRef getSpecifierName(uint16_t S);
} // namespace Sparc

} // end namespace llvm

#endif // LLVM_LIB_TARGET_SPARC_MCTARGETDESC_SPARCMCASMINFO_H
