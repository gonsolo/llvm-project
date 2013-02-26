//===-- MachThreadList.cpp --------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  Created by Greg Clayton on 6/19/07.
//
//===----------------------------------------------------------------------===//

#include "MachThreadList.h"

#include <inttypes.h>
#include <sys/sysctl.h>

#include "DNBLog.h"
#include "DNBThreadResumeActions.h"
#include "MachProcess.h"

MachThreadList::MachThreadList() :
    m_threads(),
    m_threads_mutex(PTHREAD_MUTEX_RECURSIVE)
{
}

MachThreadList::~MachThreadList()
{
}

nub_state_t
MachThreadList::GetState(nub_thread_t tid)
{
    MachThreadSP thread_sp (GetThreadByID (tid));
    if (thread_sp)
        return thread_sp->GetState();
    return eStateInvalid;
}

const char *
MachThreadList::GetName (nub_thread_t tid)
{
    MachThreadSP thread_sp (GetThreadByID (tid));
    if (thread_sp)
        return thread_sp->GetName();
    return NULL;
}

nub_thread_t
MachThreadList::SetCurrentThread(nub_thread_t tid)
{
    MachThreadSP thread_sp (GetThreadByID (tid));
    if (thread_sp)
    {
        m_current_thread = thread_sp;
        return tid;
    }
    return INVALID_NUB_THREAD;
}


bool
MachThreadList::GetThreadStoppedReason(nub_thread_t tid, struct DNBThreadStopInfo *stop_info) const
{
    MachThreadSP thread_sp (GetThreadByID (tid));
    if (thread_sp)
        return thread_sp->GetStopException().GetStopInfo(stop_info);
    return false;
}

bool
MachThreadList::GetIdentifierInfo (nub_thread_t tid, thread_identifier_info_data_t *ident_info)
{
    thread_t mach_port_number = GetMachPortNumberByThreadID (tid);

    mach_msg_type_number_t count = THREAD_IDENTIFIER_INFO_COUNT;
    return ::thread_info (mach_port_number, THREAD_IDENTIFIER_INFO, (thread_info_t)ident_info, &count) == KERN_SUCCESS;
}

void
MachThreadList::DumpThreadStoppedReason (nub_thread_t tid) const
{
    MachThreadSP thread_sp (GetThreadByID (tid));
    if (thread_sp)
        thread_sp->GetStopException().DumpStopReason();
}

const char *
MachThreadList::GetThreadInfo (nub_thread_t tid) const
{
    MachThreadSP thread_sp (GetThreadByID (tid));
    if (thread_sp)
        return thread_sp->GetBasicInfoAsString();
    return NULL;
}

MachThreadSP
MachThreadList::GetThreadByID (nub_thread_t tid) const
{
    PTHREAD_MUTEX_LOCKER (locker, m_threads_mutex);
    MachThreadSP thread_sp;
    const size_t num_threads = m_threads.size();
    for (size_t idx = 0; idx < num_threads; ++idx)
    {
        if (m_threads[idx]->ThreadID() == tid)
        {
            thread_sp = m_threads[idx];
            break;
        }
    }
    return thread_sp;
}

MachThreadSP
MachThreadList::GetThreadByMachPortNumber (thread_t mach_port_number) const
{
    PTHREAD_MUTEX_LOCKER (locker, m_threads_mutex);
    MachThreadSP thread_sp;
    const size_t num_threads = m_threads.size();
    for (size_t idx = 0; idx < num_threads; ++idx)
    {
        if (m_threads[idx]->MachPortNumber() == mach_port_number)
        {
            thread_sp = m_threads[idx];
            break;
        }
    }
    return thread_sp;
}

nub_thread_t
MachThreadList::GetThreadIDByMachPortNumber (thread_t mach_port_number) const
{
    PTHREAD_MUTEX_LOCKER (locker, m_threads_mutex);
    MachThreadSP thread_sp;
    const size_t num_threads = m_threads.size();
    for (size_t idx = 0; idx < num_threads; ++idx)
    {
        if (m_threads[idx]->MachPortNumber() == mach_port_number)
        {
            return m_threads[idx]->ThreadID();
        }
    }
    return INVALID_NUB_THREAD;
}

