#include "llvm/Support/ToolRunner.h"
#include "Support/Debug.h"
#include "Support/FileUtilities.h"

//===---------------------------------------------------------------------===//
// LLI Implementation of AbstractIntepreter interface
//
class LLI : public AbstractInterpreter {
  std::string LLIPath;          // The path to the LLI executable
public:
  LLI(const std::string &Path) : LLIPath(Path) { }


  virtual int ExecuteProgram(const std::string &Bytecode,
                             const cl::list<std::string> &Args,
                             const std::string &InputFile,
                             const std::string &OutputFile,
                             const std::string &SharedLib = "");
};

int LLI::ExecuteProgram(const std::string &Bytecode,
                        const cl::list<std::string> &Args,
                        const std::string &InputFile,
                        const std::string &OutputFile,
                        const std::string &SharedLib) {
  if (!SharedLib.empty()) {
    std::cerr << "LLI currently does not support loading shared libraries.\n"
              << "Exiting.\n";
    exit(1);
  }

  std::vector<const char*> LLIArgs;
  LLIArgs.push_back(LLIPath.c_str());
  LLIArgs.push_back("-quiet");
  LLIArgs.push_back("-force-interpreter=true");
  LLIArgs.push_back(Bytecode.c_str());
  // Add optional parameters to the running program from Argv
  for (unsigned i=0, e = Args.size(); i != e; ++i)
    LLIArgs.push_back(Args[i].c_str());
  LLIArgs.push_back(0);

  std::cout << "<lli>" << std::flush;
  DEBUG(std::cerr << "\nAbout to run:\n\t";
        for (unsigned i=0, e = LLIArgs.size(); i != e; ++i)
          std::cerr << " " << LLIArgs[i];
        std::cerr << "\n";
        );
  return RunProgramWithTimeout(LLIPath, &LLIArgs[0],
                               InputFile, OutputFile, OutputFile);
}

// LLI create method - Try to find the LLI executable
AbstractInterpreter *createLLItool(const std::string &ProgramPath, 
                                   std::string &Message) {
  std::string LLIPath = FindExecutable("lli", ProgramPath);
  if (!LLIPath.empty()) {
    Message = "Found lli: " + LLIPath + "\n";
    return new LLI(LLIPath);
  }

  Message = "Cannot find `lli' in executable directory or PATH!\n";
  return 0;
}

//===----------------------------------------------------------------------===//
// LLC Implementation of AbstractIntepreter interface
//
int LLC::OutputAsm(const std::string &Bytecode,
                   std::string &OutputAsmFile) {
  OutputAsmFile = getUniqueFilename(Bytecode+".llc.s");
  const char *LLCArgs[] = {
    LLCPath.c_str(),
    "-o", OutputAsmFile.c_str(), // Output to the Asm file
    "-f",                        // Overwrite as necessary...
    Bytecode.c_str(),            // This is the input bytecode
    0
  };

  std::cout << "<llc>" << std::flush;
  if (RunProgramWithTimeout(LLCPath, LLCArgs, "/dev/null", "/dev/null",
                            "/dev/null")) {                            
    // If LLC failed on the bytecode, print error...
    std::cerr << "Error: `llc' failed!\n";
    removeFile(OutputAsmFile);
    return 1;
  }

  return 0;
}

int LLC::ExecuteProgram(const std::string &Bytecode,
                        const cl::list<std::string> &Args,
                        const std::string &InputFile,
                        const std::string &OutputFile,
                        const std::string &SharedLib) {

  std::string OutputAsmFile;
  if (OutputAsm(Bytecode, OutputAsmFile)) {
    std::cerr << "Could not generate asm code with `llc', exiting.\n";
    exit(1);
  }

  // Assuming LLC worked, compile the result with GCC and run it.
  int Result = gcc->ExecuteProgram(OutputAsmFile, Args, AsmFile,
                                   InputFile, OutputFile, SharedLib);
  removeFile(OutputAsmFile);
  return Result;
}

/// createLLCtool - Try to find the LLC executable
///
LLC *createLLCtool(const std::string &ProgramPath, std::string &Message)
{
  std::string LLCPath = FindExecutable("llc", ProgramPath);
  if (LLCPath.empty()) {
    Message = "Cannot find `llc' in executable directory or PATH!\n";
    return 0;
  }

  Message = "Found llc: " + LLCPath + "\n";
  GCC *gcc = createGCCtool(ProgramPath, Message);
  if (!gcc) {
    std::cerr << Message << "\n";
    exit(1);
  }
  return new LLC(LLCPath, gcc);
}

