//===-- include/flang/Evaluate/characteristics.h ----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// Defines data structures to represent "characteristics" of Fortran
// procedures and other entities as they are specified in section 15.3
// of Fortran 2018.

#ifndef FORTRAN_EVALUATE_CHARACTERISTICS_H_
#define FORTRAN_EVALUATE_CHARACTERISTICS_H_

#include "common.h"
#include "expression.h"
#include "shape.h"
#include "tools.h"
#include "type.h"
#include "flang/Common/enum-set.h"
#include "flang/Common/idioms.h"
#include "flang/Common/indirection.h"
#include "flang/Parser/char-block.h"
#include "flang/Semantics/symbol.h"
#include "flang/Support/Fortran-features.h"
#include "flang/Support/Fortran.h"
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace llvm {
class raw_ostream;
}

namespace Fortran::evaluate::characteristics {
struct Procedure;
}
extern template class Fortran::common::Indirection<
    Fortran::evaluate::characteristics::Procedure, true>;

namespace Fortran::evaluate::characteristics {

using common::CopyableIndirection;

// Are these procedures distinguishable for a generic name or FINAL?
std::optional<bool> Distinguishable(const common::LanguageFeatureControl &,
    const Procedure &, const Procedure &);
// Are these procedures distinguishable for a generic operator or assignment?
std::optional<bool> DistinguishableOpOrAssign(
    const common::LanguageFeatureControl &, const Procedure &,
    const Procedure &);

// Shapes of function results and dummy arguments have to have
// the same rank, the same deferred dimensions, and the same
// values for explicit dimensions when constant.
bool ShapesAreCompatible(const std::optional<Shape> &,
    const std::optional<Shape> &, bool *possibleWarning = nullptr);

class TypeAndShape {
public:
  ENUM_CLASS(Attr, AssumedRank, AssumedShape, AssumedSize, DeferredShape)
  using Attrs = common::EnumSet<Attr, Attr_enumSize>;

  explicit TypeAndShape(DynamicType t) : type_{t}, shape_{Shape{}} {
    AcquireLEN();
  }
  TypeAndShape(DynamicType t, int rank) : type_{t}, shape_{Shape(rank)} {
    AcquireLEN();
  }
  TypeAndShape(DynamicType t, Shape &&s) : type_{t}, shape_{std::move(s)} {
    AcquireLEN();
  }
  TypeAndShape(DynamicType t, std::optional<Shape> &&s) : type_{t} {
    shape_ = std::move(s);
    AcquireLEN();
  }
  DEFAULT_CONSTRUCTORS_AND_ASSIGNMENTS(TypeAndShape)

  bool operator==(const TypeAndShape &) const;
  bool operator!=(const TypeAndShape &that) const { return !(*this == that); }

  static std::optional<TypeAndShape> Characterize(
      const semantics::Symbol &, FoldingContext &, bool invariantOnly = true);
  static std::optional<TypeAndShape> Characterize(
      const semantics::DeclTypeSpec &, FoldingContext &,
      bool invariantOnly = true);
  static std::optional<TypeAndShape> Characterize(
      const ActualArgument &, FoldingContext &, bool invariantOnly = true);

  // General case for Expr<T>, &c.
  template <typename A>
  static std::optional<TypeAndShape> Characterize(
      const A &x, FoldingContext &context, bool invariantOnly = true) {
    const auto *symbol{UnwrapWholeSymbolOrComponentDataRef(x)};
    if (symbol && !symbol->owner().IsDerivedType()) { // Whole variable
      if (auto result{Characterize(*symbol, context, invariantOnly)}) {
        return result;
      }
    }
    if (auto type{x.GetType()}) {
      TypeAndShape result{*type, GetShape(context, x, invariantOnly)};
      result.corank_ = GetCorank(x);
      if (type->category() == TypeCategory::Character) {
        if (const auto *chExpr{UnwrapExpr<Expr<SomeCharacter>>(x)}) {
          if (auto length{chExpr->LEN()}) {
            result.set_LEN(std::move(*length));
          }
        }
      }
      if (symbol) { // component
        result.AcquireAttrs(*symbol);
      }
      return std::move(result.Rewrite(context));
    }
    return std::nullopt;
  }