thread_t
MachThreadList::GetMachPortNumberByThreadID (nub_thread_t globally_unique_id) const
{
    PTHREAD_MUTEX_LOCKER (locker, m_threads_mutex);
    MachThreadSP thread_sp;
    const size_t num_threads = m_threads.size();
    for (size_t idx = 0; idx < num_threads; ++idx)
    {
        if (m_threads[idx]->ThreadID() == globally_unique_id)
        {
            return m_threads[idx]->MachPortNumber();
        }
    }
    return 0;
}

bool
MachThreadList::GetRegisterValue (nub_thread_t tid, uint32_t reg_set_idx, uint32_t reg_idx, DNBRegisterValue *reg_value ) const
{
    MachThreadSP thread_sp (GetThreadByID (tid));
    if (thread_sp)
        return thread_sp->GetRegisterValue(reg_set_idx, reg_idx, reg_value);

    return false;
}

bool
MachThreadList::SetRegisterValue (nub_thread_t tid, uint32_t reg_set_idx, uint32_t reg_idx, const DNBRegisterValue *reg_value ) const
{
    MachThreadSP thread_sp (GetThreadByID (tid));
    if (thread_sp)
        return thread_sp->SetRegisterValue(reg_set_idx, reg_idx, reg_value);

    return false;
}

nub_size_t
MachThreadList::GetRegisterContext (nub_thread_t tid, void *buf, size_t buf_len)
{
    MachThreadSP thread_sp (GetThreadByID (tid));
    if (thread_sp)
        return thread_sp->GetRegisterContext (buf, buf_len);
    return 0;
}

nub_size_t
MachThreadList::SetRegisterContext (nub_thread_t tid, const void *buf, size_t buf_len)
{
    MachThreadSP thread_sp (GetThreadByID (tid));
    if (thread_sp)
        return thread_sp->SetRegisterContext (buf, buf_len);
    return 0;
}

nub_size_t
MachThreadList::NumThreads () const
{
    PTHREAD_MUTEX_LOCKER (locker, m_threads_mutex);
    return m_threads.size();
}

nub_thread_t
MachThreadList::ThreadIDAtIndex (nub_size_t idx) const
{
    PTHREAD_MUTEX_LOCKER (locker, m_threads_mutex);
    if (idx < m_threads.size())
        return m_threads[idx]->ThreadID();
    return INVALID_NUB_THREAD;
}

nub_thread_t
MachThreadList::CurrentThreadID ( )
{
    MachThreadSP thread_sp;
    CurrentThread(thread_sp);
    if (thread_sp.get())
        return thread_sp->ThreadID();
    return INVALID_NUB_THREAD;
}

bool
MachThreadList::NotifyException(MachException::Data& exc)
{
    MachThreadSP thread_sp (GetThreadByMachPortNumber (exc.thread_port));
    if (thread_sp)
    {
        thread_sp->NotifyException(exc);
        return true;
    }
    return false;
}

void
MachThreadList::Clear()
{
    PTHREAD_MUTEX_LOCKER (locker, m_threads_mutex);
    m_threads.clear();
}

uint32_t
MachThreadList::UpdateThreadList(MachProcess *process, bool update, MachThreadList::collection *new_threads)
{
    // locker will keep a mutex locked until it goes out of scope
    DNBLogThreadedIf (LOG_THREAD, "MachThreadList::UpdateThreadList (pid = %4.4x, update = %u) process stop count = %u", process->ProcessID(), update, process->StopCount());
    PTHREAD_MUTEX_LOCKER (locker, m_threads_mutex);

#if defined (__i386__) || defined (__x86_64__)
    if (process->StopCount() == 0)
    {
        int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, process->ProcessID() };
        struct kinfo_proc processInfo;
        size_t bufsize = sizeof(processInfo);
        bool is_64_bit = false;
        if (sysctl(mib, (unsigned)(sizeof(mib)/sizeof(int)), &processInfo, &bufsize, NULL, 0) == 0 && bufsize > 0)
        {
            if (processInfo.kp_proc.p_flag & P_LP64)
                is_64_bit = true;
        }
        if (is_64_bit)
            DNBArchProtocol::SetArchitecture(CPU_TYPE_X86_64);
        else
            DNBArchProtocol::SetArchitecture(CPU_TYPE_I386);
    }
