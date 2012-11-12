#!/usr/bin/python

#----------------------------------------------------------------------
# Be sure to add the python path that points to the LLDB shared library.
#
# # To use this in the embedded python interpreter using "lldb" just
# import it with the full path using the "command script import" 
# command
#   (lldb) command script import /path/to/cmdtemplate.py
#----------------------------------------------------------------------

import lldb
import commands
import optparse
import shlex

def create_ls_options():
    usage = "usage: %prog [options] <PATH> [PATH ...]"
    description='''This command lets you run the /bin/ls shell command from
within lldb. This code is designed to demonstrate the best principles that 
should be used when creating a new LLDB command through python.
Creating the options in a separate function allows the parser to be
created without running the command. The usage string is generated by the
optparse module and can be used to populate the ls.__doc__ documentation
string in the command interpreter function prior to registering the
command with LLDB. The allows the output of "ls --help" to exactly match
the output of "help ls" when both commands are run from within LLDB.
'''
    parser = optparse.OptionParser(description=description, prog='ls',usage=usage)
    parser.add_option('-v', '--verbose', action='store_true', dest='verbose', help='display verbose debug info', default=False)
    return parser

def ls(debugger, command, result, dict):
    # Use the Shell Lexer to properly parse up command options just like a 
    # shell would
    command_args = shlex.split(command)
    parser = create_ls_options()
    try:
        (options, args) = parser.parse_args(command_args)
    except:
        return
    
    for arg in args:
        if options.verbose:
            result.PutCString(commands.getoutput('/bin/ls "%s"' % arg))
        else:
            result.PutCString(commands.getoutput('/bin/ls -lAF "%s"' % arg))

def __lldb_init_module (debugger, dict):
    # This initializer is being run from LLDB in the embedded command interpreter    
    # Make the options so we can generate the help text for the new LLDB 
    # command line command prior to registering it with LLDB below
    parser = create_ls_options()
    ls.__doc__ = parser.format_help()
    # Add any commands contained in this module to LLDB
    debugger.HandleCommand('command script add -f cmdtemplate.ls ls')
    print 'The "ls" command has been installed, type "help ls" or "ls --help" for detailed help.'
