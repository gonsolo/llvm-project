"""
Verify the default cache line size for android targets
"""

import os
import unittest2
import lldb
from lldbtest import *
import lldbutil

class DefaultCacheLineSizeTestCase(TestBase):

    mydir = TestBase.compute_mydir(__file__)

    @skipUnlessPlatform(['android'])
    def test_cache_line_size(self):
        self.build(dictionary=self.getBuildFlags())
        exe = os.path.join(os.getcwd(), "a.out")
        target = self.dbg.CreateTarget(exe)
        self.assertTrue(target and target.IsValid(), "Target is valid")

        breakpoint = target.BreakpointCreateByName("main")
        self.assertTrue(breakpoint and breakpoint.IsValid(), "Breakpoint is valid")

        # Run the program.
        process = target.LaunchSimple(None, None, self.get_process_working_directory())
        self.assertTrue(process and process.IsValid(), PROCESS_IS_VALID)
        self.assertEqual(process.GetState(), lldb.eStateStopped, PROCESS_STOPPED)

        # check the setting value
        self.expect("settings show target.process.memory-cache-line-size", patterns=[" = 2048"])

        # Run to completion.
        process.Continue()
        self.assertEqual(process.GetState(), lldb.eStateExited, PROCESS_EXITED)

if __name__ == '__main__':
    import atexit
    lldb.SBDebugger.Initialize()
    atexit.register(lambda: lldb.SBDebugger.Terminate())
    unittest2.main()