#endif
    
    if (m_threads.empty() || update)
    {
        thread_array_t thread_list = NULL;
        mach_msg_type_number_t thread_list_count = 0;
        task_t task = process->Task().TaskPort();
        DNBError err(::task_threads (task, &thread_list, &thread_list_count), DNBError::MachKernel);

        if (DNBLogCheckLogBit(LOG_THREAD) || err.Fail())
            err.LogThreaded("::task_threads ( task = 0x%4.4x, thread_list => %p, thread_list_count => %u )", task, thread_list, thread_list_count);

        if (err.Error() == KERN_SUCCESS && thread_list_count > 0)
        {
            MachThreadList::collection currThreads;
            size_t idx;
            // Iterator through the current thread list and see which threads
            // we already have in our list (keep them), which ones we don't
            // (add them), and which ones are not around anymore (remove them).
            for (idx = 0; idx < thread_list_count; ++idx)
            {
                const thread_t mach_port_num = thread_list[idx];
                
                uint64_t unique_thread_id = MachThread::GetGloballyUniqueThreadIDForMachPortID (mach_port_num);
                MachThreadSP thread_sp (GetThreadByID (unique_thread_id));
                if (thread_sp)
                {
                    // Keep the existing thread class
                    currThreads.push_back(thread_sp);
                }
                else
                {
                    // We don't have this thread, lets add it.
                    thread_sp.reset(new MachThread(process, unique_thread_id, mach_port_num));

                    // Add the new thread regardless of its is user ready state...
                    // Make sure the thread is ready to be displayed and shown to users
                    // before we add this thread to our list...
                    if (thread_sp->IsUserReady())
                    {
                        if (new_threads)
                            new_threads->push_back(thread_sp);
                    
                        currThreads.push_back(thread_sp);
                    }
                }
            }

            m_threads.swap(currThreads);
            m_current_thread.reset();

            // Free the vm memory given to us by ::task_threads()
            vm_size_t thread_list_size = (vm_size_t) (thread_list_count * sizeof (thread_t));
            ::vm_deallocate (::mach_task_self(),
                             (vm_address_t)thread_list,
                             thread_list_size);
        }
    }
    return m_threads.size();
}


void
MachThreadList::CurrentThread (MachThreadSP& thread_sp)
{
    // locker will keep a mutex locked until it goes out of scope
    PTHREAD_MUTEX_LOCKER (locker, m_threads_mutex);
    if (m_current_thread.get() == NULL)
    {
        // Figure out which thread is going to be our current thread.
        // This is currently done by finding the first thread in the list
        // that has a valid exception.
        const uint32_t num_threads = m_threads.size();
        for (uint32_t idx = 0; idx < num_threads; ++idx)
        {
            if (m_threads[idx]->GetStopException().IsValid())
            {
                m_current_thread = m_threads[idx];
                break;
            }
        }
    }
    thread_sp = m_current_thread;
}

void
MachThreadList::Dump() const
{
    PTHREAD_MUTEX_LOCKER (locker, m_threads_mutex);
    const uint32_t num_threads = m_threads.size();
    for (uint32_t idx = 0; idx < num_threads; ++idx)
    {
        m_threads[idx]->Dump(idx);
    }
}


