//===-- CommandObjectHelp.cpp -----------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "CommandObjectHelp.h"

// C Includes
// C++ Includes
// Other libraries and framework includes
// Project includes
#include "lldb/Interpreter/CommandObjectMultiword.h"
#include "lldb/Interpreter/CommandInterpreter.h"
#include "lldb/Interpreter/Options.h"
#include "lldb/Interpreter/CommandReturnObject.h"

using namespace lldb;
using namespace lldb_private;

//-------------------------------------------------------------------------
// CommandObjectHelp
//-------------------------------------------------------------------------

CommandObjectHelp::CommandObjectHelp (CommandInterpreter &interpreter) :
    CommandObject (interpreter,
                   "help",
                   "Show a list of all debugger commands, or give details about specific commands.",
                   "help [<cmd-name>]")
{
    CommandArgumentEntry arg;
    CommandArgumentData command_arg;

    // Define the first (and only) variant of this arg.
    command_arg.arg_type = eArgTypeCommandName;
    command_arg.arg_repetition = eArgRepeatStar;

    // There is only one variant this argument could be; put it into the argument entry.
    arg.push_back (command_arg);

    // Push the data for the first argument into the m_arguments vector.
    m_arguments.push_back (arg);
}

CommandObjectHelp::~CommandObjectHelp()
{
}


