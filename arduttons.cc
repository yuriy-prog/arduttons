#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/param.h>

#include "toptparser.h"

using namespace std;

//
void printusage(lstring params)
{
//
   cout << "use ./arduttons without params for help" << endl;
}

//
void daemon(lstring params)
{
//
   cout << "enter to daemon" << endl;
   char cwd[MAXPATHLEN];
   if (getcwd(cwd, MAXPATHLEN) == NULL)
      {
   //
      cerr << "error: getcwd" << endl;
      exit(1);
      }
   cout << "cwd=" << cwd << endl;
}

//
void stop(lstring params)
{
//
   cout << "stop daemon" << endl;
}

//
void reset(lstring params)
{
//
   cout << "drop testing mode, reset to watchman mode" << endl;
}

//
void test(lstring params)
{
//
   cout << "enter to testing mode" << endl;
}

//
void status(lstring params)
{
//
   cout << "get status from daemon" << endl;
}


//
int main(int argc, char *argv[])
{
//
   TCommandLineParser clp("use: arduttons.exe [-h] [...]\n     (keys case sensitive, NO RECURSE, be accurate)");
   clp.registry("-h",      printusage, 0, "print help");
   clp.registry("-daemon", daemon,     0, "enter to daemon mode (drop other opts)");
   clp.registry("-stop",   daemon,     0, "stop daemon");
   clp.registry("-reset",  reset,      0, "clear testing mode, reset to watchman mode");
   clp.registry("-test",   test,       0, "enter to testing mode");
   clp.registry("-status", status,     0, "get status");

   if ( clp.parse(argc-1, argv+1) ) clp.run();
   else clp.printusage();

   cout << "argv[0]=" << argv[0] << endl;
   return 0;
}