void
MachThreadList::ProcessWillResume(MachProcess *process, const DNBThreadResumeActions &thread_actions)
{
    PTHREAD_MUTEX_LOCKER (locker, m_threads_mutex);

    // Update our thread list, because sometimes libdispatch or the kernel
    // will spawn threads while a task is suspended.
    MachThreadList::collection new_threads;
    
    // First figure out if we were planning on running only one thread, and if so force that thread to resume.
    bool run_one_thread;
    nub_thread_t solo_thread = INVALID_NUB_THREAD;
    if (thread_actions.GetSize() > 0 
        && thread_actions.NumActionsWithState(eStateStepping) + thread_actions.NumActionsWithState (eStateRunning) == 1)
    {
        run_one_thread = true;
        const DNBThreadResumeAction *action_ptr = thread_actions.GetFirst();
        size_t num_actions = thread_actions.GetSize();
        for (size_t i = 0; i < num_actions; i++, action_ptr++)
        {
            if (action_ptr->state == eStateStepping || action_ptr->state == eStateRunning)
            {
                solo_thread = action_ptr->tid;
                break;
            }
        }
    }
    else
        run_one_thread = false;

    UpdateThreadList(process, true, &new_threads);

    DNBThreadResumeAction resume_new_threads = { -1U, eStateRunning, 0, INVALID_NUB_ADDRESS };
    // If we are planning to run only one thread, any new threads should be suspended.
    if (run_one_thread)
        resume_new_threads.state = eStateSuspended;

    const uint32_t num_new_threads = new_threads.size();
    const uint32_t num_threads = m_threads.size();
    for (uint32_t idx = 0; idx < num_threads; ++idx)
    {
        MachThread *thread = m_threads[idx].get();
        bool handled = false;
        for (uint32_t new_idx = 0; new_idx < num_new_threads; ++new_idx)
        {
            if (thread == new_threads[new_idx].get())
            {
                thread->ThreadWillResume(&resume_new_threads);
                handled = true;
                break;
            }
        }

        if (!handled)
        {
            const DNBThreadResumeAction *thread_action = thread_actions.GetActionForThread (thread->ThreadID(), true);
            // There must always be a thread action for every thread.
            assert (thread_action);
            bool others_stopped = false;
            if (solo_thread == thread->ThreadID())
                others_stopped = true;
            thread->ThreadWillResume (thread_action, others_stopped);
        }
    }
    
    if (new_threads.size())
    {
        for (uint32_t idx = 0; idx < num_new_threads; ++idx)
        {
            DNBLogThreadedIf (LOG_THREAD, "MachThreadList::ProcessWillResume (pid = %4.4x) stop-id=%u, resuming newly discovered thread: 0x%8.8" PRIx64 ", thread-is-user-ready=%i)",
                              process->ProcessID(), 
                              process->StopCount(), 
                              new_threads[idx]->ThreadID(),
                              new_threads[idx]->IsUserReady());
        }
    }
}

uint32_t
MachThreadList::ProcessDidStop(MachProcess *process)
{
    PTHREAD_MUTEX_LOCKER (locker, m_threads_mutex);
    // Update our thread list
    const uint32_t num_threads = UpdateThreadList(process, true);
    for (uint32_t idx = 0; idx < num_threads; ++idx)
    {
        m_threads[idx]->ThreadDidStop();
    }
    return num_threads;
}

//----------------------------------------------------------------------
// Check each thread in our thread list to see if we should notify our
// client of the current halt in execution.
//
// Breakpoints can have callback functions associated with them than
// can return true to stop, or false to continue executing the inferior.
//
// RETURNS
//    true if we should stop and notify our clients
//    false if we should resume our child process and skip notification
//----------------------------------------------------------------------
bool
MachThreadList::ShouldStop(bool &step_more)
{
    PTHREAD_MUTEX_LOCKER (locker, m_threads_mutex);
    uint32_t should_stop = false;
    const uint32_t num_threads = m_threads.size();
    for (uint32_t idx = 0; !should_stop && idx < num_threads; ++idx)
    {
        should_stop = m_threads[idx]->ShouldStop(step_more);
    }
    return should_stop;
}


void
MachThreadList::NotifyBreakpointChanged (const DNBBreakpoint *bp)
{
    PTHREAD_MUTEX_LOCKER (locker, m_threads_mutex);
    const uint32_t num_threads = m_threads.size();
    for (uint32_t idx = 0; idx < num_threads; ++idx)
    {
        m_threads[idx]->NotifyBreakpointChanged(bp);
    }
}