  // Specialization for character designators
  template <int KIND>
  static std::optional<TypeAndShape> Characterize(
      const Designator<Type<TypeCategory::Character, KIND>> &x,
      FoldingContext &context, bool invariantOnly = true) {
    const auto *symbol{UnwrapWholeSymbolOrComponentDataRef(x)};
    if (symbol && !symbol->owner().IsDerivedType()) { // Whole variable
      if (auto result{Characterize(*symbol, context, invariantOnly)}) {
        return result;
      }
    }
    if (auto type{x.GetType()}) {
      TypeAndShape result{*type, GetShape(context, x, invariantOnly)};
      if (type->category() == TypeCategory::Character) {
        if (auto length{x.LEN()}) {
          result.set_LEN(std::move(*length));
        }
      }
      if (symbol) { // component
        result.AcquireAttrs(*symbol);
      }
      return std::move(result.Rewrite(context));
    }
    return std::nullopt;
  }

  template <typename A>
  static std::optional<TypeAndShape> Characterize(const std::optional<A> &x,
      FoldingContext &context, bool invariantOnly = true) {
    if (x) {
      return Characterize(*x, context, invariantOnly);
    } else {
      return std::nullopt;
    }
  }
  template <typename A>
  static std::optional<TypeAndShape> Characterize(
      A *ptr, FoldingContext &context, bool invariantOnly = true) {
    if (ptr) {
      return Characterize(std::as_const(*ptr), context, invariantOnly);
    } else {
      return std::nullopt;
    }
  }

  DynamicType type() const { return type_; }
  TypeAndShape &set_type(DynamicType t) {
    type_ = t;
    return *this;
  }
  const std::optional<Expr<SubscriptInteger>> &LEN() const { return LEN_; }
  TypeAndShape &set_LEN(Expr<SubscriptInteger> &&len) {
    LEN_ = std::move(len);
    return *this;
  }
  const std::optional<Shape> &shape() const { return shape_; }
  const Attrs &attrs() const { return attrs_; }
  Attrs &attrs() { return attrs_; }
  bool isPossibleSequenceAssociation() const {
    return isPossibleSequenceAssociation_;
  }
  TypeAndShape &set_isPossibleSequenceAssociation(bool yes) {
    isPossibleSequenceAssociation_ = yes;
    return *this;
  }
  int corank() const { return corank_; }
  void set_corank(int n) { corank_ = n; }

  // Return -1 for assumed-rank as a safety.
  int Rank() const { return shape_ ? GetRank(*shape_) : -1; }

  // Can sequence association apply to this argument?
  bool CanBeSequenceAssociated() const {
    constexpr Attrs notAssumedOrExplicitShape{~Attrs{Attr::AssumedSize}};
    return Rank() > 0 && (attrs() & notAssumedOrExplicitShape).none();
  }

  bool IsCompatibleWith(parser::ContextualMessages &, const TypeAndShape &that,
      const char *thisIs = "pointer", const char *thatIs = "target",
      bool omitShapeConformanceCheck = false,
      enum CheckConformanceFlags::Flags = CheckConformanceFlags::None) const;
  std::optional<Expr<SubscriptInteger>> MeasureElementSizeInBytes(
      FoldingContext &, bool align) const;
  std::optional<Expr<SubscriptInteger>> MeasureSizeInBytes(
      FoldingContext &) const;

  // called by Fold() to rewrite in place
  TypeAndShape &Rewrite(FoldingContext &);

  std::string AsFortran() const;
  llvm::raw_ostream &Dump(llvm::raw_ostream &) const;

private:
  static std::optional<TypeAndShape> Characterize(
      const semantics::AssocEntityDetails &, FoldingContext &,
      bool invariantOnly = true);
  void AcquireAttrs(const semantics::Symbol &);
  void AcquireLEN();
  void AcquireLEN(const semantics::Symbol &);

