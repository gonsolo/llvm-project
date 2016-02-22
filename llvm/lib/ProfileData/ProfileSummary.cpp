//=-- Profilesummary.cpp - Profile summary computation ----------------------=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains support for computing profile summary data.
//
//===----------------------------------------------------------------------===//

#include "llvm/IR/Attributes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/ProfileData/InstrProf.h"
#include "llvm/ProfileData/ProfileCommon.h"
#include "llvm/ProfileData/SampleProf.h"

using namespace llvm;

// A set of cutoff values. Each value, when divided by ProfileSummary::Scale
// (which is 1000000) is a desired percentile of total counts.
const std::vector<uint32_t> ProfileSummary::DefaultCutoffs(
    {10000,  /*  1% */
     100000, /* 10% */
     200000, 300000, 400000, 500000, 600000, 500000, 600000, 700000, 800000,
     900000, 950000, 990000, 999000, 999900, 999990, 999999});

void InstrProfSummary::addRecord(const InstrProfRecord &R) {
  addEntryCount(R.Counts[0]);
  for (size_t I = 1, E = R.Counts.size(); I < E; ++I)
    addInternalCount(R.Counts[I]);
}

// To compute the detailed summary, we consider each line containing samples as
// equivalent to a block with a count in the instrumented profile.
void SampleProfileSummary::addRecord(const sampleprof::FunctionSamples &FS) {
  NumFunctions++;
  if (FS.getHeadSamples() > MaxHeadSamples)
    MaxHeadSamples = FS.getHeadSamples();
  for (const auto &I : FS.getBodySamples())
    addCount(I.second.getSamples());
}

// The argument to this method is a vector of cutoff percentages and the return
// value is a vector of (Cutoff, MinCount, NumCounts) triplets.
void ProfileSummary::computeDetailedSummary() {
  if (DetailedSummaryCutoffs.empty())
    return;
  auto Iter = CountFrequencies.begin();
  auto End = CountFrequencies.end();
  std::sort(DetailedSummaryCutoffs.begin(), DetailedSummaryCutoffs.end());

  uint32_t CountsSeen = 0;
  uint64_t CurrSum = 0, Count = 0;

  for (uint32_t Cutoff : DetailedSummaryCutoffs) {
    assert(Cutoff <= 999999);
    APInt Temp(128, TotalCount);
    APInt N(128, Cutoff);
    APInt D(128, ProfileSummary::Scale);
    Temp *= N;
    Temp = Temp.sdiv(D);
    uint64_t DesiredCount = Temp.getZExtValue();
    assert(DesiredCount <= TotalCount);
    while (CurrSum < DesiredCount && Iter != End) {
      Count = Iter->first;
      uint32_t Freq = Iter->second;
      CurrSum += (Count * Freq);
      CountsSeen += Freq;
      Iter++;
    }
    assert(CurrSum >= DesiredCount);
    ProfileSummaryEntry PSE = {Cutoff, Count, CountsSeen};
    DetailedSummary.push_back(PSE);
  }
}

// Returns true if the function is a hot function.
bool ProfileSummary::isFunctionHot(const Function *F) {
  // FIXME: update when summary data is stored in module's metadata.
  return false;
}

// Returns true if the function is a cold function.
bool ProfileSummary::isFunctionUnlikely(const Function *F) {
  if (F->hasFnAttribute(Attribute::Cold)) {
    return true;
  }
  if (!F->getEntryCount()) {
    return false;
  }
  // FIXME: update when summary data is stored in module's metadata.
  return (*F->getEntryCount()) == 0;
}

InstrProfSummary::InstrProfSummary(const IndexedInstrProf::Summary &S)
    : ProfileSummary(), MaxInternalBlockCount(S.get(
                            IndexedInstrProf::Summary::MaxInternalBlockCount)),
      MaxFunctionCount(S.get(IndexedInstrProf::Summary::MaxFunctionCount)),
      NumFunctions(S.get(IndexedInstrProf::Summary::TotalNumFunctions)) {

  TotalCount = S.get(IndexedInstrProf::Summary::TotalBlockCount);
  MaxCount = S.get(IndexedInstrProf::Summary::MaxBlockCount);
  NumCounts = S.get(IndexedInstrProf::Summary::TotalNumBlocks);

  for (unsigned I = 0; I < S.NumCutoffEntries; I++) {
    const IndexedInstrProf::Summary::Entry &Ent = S.getEntry(I);
    DetailedSummary.emplace_back((uint32_t)Ent.Cutoff, Ent.MinBlockCount,
                                 Ent.NumBlocks);
  }
}
void InstrProfSummary::addEntryCount(uint64_t Count) {
  addCount(Count);
  NumFunctions++;
  if (Count > MaxFunctionCount)
    MaxFunctionCount = Count;
}

void InstrProfSummary::addInternalCount(uint64_t Count) {
  addCount(Count);
  if (Count > MaxInternalBlockCount)
    MaxInternalBlockCount = Count;
}