uint32_t
MachThreadList::EnableHardwareBreakpoint (const DNBBreakpoint* bp) const
{
    if (bp != NULL)
    {
        MachThreadSP thread_sp (GetThreadByID (bp->ThreadID()));
        if (thread_sp)
            return thread_sp->EnableHardwareBreakpoint(bp);
    }
    return INVALID_NUB_HW_INDEX;
}

bool
MachThreadList::DisableHardwareBreakpoint (const DNBBreakpoint* bp) const
{
    if (bp != NULL)
    {
        MachThreadSP thread_sp (GetThreadByID (bp->ThreadID()));
        if (thread_sp)
            return thread_sp->DisableHardwareBreakpoint(bp);
    }
    return false;
}

// DNBWatchpointSet() -> MachProcess::CreateWatchpoint() -> MachProcess::EnableWatchpoint()
// -> MachThreadList::EnableHardwareWatchpoint().
uint32_t
MachThreadList::EnableHardwareWatchpoint (const DNBBreakpoint* wp) const
{
    uint32_t hw_index = INVALID_NUB_HW_INDEX;
    if (wp != NULL)
    {
        PTHREAD_MUTEX_LOCKER (locker, m_threads_mutex);
        const uint32_t num_threads = m_threads.size();
        for (uint32_t idx = 0; idx < num_threads; ++idx)
        {
            if ((hw_index = m_threads[idx]->EnableHardwareWatchpoint(wp)) == INVALID_NUB_HW_INDEX)
            {
                // We know that idx failed for some reason.  Let's rollback the transaction for [0, idx).
                for (uint32_t i = 0; i < idx; ++i)
                    m_threads[i]->RollbackTransForHWP();
                return INVALID_NUB_HW_INDEX;
            }
        }
        // Notify each thread to commit the pending transaction.
        for (uint32_t idx = 0; idx < num_threads; ++idx)
            m_threads[idx]->FinishTransForHWP();

        // Use an arbitrary thread to signal the completion of our transaction.
        if (num_threads)
            m_threads[0]->HardwareWatchpointStateChanged();
    }
    return hw_index;
}

bool
MachThreadList::DisableHardwareWatchpoint (const DNBBreakpoint* wp) const
{
    if (wp != NULL)
    {
        PTHREAD_MUTEX_LOCKER (locker, m_threads_mutex);
        const uint32_t num_threads = m_threads.size();
        for (uint32_t idx = 0; idx < num_threads; ++idx)
        {
            if (!m_threads[idx]->DisableHardwareWatchpoint(wp))
            {
                // We know that idx failed for some reason.  Let's rollback the transaction for [0, idx).
                for (uint32_t i = 0; i < idx; ++i)
                    m_threads[i]->RollbackTransForHWP();
                return false;
            }
        }
        // Notify each thread to commit the pending transaction.
        for (uint32_t idx = 0; idx < num_threads; ++idx)
            m_threads[idx]->FinishTransForHWP();

        // Use an arbitrary thread to signal the completion of our transaction.
        if (num_threads)
            m_threads[0]->HardwareWatchpointStateChanged();
        return true;
    }
    return false;
}

uint32_t
MachThreadList::NumSupportedHardwareWatchpoints () const
{
    PTHREAD_MUTEX_LOCKER (locker, m_threads_mutex);
    const uint32_t num_threads = m_threads.size();
    // Use an arbitrary thread to retrieve the number of supported hardware watchpoints.
    if (num_threads)
        return m_threads[0]->NumSupportedHardwareWatchpoints();
    return 0;
}

uint32_t
MachThreadList::GetThreadIndexForThreadStoppedWithSignal (const int signo) const
{
    PTHREAD_MUTEX_LOCKER (locker, m_threads_mutex);
    uint32_t should_stop = false;
    const uint32_t num_threads = m_threads.size();
    for (uint32_t idx = 0; !should_stop && idx < num_threads; ++idx)
    {
        if (m_threads[idx]->GetStopException().SoftSignal () == signo)
            return idx;
    }
    return UINT32_MAX;
}