bool
CommandObjectHelp::Execute (Args& command, CommandReturnObject &result)
{
    CommandObject::CommandMap::iterator pos;
    CommandObject *cmd_obj;
    const int argc = command.GetArgumentCount ();
    
    // 'help' doesn't take any options or arguments, other than command names.  If argc is 0, we show the user
    // all commands and aliases.  Otherwise every argument must be the name of a command or a sub-command.
    if (argc == 0)
    {
        result.SetStatus (eReturnStatusSuccessFinishNoResult);
        m_interpreter.GetHelp (result);  // General help, for ALL commands.
    }
    else
    {
        // Get command object for the first command argument. Only search built-in command dictionary.
        StringList matches;
        cmd_obj = m_interpreter.GetCommandObject (command.GetArgumentAtIndex (0), &matches);
        
        if (cmd_obj != NULL)
        {
            bool all_okay = true;
            CommandObject *sub_cmd_obj = cmd_obj;
            // Loop down through sub_command dictionaries until we find the command object that corresponds
            // to the help command entered.
            for (int i = 1; i < argc && all_okay; ++i)
            {
                std::string sub_command = command.GetArgumentAtIndex(i);
                if (! sub_cmd_obj->IsMultiwordObject ())
                {
                    all_okay = false;
                }
                else
                {
                    pos = ((CommandObjectMultiword *) sub_cmd_obj)->m_subcommand_dict.find (sub_command);
                    if (pos != ((CommandObjectMultiword *) sub_cmd_obj)->m_subcommand_dict.end())
                        sub_cmd_obj = pos->second.get();
                    else
                        all_okay = false;
                }
            }
            
            if (!all_okay || (sub_cmd_obj == NULL))
            {
                std::string cmd_string;
                command.GetCommandString (cmd_string);
                result.AppendErrorWithFormat
                ("'%s' is not a known command.\nTry 'help' to see a current list of commands.\n",
                 cmd_string.c_str());
                result.SetStatus (eReturnStatusFailed);
            }
            else
            {
                Stream &output_strm = result.GetOutputStream();
                if (sub_cmd_obj->GetOptions() != NULL)
                {
                    if (sub_cmd_obj->WantsRawCommandString())
                    {
                        std::string help_text (sub_cmd_obj->GetHelp());
                        help_text.append ("  This command takes 'raw' input (no need to quote stuff).");
                        m_interpreter.OutputFormattedHelpText (output_strm, "", "", help_text.c_str(), 1);
                    }
                    else
                        m_interpreter.OutputFormattedHelpText (output_strm, "", "", sub_cmd_obj->GetHelp(), 1);
                    output_strm.Printf ("\nSyntax: %s\n", sub_cmd_obj->GetSyntax());
                    sub_cmd_obj->GetOptions()->GenerateOptionUsage (m_interpreter, output_strm, sub_cmd_obj);
                    const char *long_help = sub_cmd_obj->GetHelpLong();
                    if ((long_help != NULL)
                        && (strlen (long_help) > 0))
                        output_strm.Printf ("\n%s", long_help);
                    // Mark this help command with a success status.
                    result.SetStatus (eReturnStatusSuccessFinishNoResult);
                }
                else if (sub_cmd_obj->IsMultiwordObject())
                {
                    if (sub_cmd_obj->WantsRawCommandString())
                    {
                        std::string help_text (sub_cmd_obj->GetHelp());
                        help_text.append ("  This command takes 'raw' input (no need to quote stuff).");
                        m_interpreter.OutputFormattedHelpText (output_strm, "", "", help_text.c_str(), 1);
                    }
                    else
                        m_interpreter.OutputFormattedHelpText (output_strm, "", "", sub_cmd_obj->GetHelp(), 1);
                    ((CommandObjectMultiword *) sub_cmd_obj)->GenerateHelpText (result);
                }
                else
                {
                    const char *long_help = sub_cmd_obj->GetHelpLong();
                    if ((long_help != NULL)
                        && (strlen (long_help) > 0))
                        output_strm.Printf ("\n%s", long_help);
                    else if (sub_cmd_obj->WantsRawCommandString())
                    {
                        std::string help_text (sub_cmd_obj->GetHelp());
                        help_text.append ("  This command takes 'raw' input (no need to quote stuff).");
                        m_interpreter.OutputFormattedHelpText (output_strm, "", "", help_text.c_str(), 1);
                    }
                    else
                        m_interpreter.OutputFormattedHelpText (output_strm, "", "", sub_cmd_obj->GetHelp(), 1);
                    output_strm.Printf ("\nSyntax: %s\n", sub_cmd_obj->GetSyntax());
                    // Mark this help command with a success status.
                    result.SetStatus (eReturnStatusSuccessFinishNoResult);
                }
            }
        }
        else if (matches.GetSize() > 0)
        {
            Stream &output_strm = result.GetOutputStream();
            output_strm.Printf("Help requested with ambiguous command name, possible completions:\n");
            const uint32_t match_count = matches.GetSize();
            for (uint32_t i = 0; i < match_count; i++)
            {
                output_strm.Printf("\t%s\n", matches.GetStringAtIndex(i));
            }
        }
        else
        {
            // Maybe the user is asking for help about a command argument rather than a command.
            const CommandArgumentType arg_type = CommandObject::LookupArgumentName (command.GetArgumentAtIndex (0));
            if (arg_type != eArgTypeLastArg)
            {
                Stream &output_strm = result.GetOutputStream ();
                CommandObject::GetArgumentHelp (output_strm, arg_type, m_interpreter);
                result.SetStatus (eReturnStatusSuccessFinishNoResult);
            }
            else
            {
                result.AppendErrorWithFormat 
                    ("'%s' is not a known command.\nTry 'help' to see a current list of commands.\n",
                     command.GetArgumentAtIndex(0));
                result.SetStatus (eReturnStatusFailed);
            }
        }
    }
    
    return result.Succeeded();
}

int
CommandObjectHelp::HandleCompletion
(
    Args &input,
    int &cursor_index,
    int &cursor_char_position,
    int match_start_point,
    int max_return_elements,
    bool &word_complete,
    StringList &matches
)
{
    // Return the completions of the commands in the help system:
    if (cursor_index == 0)
    {
        return m_interpreter.HandleCompletionMatches (input, 
                                                    cursor_index, 
                                                    cursor_char_position, 
                                                    match_start_point, 
                                                    max_return_elements, 
                                                    word_complete, 
                                                    matches);
    }
    else
    {
        CommandObject *cmd_obj = m_interpreter.GetCommandObject (input.GetArgumentAtIndex(0));
        input.Shift();
        cursor_index--;
        return cmd_obj->HandleCompletion (input, 
                                          cursor_index, 
                                          cursor_char_position, 
                                          match_start_point, 
                                          max_return_elements, 
                                          word_complete, 
                                          matches);
    }
}
