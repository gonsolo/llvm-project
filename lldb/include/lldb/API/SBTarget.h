//===-- SBTarget.h ----------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLDB_SBTarget_h_
#define LLDB_SBTarget_h_

#include "lldb/API/SBDefines.h"
#include "lldb/API/SBBroadcaster.h"
#include "lldb/API/SBFileSpec.h"

namespace lldb {

class SBBreakpoint;

class SBTarget
{
public:
    //------------------------------------------------------------------
    // Broadcaster bits.
    //------------------------------------------------------------------
    enum
    {
        eBroadcastBitBreakpointChanged  = (1 << 0),
        eBroadcastBitModulesLoaded      = (1 << 1),
        eBroadcastBitModulesUnloaded    = (1 << 2)
    };

    //------------------------------------------------------------------
    // Constructors and Destructors
    //------------------------------------------------------------------
    SBTarget (const lldb::SBTarget& rhs);

    SBTarget ();  // Required for SWIG.

    //------------------------------------------------------------------
    // Destructor
    //------------------------------------------------------------------
    ~SBTarget();

    const lldb::SBTarget&
    Assign (const lldb::SBTarget& rhs);

    bool
    IsValid() const;

    lldb::SBProcess
    CreateProcess (); // DEPRECATED

    lldb::SBProcess
    GetProcess ();

    // DEPRECATED in favor of the function below that contains an SBError as the
    // last parameter.
//     lldb::SBProcess
//     LaunchProcess (char const **argv,
//                    char const **envp,
//                    const char *tty,
//                    uint32_t launch_flags,   // See lldb::LaunchFlags
//                    bool stop_at_entry);

    lldb::SBProcess
    LaunchProcess (char const **argv,
                   char const **envp,
                   const char *tty,
                   uint32_t launch_flags,   // See lldb::LaunchFlags
                   bool stop_at_entry,
                   SBError& error);

    lldb::SBProcess
    AttachToProcess (lldb::pid_t pid, // The process ID to attach to
                     SBError& error); // An error explaining what went wrong if attach fails

    lldb::SBProcess
    AttachToProcess (const char *name,  // basename of process to attach to
                     bool wait_for,     // if true wait for a new instance of "name" to be launched
                     SBError& error);   // An error explaining what went wrong if attach fails

    lldb::SBFileSpec
    GetExecutable ();

    uint32_t
    GetNumModules () const;

    lldb::SBModule
    GetModuleAtIndex (uint32_t idx);

    lldb::SBDebugger
    GetDebugger() const;

    lldb::SBModule
    FindModule (const lldb::SBFileSpec &file_spec);

    void
    Clear ();

    bool
    DeleteTargetFromList (lldb_private::TargetList *list);

    lldb::SBBreakpoint
    BreakpointCreateByLocation (const char *file, uint32_t line);

    lldb::SBBreakpoint
    BreakpointCreateByLocation (const lldb::SBFileSpec &file_spec, uint32_t line);

    lldb::SBBreakpoint
    BreakpointCreateByName (const char *symbol_name, const char *module_name = NULL);

    lldb::SBBreakpoint
    BreakpointCreateByRegex (const char *symbol_name_regex, const char *module_name = NULL);

    lldb::SBBreakpoint
    BreakpointCreateByAddress (addr_t address);

    uint32_t
    GetNumBreakpoints () const;

    lldb::SBBreakpoint
    GetBreakpointAtIndex (uint32_t idx) const;

    bool
    BreakpointDelete (break_id_t break_id);

    lldb::SBBreakpoint
    FindBreakpointByID (break_id_t break_id);

    bool
    EnableAllBreakpoints ();

    bool
    DisableAllBreakpoints ();

    bool
    DeleteAllBreakpoints ();

    lldb::SBBroadcaster
    GetBroadcaster () const;

    //void
    //Disassemble ();

    void
    Disassemble (lldb::addr_t start_address, 
                 lldb::addr_t end_address = LLDB_INVALID_ADDRESS,
                 const char *module_name = NULL);

    void
    Disassemble (const char *function_name, const char *module_name = NULL);

#ifndef SWIG
    bool
    operator == (const lldb::SBTarget &rhs) const;

    bool
    operator != (const lldb::SBTarget &rhs) const;

#endif

    bool
    GetDescription (lldb::SBStream &description);

protected:
    friend class SBAddress;
    friend class SBDebugger;
    friend class SBFunction;
    friend class SBProcess;
    friend class SBSymbol;

    //------------------------------------------------------------------
    // Constructors are private, use static Target::Create function to
    // create an instance of this class.
    //------------------------------------------------------------------

    SBTarget (const lldb::TargetSP& target_sp);

    void
    reset (const lldb::TargetSP& target_sp);

    lldb_private::Target *
    operator ->() const;

    lldb_private::Target *
    get() const;

private:
    //------------------------------------------------------------------
    // For Target only
    //------------------------------------------------------------------

    lldb::TargetSP m_opaque_sp;
};

} // namespace lldb

#endif  // LLDB_SBTarget_h_
