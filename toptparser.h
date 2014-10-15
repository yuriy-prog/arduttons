#include <list>
#include <vector>
#include <string>
#include <iostream>

using namespace std;
//
typedef list<string> lstring;
typedef void (*argrunfunc)(lstring);
//
//
class TCommandLineParser
{
//
   typedef struct { 
            bool isset;
          string key;
      argrunfunc func; 
             int nvals; 
         lstring vals;
          string help; } Targ;
   typedef vector<Targ> Targs;

   Targs args;
   string usage;

public:
   TCommandLineParser(string aUsage) : usage(aUsage) {}
   ~TCommandLineParser() {}

   //
   void print();
   void printusage();
   //
   void registry(string key, argrunfunc rfunc, int nvals=0, string help="");
   bool parse(int argc, char ** argv);
   bool addkeys(lstring keys);
   void run(); //!!! обработка в порядке регистрации
};