  DynamicType type_;
  std::optional<Expr<SubscriptInteger>> LEN_;
  std::optional<Shape> shape_;
  Attrs attrs_;
  bool isPossibleSequenceAssociation_{false};
  int corank_{0};
};

// 15.3.2.2
struct DummyDataObject {
  ENUM_CLASS(Attr, Optional, Allocatable, Asynchronous, Contiguous, Value,
      Volatile, Pointer, Target, DeducedFromActual, OnlyIntrinsicInquiry)
  using Attrs = common::EnumSet<Attr, Attr_enumSize>;
  static bool IdenticalSignificantAttrs(const Attrs &x, const Attrs &y) {
    return (x - Attr::DeducedFromActual) == (y - Attr::DeducedFromActual);
  }
  DEFAULT_CONSTRUCTORS_AND_ASSIGNMENTS(DummyDataObject)
  explicit DummyDataObject(const TypeAndShape &t) : type{t} {}
  explicit DummyDataObject(TypeAndShape &&t) : type{std::move(t)} {}
  explicit DummyDataObject(DynamicType t) : type{t} {}
  bool operator==(const DummyDataObject &) const;
  bool operator!=(const DummyDataObject &that) const {
    return !(*this == that);
  }
  bool IsCompatibleWith(const DummyDataObject &, std::string *whyNot = nullptr,
      std::optional<std::string> *warning = nullptr) const;
  static std::optional<DummyDataObject> Characterize(
      const semantics::Symbol &, FoldingContext &);
  bool CanBePassedViaImplicitInterface(std::string *whyNot = nullptr) const;
  bool IsPassedByDescriptor(bool isBindC) const;
  llvm::raw_ostream &Dump(llvm::raw_ostream &) const;

  TypeAndShape type;
  std::vector<Expr<SubscriptInteger>> coshape;
  common::Intent intent{common::Intent::Default};
  Attrs attrs;
  common::IgnoreTKRSet ignoreTKR;
  std::optional<common::CUDADataAttr> cudaDataAttr;
};

// 15.3.2.3
struct DummyProcedure {
  ENUM_CLASS(Attr, Pointer, Optional)
  using Attrs = common::EnumSet<Attr, Attr_enumSize>;
  DECLARE_CONSTRUCTORS_AND_ASSIGNMENTS(DummyProcedure)
  explicit DummyProcedure(Procedure &&);
  bool operator==(const DummyProcedure &) const;
  bool operator!=(const DummyProcedure &that) const { return !(*this == that); }
  bool IsCompatibleWith(
      const DummyProcedure &, std::string *whyNot = nullptr) const;
  bool CanBePassedViaImplicitInterface(std::string *whyNot = nullptr) const;
  llvm::raw_ostream &Dump(llvm::raw_ostream &) const;

  CopyableIndirection<Procedure> procedure;
  common::Intent intent{common::Intent::Default};
  Attrs attrs;
};

// 15.3.2.4
struct AlternateReturn {
  bool operator==(const AlternateReturn &) const { return true; }
  bool operator!=(const AlternateReturn &) const { return false; }
  llvm::raw_ostream &Dump(llvm::raw_ostream &) const;
};

// 15.3.2.1
struct DummyArgument {
  DECLARE_CONSTRUCTORS_AND_ASSIGNMENTS(DummyArgument)
  DummyArgument(std::string &&name, DummyDataObject &&x)
      : name{std::move(name)}, u{std::move(x)} {}
  DummyArgument(std::string &&name, DummyProcedure &&x)
      : name{std::move(name)}, u{std::move(x)} {}
  explicit DummyArgument(AlternateReturn &&x) : u{std::move(x)} {}
  ~DummyArgument();
  bool operator==(const DummyArgument &) const;
  bool operator!=(const DummyArgument &that) const { return !(*this == that); }
  static std::optional<DummyArgument> FromActual(std::string &&,
      const Expr<SomeType> &, FoldingContext &, bool forImplicitInterface);
  static std::optional<DummyArgument> FromActual(std::string &&,
      const ActualArgument &, FoldingContext &, bool forImplicitInterface);
  bool IsOptional() const;
  void SetOptional(bool = true);
  common::Intent GetIntent() const;
  void SetIntent(common::Intent);
  bool CanBePassedViaImplicitInterface(std::string *whyNot = nullptr) const;
  bool IsTypelessIntrinsicDummy() const;
  bool IsCompatibleWith(const DummyArgument &, std::string *whyNot = nullptr,
      std::optional<std::string> *warning = nullptr) const;
  llvm::raw_ostream &Dump(llvm::raw_ostream &) const;

  // name and pass are not characteristics and so do not participate in
  // compatibility checks, but they are needed to determine whether
  // procedures are distinguishable
  std::string name;
  bool pass{false}; // is this the PASS argument of its procedure
  std::variant<DummyDataObject, DummyProcedure, AlternateReturn> u;
};

using DummyArguments = std::vector<DummyArgument>;

// 15.3.3
struct FunctionResult {
  ENUM_CLASS(Attr, Allocatable, Pointer, Contiguous)
  using Attrs = common::EnumSet<Attr, Attr_enumSize>;
  DECLARE_CONSTRUCTORS_AND_ASSIGNMENTS(FunctionResult)
  explicit FunctionResult(DynamicType);
  explicit FunctionResult(TypeAndShape &&);
  explicit FunctionResult(Procedure &&);
  ~FunctionResult();
  bool operator==(const FunctionResult &) const;
  bool operator!=(const FunctionResult &that) const { return !(*this == that); }
  static std::optional<FunctionResult> Characterize(
      const Symbol &, FoldingContext &);

