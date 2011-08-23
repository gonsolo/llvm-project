"""
Test lldb data formatter subsystem.
"""

import os, time
import unittest2
import lldb
from lldbtest import *

class CppDataFormatterTestCase(TestBase):

    mydir = os.path.join("functionalities", "data-formatter", "data-formatter-cpp")

    @unittest2.skipUnless(sys.platform.startswith("darwin"), "requires Darwin")
    def test_with_dsym_and_run_command(self):
        """Test data formatter commands."""
        self.buildDsym()
        self.data_formatter_commands()

    def test_with_dwarf_and_run_command(self):
        """Test data formatter commands."""
        self.buildDwarf()
        self.data_formatter_commands()

    def setUp(self):
        # Call super's setUp().
        TestBase.setUp(self)
        # Find the line number to break at.
        self.line = line_number('main.cpp', '// Set break point at this line.')

    def data_formatter_commands(self):
        """Test that that file and class static variables display correctly."""
        self.runCmd("file a.out", CURRENT_EXECUTABLE_SET)

        self.expect("breakpoint set -f main.cpp -l %d" % self.line,
                    BREAKPOINT_CREATED,
            startstr = "Breakpoint created: 1: file ='main.cpp', line = %d, locations = 1" %
                        self.line)

        self.runCmd("run", RUN_SUCCEEDED)

        # The stop reason of the thread should be breakpoint.
        self.expect("thread list", STOPPED_DUE_TO_BREAKPOINT,
            substrs = ['stopped',
                       'stop reason = breakpoint'])

        self.expect("frame variable",
            substrs = ['(Speed) SPILookHex = 5.55' # Speed by default is 5.55.
                        ]);

        # This is the function to remove the custom formats in order to have a
        # clean slate for the next test case.
        def cleanup():
            self.runCmd('type format clear', check=False)
            self.runCmd('type summary clear', check=False)

        # Execute the cleanup function during test case tear down.
        self.addTearDownHook(cleanup)

        self.runCmd("type format add -C yes -f x Speed BitField")
        self.runCmd("type format add -C no -f c RealNumber")
        self.runCmd("type format add -C no -f x Type2")
        self.runCmd("type format add -C yes -f c Type1")

        # The type format list should show our custom formats.
        self.expect("type format list",
            substrs = ['RealNumber',
                       'Speed',
                       'BitField',
                       'Type1',
                       'Type2'])

        self.expect("frame variable",
            patterns = ['\(Speed\) SPILookHex = 0x[0-9a-f]+' # Speed should look hex-ish now.
                        ]);
        
        # gcc4.2 on Mac OS X skips typedef chains in the DWARF output
        if self.getCompiler() in ['clang', 'llvm-gcc']:        
            self.expect("frame variable",
                patterns = ['\(SignalMask\) SMILookHex = 0x[0-9a-f]+' # SignalMask should look hex-ish now.
                            ]);
            self.expect("frame variable", matching=False,
                        patterns = ['\(Type4\) T4ILookChar = 0x[0-9a-f]+' # Type4 should NOT look hex-ish now.
                                    ]);
        
        # Now let's delete the 'Speed' custom format.
        self.runCmd("type format delete Speed")

        # The type format list should not show 'Speed' at this point.
        self.expect("type format list", matching=False,
            substrs = ['Speed'])

        # Delete type format for 'Speed', we should expect an error message.
        self.expect("type format delete Speed", error=True,
            substrs = ['no custom format for Speed'])
        
        self.runCmd("type summary add --summary-string \"arr = ${var%s}\" -x \"char \\[[0-9]+\\]\" -v")
        
        self.expect("frame variable strarr",
                    substrs = ['arr = "Hello world!"'])
        
        self.runCmd("type summary clear")
        
        self.runCmd("type summary add --summary-string \"ptr = ${var%s}\" \"char *\" -v")
        
        self.expect("frame variable strptr",
                    substrs = ['ptr = "Hello world!"'])
        
        self.runCmd("type summary add --summary-string \"arr = ${var%s}\" -x \"char \\[[0-9]+\\]\" -v")
        
        self.expect("frame variable strarr",
                    substrs = ['arr = "Hello world!'])
        
        self.expect("frame variable strptr",
                    substrs = ['ptr = "Hello world!"'])

        self.runCmd("type summary add -c Point")
            
        self.expect("frame variable iAmSomewhere",
            substrs = ['x=4',
                       'y=6'])
        
        self.expect("type summary list",
            substrs = ['Point',
                       'one-line'])

        self.runCmd("type summary add --summary-string \"y=${var.y%x}\" Point")

        self.expect("frame variable iAmSomewhere",
            substrs = ['y=0x'])
        
        self.runCmd("type summary add --summary-string \"y=${var.y},x=${var.x}\" Point")
        
        self.expect("frame variable iAmSomewhere",
                    substrs = ['y=6',
                               'x=4'])

        self.runCmd("type summary add --summary-string \"hello\" Point -e")

        self.expect("type summary list",
            substrs = ['Point',
                       'show children'])
        
        self.expect("frame variable iAmSomewhere",
            substrs = ['hello',
                       'x = 4',
                       '}'])

        self.runCmd("type summary add --summary-string \"Sign: ${var[31]%B} Exponent: ${var[23-30]%x} Mantissa: ${var[0-22]%u}\" ShowMyGuts")

        self.expect("frame variable cool_pointer->floating",
            substrs = ['Sign: true',
                       'Exponent: 0x',
                       '80'])

        self.runCmd("type summary add --summary-string \"a test\" i_am_cool")

        self.expect("frame variable cool_pointer",
            substrs = ['a test'])

        self.runCmd("type summary add --summary-string \"a test\" i_am_cool --skip-pointers")
        
        self.expect("frame variable cool_pointer",
            substrs = ['a test'],
            matching = False)

        self.runCmd("type summary add --summary-string \"${var[1-3]}\" \"int [5]\"")

        self.expect("frame variable int_array",
            substrs = ['2',
                       '3',
                       '4'])

        self.runCmd("type summary clear")

        self.runCmd("type summary add --summary-string \"${var[0-2].integer}\" \"i_am_cool *\"")
        self.runCmd("type summary add --summary-string \"${var[2-4].integer}\" \"i_am_cool [5]\"")

        self.expect("frame variable cool_array",
            substrs = ['1,1,6'])

        self.expect("frame variable cool_pointer",
            substrs = ['3,0,0'])

        # test special symbols for formatting variables into summaries
        self.runCmd("type summary add --summary-string \"cool object @ ${var%L}\" i_am_cool")
        self.runCmd("type summary delete \"i_am_cool [5]\"")
        
        # this test might fail if the compiler tries to store
        # these values into registers.. hopefully this is not
        # going to be the case
        self.expect("frame variable cool_array",
            substrs = ['[0] = cool object @ 0x',
                       '[1] = cool object @ 0x',
                       '[2] = cool object @ 0x',
                       '[3] = cool object @ 0x',
                       '[4] = cool object @ 0x'])
                            
        # test getting similar output by exploiting ${var} = 'type @ location' for aggregates
        self.runCmd("type summary add --summary-string \"${var}\" i_am_cool")
        
        # this test might fail if the compiler tries to store
        # these values into registers.. hopefully this is not
        # going to be the case
        self.expect("frame variable cool_array",
                    substrs = ['[0] = i_am_cool @ 0x',
                               '[1] = i_am_cool @ 0x',
                               '[2] = i_am_cool @ 0x',
                               '[3] = i_am_cool @ 0x',
                               '[4] = i_am_cool @ 0x'])

            
        # test getting same output by exploiting %T and %L together for aggregates
        self.runCmd("type summary add --summary-string \"${var%T} @ ${var%L}\" i_am_cool")
        
        # this test might fail if the compiler tries to store
        # these values into registers.. hopefully this is not
        # going to be the case
        self.expect("frame variable cool_array",
                    substrs = ['[0] = i_am_cool @ 0x',
                               '[1] = i_am_cool @ 0x',
                               '[2] = i_am_cool @ 0x',
                               '[3] = i_am_cool @ 0x',
                               '[4] = i_am_cool @ 0x'])
                            
        self.runCmd("type summary add --summary-string \"goofy\" i_am_cool")
        self.runCmd("type summary add --summary-string \"${var.second_cool%S}\" i_am_cooler")

        self.expect("frame variable the_coolest_guy",
            substrs = ['(i_am_cooler) the_coolest_guy = goofy'])

        # check that unwanted type specifiers are removed
        self.runCmd("type summary delete i_am_cool")
        self.runCmd("type summary add --summary-string \"goofy\" \"class i_am_cool\"")
        self.expect("frame variable the_coolest_guy",
                substrs = ['(i_am_cooler) the_coolest_guy = goofy'])

        self.runCmd("type summary delete i_am_cool")
        self.runCmd("type summary add --summary-string \"goofy\" \"enum i_am_cool\"")
        self.expect("frame variable the_coolest_guy",
                    substrs = ['(i_am_cooler) the_coolest_guy = goofy'])

        self.runCmd("type summary delete i_am_cool")
        self.runCmd("type summary add --summary-string \"goofy\" \"struct i_am_cool\"")
        self.expect("frame variable the_coolest_guy",
                    substrs = ['(i_am_cooler) the_coolest_guy = goofy'])

        # many spaces, but we still do the right thing
        self.runCmd("type summary delete i_am_cool")
        self.runCmd("type summary add --summary-string \"goofy\" \"union     i_am_cool\"")
        self.expect("frame variable the_coolest_guy",
                    substrs = ['(i_am_cooler) the_coolest_guy = goofy'])

        # but that not *every* specifier is removed
        self.runCmd("type summary delete i_am_cool")
        self.runCmd("type summary add --summary-string \"goofy\" \"wrong i_am_cool\"")
        self.expect("frame variable the_coolest_guy", matching=False,
                    substrs = ['(i_am_cooler) the_coolest_guy = goofy'])


if __name__ == '__main__':
    import atexit
    lldb.SBDebugger.Initialize()
    atexit.register(lambda: lldb.SBDebugger.Terminate())
    unittest2.main()
