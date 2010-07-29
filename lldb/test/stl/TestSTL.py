"""
Test that we can successfully step into an STL function.
"""

import os, time
import unittest
import lldb
import lldbtest
import time

class TestSTL(lldbtest.TestBase):

    mydir = "stl"

    def test_step_into_stl(self):
        """Test that we can successfully step into an STL function."""
        res = self.res
        exe = os.path.join(os.getcwd(), "a.out")
        # The following two lines, if uncommented, will enable loggings.
        #self.ci.HandleCommand("log enable -f /tmp/lldb.log lldb default", res)
        #self.assertTrue(res.Succeeded())
        self.ci.HandleCommand("file " + exe, res)
        self.assertTrue(res.Succeeded())

        # Break on line 13 of main.cpp.
        self.ci.HandleCommand("breakpoint set -f main.cpp -l 13", res)
        self.assertTrue(res.Succeeded())
        self.assertTrue(res.GetOutput().startswith(
            "Breakpoint created: 1: file ='main.cpp', line = 13, locations = 1")
                        )

        self.ci.HandleCommand("run", res)
        time.sleep(0.1)
        self.assertTrue(res.Succeeded())

        # Stop at 'std::string hello_world ("Hello World!");'.
        self.ci.HandleCommand("thread list", res)
        print "thread list ->", res.GetOutput()
        self.assertTrue(res.Succeeded())
        output = res.GetOutput()
        self.assertTrue(output.find('main.cpp:13') > 0 and
                        output.find('stop reason = breakpoint') > 0)

        # The breakpoint should have a hit count of 1.
        self.ci.HandleCommand("breakpoint list", res)
        self.assertTrue(res.Succeeded())
        self.assertTrue(res.GetOutput().find(' resolved, hit count = 1') > 0)

        # Now do 'thread step-in', we should stop on the basic_string template.
        self.ci.HandleCommand("thread step-in", res)
        print "thread step-in:", res.GetOutput()

        #
        # This assertion currently always fails.
        # This might be related: rdar://problem/8247112.
        #
        self.assertTrue(res.Succeeded())

        #self.ci.HandleCommand("process status", res)
        #print "process status:", res.GetOutput()
        self.ci.HandleCommand("thread backtrace", res)
        print "thread backtrace:", res.GetOutput()
        self.assertTrue(res.Succeeded())
        output = res.GetOutput()
        self.assertTrue(output.find('[inlined]') > 0 and
                        output.find('basic_string.h') and
                        output.find('stop reason = step in,') > 0)

        self.ci.HandleCommand("continue", res)
        self.assertTrue(res.Succeeded())


if __name__ == '__main__':
    lldb.SBDebugger.Initialize()
    unittest.main()
    lldb.SBDebugger.Terminate()
