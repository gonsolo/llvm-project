//===----------------------------------------------------------------------===//
// LLVM BugPoint Utility 
//
// This program is an automated compiler debugger tool.  It is used to narrow
// down miscompilations and crash problems to a specific pass in the compiler,
// and the specific Module or Function input that is causing the problem.
//
//===----------------------------------------------------------------------===//

#include "BugDriver.h"
#include "Support/CommandLine.h"
#include "llvm/Support/PassNameParser.h"

static cl::list<std::string>
InputFilenames(cl::Positional, cl::OneOrMore,
               cl::desc("<input llvm ll/bc files>"));

// The AnalysesList is automatically populated with registered Passes by the
// PassNameParser.
//
static cl::list<const PassInfo*, bool, PassNameParser>
PassList(cl::desc("Passes available:"), cl::OneOrMore);

//cl::list<std::string>
//InputArgv(cl::ConsumeAfter, cl::desc("<program arguments>..."));

//cl::opt<bool>
//Verbose("v", cl::desc("Enable verbose output"));

int main(int argc, char **argv) {
  cl::ParseCommandLineOptions(argc, argv);

  
  BugDriver D(argv[0]);
  if (D.addSources(InputFilenames)) return 1;
  D.addPasses(PassList.begin(), PassList.end());

  return D.run();
}