//===---------------------------------------------------------------------===//
// JIT Implementation of AbstractIntepreter interface
//
class JIT : public AbstractInterpreter {
  std::string LLIPath;          // The path to the LLI executable
public:
  JIT(const std::string &Path) : LLIPath(Path) { }


  virtual int ExecuteProgram(const std::string &Bytecode,
                             const cl::list<std::string> &Args,
                             const std::string &InputFile,
                             const std::string &OutputFile,
                             const std::string &SharedLib = "");
};

int JIT::ExecuteProgram(const std::string &Bytecode,
                        const cl::list<std::string> &Args,
                        const std::string &InputFile,
                        const std::string &OutputFile,
                        const std::string &SharedLib) {
  // Construct a vector of parameters, incorporating those from the command-line
  std::vector<const char*> JITArgs;
  JITArgs.push_back(LLIPath.c_str());
  JITArgs.push_back("-quiet");
  JITArgs.push_back("-force-interpreter=false");
  if (!SharedLib.empty()) {
    JITArgs.push_back("-load");
    JITArgs.push_back(SharedLib.c_str());
  }
  JITArgs.push_back(Bytecode.c_str());
  // Add optional parameters to the running program from Argv
  for (unsigned i=0, e = Args.size(); i != e; ++i)
    JITArgs.push_back(Args[i].c_str());
  JITArgs.push_back(0);

  std::cout << "<jit>" << std::flush;
  DEBUG(std::cerr << "\nAbout to run:\n\t";
        for (unsigned i=0, e = JITArgs.size(); i != e; ++i)
          std::cerr << " " << JITArgs[i];
        std::cerr << "\n";
        );
  DEBUG(std::cerr << "\nSending output to " << OutputFile << "\n");
  return RunProgramWithTimeout(LLIPath, &JITArgs[0],
                               InputFile, OutputFile, OutputFile);
}

/// createJITtool - Try to find the LLI executable
///
AbstractInterpreter *createJITtool(const std::string &ProgramPath, 
                                   std::string &Message) {
  std::string LLIPath = FindExecutable("lli", ProgramPath);
  if (!LLIPath.empty()) {
    Message = "Found lli: " + LLIPath + "\n";
    return new JIT(LLIPath);
  }

  Message = "Cannot find `lli' in executable directory or PATH!\n";
  return 0;
}

int CBE::OutputC(const std::string &Bytecode,
                 std::string &OutputCFile) {
  OutputCFile = getUniqueFilename(Bytecode+".cbe.c");
  const char *DisArgs[] = {
    DISPath.c_str(),
    "-o", OutputCFile.c_str(),   // Output to the C file
    "-c",                        // Output to C
    "-f",                        // Overwrite as necessary...
    Bytecode.c_str(),            // This is the input bytecode
    0
  };

  std::cout << "<cbe>" << std::flush;
  if (RunProgramWithTimeout(DISPath, DisArgs, "/dev/null", "/dev/null",
                            "/dev/null")) {                            
    // If dis failed on the bytecode, print error...
    std::cerr << "Error: `llvm-dis -c' failed!\n";
    return 1;
  }

  return 0;
}

int CBE::ExecuteProgram(const std::string &Bytecode,
                        const cl::list<std::string> &Args,
                        const std::string &InputFile,
                        const std::string &OutputFile,
                        const std::string &SharedLib) {
  std::string OutputCFile;
  if (OutputC(Bytecode, OutputCFile)) {
    std::cerr << "Could not generate C code with `llvm-dis', exiting.\n";
    exit(1);
  }

  int Result = gcc->ExecuteProgram(OutputCFile, Args, CFile, 
                                   InputFile, OutputFile, SharedLib);
  removeFile(OutputCFile);

  return Result;
}

/// createCBEtool - Try to find the 'dis' executable
///
CBE *createCBEtool(const std::string &ProgramPath, std::string &Message) {
  std::string DISPath = FindExecutable("llvm-dis", ProgramPath);
  if (DISPath.empty()) {
    Message = 
      "Cannot find `llvm-dis' in executable directory or PATH!\n";
    return 0;
  }

  Message = "Found llvm-dis: " + DISPath + "\n";
  GCC *gcc = createGCCtool(ProgramPath, Message);
  if (!gcc) {
    std::cerr << Message << "\n";
    exit(1);
  }
  return new CBE(DISPath, gcc);
}

