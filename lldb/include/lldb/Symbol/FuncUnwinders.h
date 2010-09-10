#ifndef liblldb_FuncUnwinders_h
#define liblldb_FuncUnwinders_h

#include "lldb/lldb-private.h"
#include "lldb/lldb-forward.h"
#include "lldb/lldb-forward-rtti.h"
#include "lldb/Core/AddressRange.h"
#include "lldb/Core/ArchSpec.h"
#include <memory>

namespace lldb_private {

class UnwindTable;

class FuncUnwinders 
{
public:
    // FuncUnwinders objects are used to track UnwindPlans for a function
    // (named or not - really just an address range)

    // We'll record three different UnwindPlans for each address range:
    //   1. Unwinding from a call site (a valid exception throw location)
    //      This is often sourced from the eh_frame exception handling info
    //   2. Unwinding from a non-call site (any location in the function)
    //      This is often done by analyzing the function prologue assembly
    //      langauge instructions
    //   3. A fast unwind method for this function which only retrieves a 
    //      limited set of registers necessary to walk the stack
    //   4. An architectural default unwind plan when none of the above are
    //      available for some reason.

    // Additionally, FuncUnwinds object can be asked where the prologue 
    // instructions are finished for migrating breakpoints past the 
    // stack frame setup instructions when we don't have line table information.

    FuncUnwinders (lldb_private::UnwindTable& unwind_table, lldb_private::UnwindAssemblyProfiler *assembly_profiler, AddressRange range);

    ~FuncUnwinders ();

    UnwindPlan*
    GetUnwindPlanAtCallSite ();

    UnwindPlan*
    GetUnwindPlanAtNonCallSite (lldb_private::Thread& thread);

    UnwindPlan*
    GetUnwindPlanFastUnwind (lldb_private::Thread& Thread);

    UnwindPlan*
    GetUnwindPlanArchitectureDefault (lldb_private::Thread& thread);

    Address&
    GetFirstNonPrologueInsn (Target& target);

    const Address&
    GetFunctionStartAddress () const;

    bool
    ContainsAddress (const Address& addr) const
    { 
        return m_range.ContainsFileAddress (addr);
    }

protected:

    UnwindTable& m_unwind_table;
    UnwindAssemblyProfiler *m_assembly_profiler;
    AddressRange m_range;

    UnwindPlan* m_unwind_at_call_site;
    UnwindPlan* m_unwind_at_non_call_site;
    UnwindPlan* m_fast_unwind;
    UnwindPlan* m_arch_default_unwind;

    Address m_first_non_prologue_insn;

}; // class FuncUnwinders

inline bool 
operator<(const FuncUnwinders& a, const FuncUnwinders& b)
{
    if (a.GetFunctionStartAddress().GetOffset() < b.GetFunctionStartAddress().GetOffset())
        return true;
    else
        return false;
}

} // namespace lldb_private


#endif //liblldb_FuncUnwinders_h
