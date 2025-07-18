//===- DependencyScanningService.cpp - clang-scan-deps service ------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "clang/Tooling/DependencyScanning/DependencyScanningService.h"

using namespace clang;
using namespace tooling;
using namespace dependencies;

DependencyScanningService::DependencyScanningService(
    ScanningMode Mode, ScanningOutputFormat Format,
    ScanningOptimizations OptimizeArgs, bool EagerLoadModules, bool TraceVFS,
    std::time_t BuildSessionTimestamp, bool CacheNegativeStats)
    : Mode(Mode), Format(Format), OptimizeArgs(OptimizeArgs),
      EagerLoadModules(EagerLoadModules), TraceVFS(TraceVFS),
      CacheNegativeStats(CacheNegativeStats),
      BuildSessionTimestamp(BuildSessionTimestamp) {}