//===---------------------------------------------------------------------===//
// GCC abstraction
//
// This is not a *real* AbstractInterpreter as it does not accept bytecode
// files, but only input acceptable to GCC, i.e. C, C++, and assembly files
//
int GCC::ExecuteProgram(const std::string &ProgramFile,
                        const cl::list<std::string> &Args,
                        FileType fileType,
                        const std::string &InputFile,
                        const std::string &OutputFile,
                        const std::string &SharedLib) {
  std::string OutputBinary = getUniqueFilename(ProgramFile+".gcc.exe");
  std::vector<const char*> GCCArgs;

  GCCArgs.push_back(GCCPath.c_str());
  if (!SharedLib.empty()) // Specify the shared library to link in...
    GCCArgs.push_back(SharedLib.c_str());
  GCCArgs.push_back("-x");
  if (fileType == CFile) {
    GCCArgs.push_back("c");
    GCCArgs.push_back("-fno-strict-aliasing");
  } else {
    GCCArgs.push_back("assembler");
  }
  GCCArgs.push_back(ProgramFile.c_str());  // Specify the input filename...
  GCCArgs.push_back("-o");
  GCCArgs.push_back(OutputBinary.c_str()); // Output to the right file...
  GCCArgs.push_back("-lm");                // Hard-code the math library...
  GCCArgs.push_back("-O2");                // Optimize the program a bit...
  GCCArgs.push_back(0);                    // NULL terminator

  std::cout << "<gcc>" << std::flush;
  if (RunProgramWithTimeout(GCCPath, &GCCArgs[0], "/dev/null", "/dev/null",
                            "/dev/null")) {
    ProcessFailure(&GCCArgs[0]);
    exit(1);
  }

  std::vector<const char*> ProgramArgs;
  ProgramArgs.push_back(OutputBinary.c_str());
  // Add optional parameters to the running program from Argv
  for (unsigned i=0, e = Args.size(); i != e; ++i)
    ProgramArgs.push_back(Args[i].c_str());
  ProgramArgs.push_back(0);                // NULL terminator

  // Now that we have a binary, run it!
  std::cout << "<program>" << std::flush;
  DEBUG(std::cerr << "\nAbout to run:\n\t";
        for (unsigned i=0, e = ProgramArgs.size(); i != e; ++i)
          std::cerr << " " << ProgramArgs[i];
        std::cerr << "\n";
        );
  int ProgramResult = RunProgramWithTimeout(OutputBinary, &ProgramArgs[0],
                                            InputFile, OutputFile, OutputFile);
  removeFile(OutputBinary);
  return ProgramResult;
}

int GCC::MakeSharedObject(const std::string &InputFile, FileType fileType,
                          std::string &OutputFile) {
  OutputFile = getUniqueFilename(InputFile+".so");
  // Compile the C/asm file into a shared object
  const char* GCCArgs[] = {
    GCCPath.c_str(),
    "-x", (fileType == AsmFile) ? "assembler" : "c",
    "-fno-strict-aliasing",
    InputFile.c_str(),           // Specify the input filename...
#if defined(sparc) || defined(__sparc__) || defined(__sparcv9)
    "-G",                        // Compile a shared library, `-G' for Sparc
#else                             
    "-shared",                   // `-shared' for Linux/X86, maybe others
#endif
    "-o", OutputFile.c_str(),    // Output to the right filename...
    "-O2",                       // Optimize the program a bit...
    0
  };
  
  std::cout << "<gcc>" << std::flush;
  if (RunProgramWithTimeout(GCCPath, GCCArgs, "/dev/null", "/dev/null",
                            "/dev/null")) {
    ProcessFailure(GCCArgs);
    return 1;
  }
  return 0;
}

void GCC::ProcessFailure(const char** GCCArgs) {
  std::cerr << "\n*** Error: invocation of the C compiler failed!\n";
  for (const char **Arg = GCCArgs; *Arg; ++Arg)
    std::cerr << " " << *Arg;
  std::cerr << "\n";

  // Rerun the compiler, capturing any error messages to print them.
  std::string ErrorFilename = getUniqueFilename("gcc.errors");
  RunProgramWithTimeout(GCCPath, GCCArgs, "/dev/null", ErrorFilename.c_str(),
                        ErrorFilename.c_str());

  // Print out the error messages generated by GCC if possible...
  std::ifstream ErrorFile(ErrorFilename.c_str());
  if (ErrorFile) {
    std::copy(std::istreambuf_iterator<char>(ErrorFile),
              std::istreambuf_iterator<char>(),
              std::ostreambuf_iterator<char>(std::cerr));
    ErrorFile.close();
    std::cerr << "\n";      
  }

  removeFile(ErrorFilename);
}

/// createGCCtool - Try to find the `gcc' executable
///
GCC *createGCCtool(const std::string &ProgramPath, std::string &Message) {
  std::string GCCPath = FindExecutable("gcc", ProgramPath);
  if (GCCPath.empty()) {
    Message = "Cannot find `gcc' in executable directory or PATH!\n";
    return 0;
  }

  Message = "Found gcc: " + GCCPath + "\n";
  return new GCC(GCCPath);
}
