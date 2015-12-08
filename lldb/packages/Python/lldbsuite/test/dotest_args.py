from __future__ import print_function
from __future__ import absolute_import

# System modules
import argparse
import sys
import multiprocessing
import os
import textwrap

# Third-party modules

# LLDB modules

class ArgParseNamespace(object):
    pass

def parse_args(parser, argv):
    """ Returns an argument object. LLDB_TEST_ARGUMENTS environment variable can
        be used to pass additional arguments.
    """
    args = ArgParseNamespace()

    if ('LLDB_TEST_ARGUMENTS' in os.environ):
        print("Arguments passed through environment: '%s'" % os.environ['LLDB_TEST_ARGUMENTS'])
        args = parser.parse_args([sys.argv[0]].__add__(os.environ['LLDB_TEST_ARGUMENTS'].split()),namespace=args)

    return parser.parse_args(args=argv, namespace=args)


def default_thread_count():
    # Check if specified in the environment
    num_threads_str = os.environ.get("LLDB_TEST_THREADS")
    if num_threads_str:
        return int(num_threads_str)
    else:
        return multiprocessing.cpu_count()


def create_parser():
    parser = argparse.ArgumentParser(description='description', prefix_chars='+-', add_help=False)
    group = None

    # Helper function for boolean options (group will point to the current group when executing X)
    X = lambda optstr, helpstr, **kwargs: group.add_argument(optstr, help=helpstr, action='store_true', **kwargs)

    group = parser.add_argument_group('Help')
    group.add_argument('-h', '--help', dest='h', action='store_true', help="Print this help message and exit.  Add '-v' for more detailed help.")

    # C and Python toolchain options
    group = parser.add_argument_group('Toolchain options')
    group.add_argument('-A', '--arch', metavar='arch', action='append', dest='archs', help=textwrap.dedent('''Specify the architecture(s) to test. This option can be specified more than once'''))
    group.add_argument('-C', '--compiler', metavar='compiler', dest='compilers', action='append', help=textwrap.dedent('''Specify the compiler(s) used to build the inferior executables. The compiler path can be an executable basename or a full path to a compiler executable. This option can be specified multiple times.'''))
    if sys.platform == 'darwin':
        group.add_argument('--apple-sdk', metavar='apple_sdk', dest='apple_sdk', help=textwrap.dedent('''Specify the name of the Apple SDK (macosx, macosx.internal, iphoneos, iphoneos.internal, or path to SDK) and use the appropriate tools from that SDK's toolchain.'''))
    # FIXME? This won't work for different extra flags according to each arch.
    group.add_argument('-E', metavar='extra-flags', help=textwrap.dedent('''Specify the extra flags to be passed to the toolchain when building the inferior programs to be debugged
                                                           suggestions: do not lump the "-A arch1 -A arch2" together such that the -E option applies to only one of the architectures'''))

    # Test filtering options
    group = parser.add_argument_group('Test filtering options')
    group.add_argument('-N', choices=['dwarf', 'dwo', 'dsym'], help="Don't do test cases marked with the @dsym_test/@dwarf_test/@dwo_test decorator by passing dsym/dwarf/dwo as the option arg")
    group.add_argument('-f', metavar='filterspec', action='append', help='Specify a filter, which consists of the test class name, a dot, followed by the test method, to only admit such test into the test suite')  # FIXME: Example?
    X('-l', "Don't skip long running tests")
    group.add_argument('-p', metavar='pattern', help='Specify a regexp filename pattern for inclusion in the test suite')
    group.add_argument('-G', '--category', metavar='category', action='append', dest='categoriesList', help=textwrap.dedent('''Specify categories of test cases of interest. Can be specified more than once.'''))
    group.add_argument('--skip-category', metavar='category', action='append', dest='skipCategories', help=textwrap.dedent('''Specify categories of test cases to skip. Takes precedence over -G. Can be specified more than once.'''))

    # Configuration options
    group = parser.add_argument_group('Configuration options')
    group.add_argument('-c', metavar='config-file', help='Read a config file specified after this option')  # FIXME: additional doc.
    group.add_argument('--framework', metavar='framework-path', help='The path to LLDB.framework')
    group.add_argument('--executable', metavar='executable-path', help='The path to the lldb executable')
    group.add_argument('--libcxx', metavar='directory', help='The path to custom libc++ library')
    group.add_argument('-e', metavar='benchmark-exe', help='Specify the full path of an executable used for benchmark purposes (see also: -x)')
    group.add_argument('-k', metavar='command', action='append', help="Specify a runhook, which is an lldb command to be executed by the debugger; The option can occur multiple times. The commands are executed one after the other to bring the debugger to a desired state, so that, for example, further benchmarking can be done")
    group.add_argument('-R', metavar='dir', help='Specify a directory to relocate the tests and their intermediate files to. BE WARNED THAT the directory, if exists, will be deleted before running this test driver. No cleanup of intermediate test files is performed in this case')
    group.add_argument('-r', metavar='dir', help="Similar to '-R', except that the directory must not exist before running this test driver")
    group.add_argument('-s', metavar='name', help='Specify the name of the dir created to store the session files of tests with errored or failed status. If not specified, the test driver uses the timestamp as the session dir name')
    group.add_argument('-x', metavar='breakpoint-spec', help='Specify the breakpoint specification for the benchmark executable')
    group.add_argument('-y', type=int, metavar='count', help="Specify the iteration count used to collect our benchmarks. An example is the number of times to do 'thread step-over' to measure stepping speed.")
    group.add_argument('-#', type=int, metavar='sharp', dest='sharp', help='Repeat the test suite for a specified number of times')
    group.add_argument('--channel', metavar='channel', dest='channels', action='append', help=textwrap.dedent("Specify the log channels (and optional categories) e.g. 'lldb all' or 'gdb-remote packets' if no categories are specified, 'default' is used"))
    group.add_argument('--log-success', dest='log_success', action='store_true', help="Leave logs/traces even for successful test runs (useful for creating reference log files during debugging.)")

    # Configuration options
    group = parser.add_argument_group('Remote platform options')
    group.add_argument('--platform-name', dest='lldb_platform_name', metavar='platform-name', help='The name of a remote platform to use')
    group.add_argument('--platform-url', dest='lldb_platform_url', metavar='platform-url', help='A LLDB platform URL to use when connecting to a remote platform to run the test suite')
    group.add_argument('--platform-working-dir', dest='lldb_platform_working_dir', metavar='platform-working-dir', help='The directory to use on the remote platform.')

    # Test-suite behaviour
    group = parser.add_argument_group('Runtime behaviour options')
    X('-d', 'Suspend the process after launch to wait indefinitely for a debugger to attach')
    X('-F', 'Fail fast. Stop the test suite on the first error/failure')
    X('-i', "Ignore (don't bailout) if 'lldb.py' module cannot be located in the build tree relative to this script; use PYTHONPATH to locate the module")
    X('-n', "Don't print the headers like build dir, lldb version, and svn info at all")
    X('-P', "Use the graphic progress bar.")
    X('-q', "Don't print extra output from this script.")
    X('-S', "Skip the build and cleanup while running the test. Use this option with care as you would need to build the inferior(s) by hand and build the executable(s) with the correct name(s). This can be used with '-# n' to stress test certain test cases for n number of times")
    X('-t', 'Turn on tracing of lldb command and other detailed test executions')
    group.add_argument('-u', dest='unset_env_varnames', metavar='variable', action='append', help='Specify an environment variable to unset before running the test cases. e.g., -u DYLD_INSERT_LIBRARIES -u MallocScribble')
    group.add_argument('--env', dest='set_env_vars', metavar='variable', action='append', help='Specify an environment variable to set to the given value before running the test cases e.g.: --env CXXFLAGS=-O3 --env DYLD_INSERT_LIBRARIES')
    X('-v', 'Do verbose mode of unittest framework (print out each test case invocation)')
    X('-w', 'Insert some wait time (currently 0.5 sec) between consecutive test cases')
    X('-T', 'Obtain and dump svn information for this checkout of LLDB (off by default)')
    group.add_argument('--enable-crash-dialog', dest='disable_crash_dialog', action='store_false', help='(Windows only) When LLDB crashes, display the Windows crash dialog.')
    group.add_argument('--show-inferior-console', dest='hide_inferior_console', action='store_false', help='(Windows only) When launching an inferior, dont hide its console window.')
    group.set_defaults(disable_crash_dialog=True)
    group.set_defaults(hide_inferior_console=True)

    group = parser.add_argument_group('Parallel execution options')
    group.add_argument(
        '--inferior',
        action='store_true',
        help=('specify this invocation is a multiprocess inferior, '
              'used internally'))
    group.add_argument(
        '--no-multiprocess',
        action='store_true',
        help='skip running the multiprocess test runner')
    group.add_argument(
        '--output-on-success',
        action='store_true',
        help=('print full output of the dotest.py inferior, '
              'even when all tests succeed'))
    group.add_argument(
        '--threads',
        type=int,
        dest='num_threads',
        default=default_thread_count(),
        help=('The number of threads/processes to use when running tests '
              'separately, defaults to the number of CPU cores available'))
    group.add_argument(
        '--test-subdir',
        action='store',
        help='Specify a test subdirectory to use relative to the test root dir'
    )
    group.add_argument(
        '--test-runner-name',
        action='store',
        help=('Specify a test runner strategy.  Valid values: multiprocessing,'
              ' multiprocessing-pool, serial, threading, threading-pool')
    )

    # Test results support.
    group = parser.add_argument_group('Test results options')
    group.add_argument(
        '--curses',
        action='store_true',
        help='Shortcut for specifying test results using the curses formatter')
    group.add_argument(
        '--results-file',
        action='store',
        help=('Specifies the file where test results will be written '
              'according to the results-formatter class used'))
    group.add_argument(
        '--results-port',
        action='store',
        type=int,
        help=('Specifies the localhost port to which the results '
              'formatted output should be sent'))
    group.add_argument(
        '--results-formatter',
        action='store',
        help=('Specifies the full package/module/class name used to translate '
              'test events into some kind of meaningful report, written to '
              'the designated output results file-like object'))
    group.add_argument(
        '--results-formatter-option',
        '-O',
        action='append',
        dest='results_formatter_options',
        help=('Specify an option to pass to the formatter. '
              'Use --results-formatter-option="--option1=val1" '
              'syntax.  Note the "=" is critical, don\'t include whitespace.'))
    group.add_argument(
        '--event-add-entries',
        action='store',
        help=('Specify comma-separated KEY=VAL entries to add key and value '
              'pairs to all test events generated by this test run.  VAL may '
              'be specified as VAL:TYPE, where TYPE may be int to convert '
              'the value to an int'))
    # Remove the reference to our helper function
    del X

    D = lambda optstr, **kwargs: group.add_argument(optstr, action='store_true', **kwargs)
    group = parser.add_argument_group('Deprecated options (do not use)')
    # Deprecated on 23.10.2015. Remove completely after a grace period.
    D('-a')
    D('+a', dest='plus_a')
    D('-m')
    D('+m', dest='plus_m')
    del D

    group = parser.add_argument_group('Test directories')
    group.add_argument('args', metavar='test-dir', nargs='*', help='Specify a list of directory names to search for test modules named after Test*.py (test discovery). If empty, search from the current working directory instead.')

    return parser