  bool IsAssumedLengthCharacter() const;

  const Procedure *IsProcedurePointer() const {
    if (const auto *pp{std::get_if<CopyableIndirection<Procedure>>(&u)}) {
      return &pp->value();
    } else {
      return nullptr;
    }
  }
  const TypeAndShape *GetTypeAndShape() const {
    return std::get_if<TypeAndShape>(&u);
  }
  void SetType(DynamicType t) { std::get<TypeAndShape>(u).set_type(t); }
  bool CanBeReturnedViaImplicitInterface(std::string *whyNot = nullptr) const;
  bool IsCompatibleWith(
      const FunctionResult &, std::string *whyNot = nullptr) const;

  llvm::raw_ostream &Dump(llvm::raw_ostream &) const;

  Attrs attrs;
  std::variant<TypeAndShape, CopyableIndirection<Procedure>> u;
  std::optional<common::CUDADataAttr> cudaDataAttr;
};

// 15.3.1
struct Procedure {
  ENUM_CLASS(Attr, Pure, Elemental, BindC, ImplicitInterface, NullPointer,
      NullAllocatable, Subroutine)
  using Attrs = common::EnumSet<Attr, Attr_enumSize>;
  Procedure(){};
  Procedure(FunctionResult &&, DummyArguments &&, Attrs);
  Procedure(DummyArguments &&, Attrs); // for subroutines and NULL()
  DECLARE_CONSTRUCTORS_AND_ASSIGNMENTS(Procedure)
  ~Procedure();
  bool operator==(const Procedure &) const;
  bool operator!=(const Procedure &that) const { return !(*this == that); }

  // Characterizes a procedure.  If a Symbol, it may be an
  // "unrestricted specific intrinsic function".
  // Error messages are produced when a procedure cannot be characterized.
  static std::optional<Procedure> Characterize(
      const semantics::Symbol &, FoldingContext &);
  static std::optional<Procedure> Characterize(
      const ProcedureDesignator &, FoldingContext &, bool emitError);
  static std::optional<Procedure> Characterize(
      const ProcedureRef &, FoldingContext &);
  static std::optional<Procedure> Characterize(
      const Expr<SomeType> &, FoldingContext &);
  // Characterizes the procedure being referenced, deducing dummy argument
  // types from actual arguments in the case of an implicit interface.
  static std::optional<Procedure> FromActuals(
      const ProcedureDesignator &, const ActualArguments &, FoldingContext &);

  // At most one of these will return true.
  // For "EXTERNAL P" with no type for or calls to P, both will be false.
  bool IsFunction() const { return functionResult.has_value(); }
  bool IsSubroutine() const { return attrs.test(Attr::Subroutine); }

  bool IsPure() const { return attrs.test(Attr::Pure); }
  bool IsElemental() const { return attrs.test(Attr::Elemental); }
  bool IsBindC() const { return attrs.test(Attr::BindC); }
  bool HasExplicitInterface() const {
    return !attrs.test(Attr::ImplicitInterface);
  }
  std::optional<int> FindPassIndex(std::optional<parser::CharBlock>) const;
  bool CanBeCalledViaImplicitInterface(std::string *whyNot = nullptr) const;
  bool CanOverride(const Procedure &, std::optional<int> passIndex) const;
  bool IsCompatibleWith(const Procedure &, bool ignoreImplicitVsExplicit,
      std::string *whyNot = nullptr, const SpecificIntrinsic * = nullptr,
      std::optional<std::string> *warning = nullptr) const;

  llvm::raw_ostream &Dump(llvm::raw_ostream &) const;

  std::optional<FunctionResult> functionResult;
  DummyArguments dummyArguments;
  Attrs attrs;
  std::optional<common::CUDASubprogramAttrs> cudaSubprogramAttrs;
};

} // namespace Fortran::evaluate::characteristics
#endif // FORTRAN_EVALUATE_CHARACTERISTICS_H_
