"""
Test utility functions for the frame object.
"""

import os
import unittest2
import lldb
from lldbtest import *

class FrameUtilsTestCase(TestBase):

    mydir = TestBase.compute_mydir(__file__)

    def setUp(self):
        # Call super's setUp().
        TestBase.setUp(self)
        # Find the line number to break inside main().
        self.line = line_number('main.c',
                                "// Find the line number here.")

    @python_api_test
    def test_frame_utils(self):
        """Test utility functions for the frame object."""
        self.buildDefault()
        self.frame_utils()

    def frame_utils(self):
        exe = os.path.join(os.getcwd(), "a.out")

        target = self.dbg.CreateTarget(exe)
        self.assertTrue(target, VALID_TARGET)

        breakpoint = target.BreakpointCreateByLocation("main.c", self.line)
        self.assertTrue(breakpoint, VALID_BREAKPOINT)

        # Now launch the process, and do not stop at entry point.
        process = target.LaunchSimple (None, None, self.get_process_working_directory())

        if not process:
            self.fail("SBTarget.LaunchProcess() failed")
        self.assertTrue(process.GetState() == lldb.eStateStopped,
                        PROCESS_STOPPED)

        import lldbutil
        thread = lldbutil.get_stopped_thread(process, lldb.eStopReasonBreakpoint)
        self.assertTrue (thread)
        frame0 = thread.GetFrameAtIndex(0)
        self.assertTrue (frame0)
        frame1 = thread.GetFrameAtIndex(1)
        self.assertTrue (frame1)
        parent = lldbutil.get_parent_frame(frame0)
        self.assertTrue(parent and parent.GetFrameID() == frame1.GetFrameID())
        frame0_args = lldbutil.get_args_as_string(frame0)
        parent_args = lldbutil.get_args_as_string(parent)
        self.assertTrue(frame0_args and parent_args and "(int)val=1" in frame0_args)
        if self.TraceOn():
            lldbutil.print_stacktrace(thread)
            print "Current frame: %s" % frame0_args
            print "Parent frame: %s" % parent_args


if __name__ == '__main__':
    import atexit
    lldb.SBDebugger.Initialize()
    atexit.register(lambda: lldb.SBDebugger.Terminate())
    unittest2.main()
