#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>
#include <list>
#include <string>
#include <strstream>
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

//
void TCommandLineParser::run()
{
//
   for (auto a : args) if (a.isset) a.func(a.vals);
}

//
void TCommandLineParser::print()
{
//
   for( auto k : args ) if (k.isset)
      {
   //
      cout << k.key ;
      for ( auto v : k.vals ) cout << " " << v;
      cout << endl;
      }
}

//
void TCommandLineParser::printusage()
{
//
   cout << usage << endl;
   for (auto i : args) 
      {
   //
      cout << "\t" << i.key;
      for (int v = 0; v < i.nvals; v++) cout << " PARAM";
      cout << " " << i.help << endl;
      }
}

//
void TCommandLineParser::registry(string key, argrunfunc rfunc, int nvals, string help)
{
//
   Targ p;
   p.isset = false;
   p.key   = key;
   p.nvals = nvals;
   p.vals  = lstring();
   p.help  = help;
   p.func  = rfunc;

   args.push_back(p);
}

//
bool TCommandLineParser::parse(int argc, char ** argv)
{
//
   int a = 0;
   bool stop = false;
   string currkey = "";
   int currvals = 0;

   while ( a < argc )
      {
   //
      stop = false;
      if ( currvals == 0 )
         {
      // get key name
         currkey = argv[a];
         stop = true;
         int f = args.size();
         for ( int i = 0; i < f; i++ ) if (args[i].key == currkey) { f = i; break; }
         if ( f != args.size() )
            {
         //
            args[f].isset = true;
            currvals = args[f].nvals;
            if (currvals != 0) stop = false;
            }
         else
            {
         //
            return false;
            }
         }
      else
         {
      // get key param
         int f = args.size();
         for ( int i = 0; i < f; i++ ) if (args[i].key == currkey) { f = i; break; }
         args[f].vals.push_back(argv[a]);
         currvals--;
         if (currvals == 0) stop = true;
         }
      a++;
      }
   //
   return stop;
}

//
bool TCommandLineParser::addkeys(lstring keys)
{
//
   int argc = keys.size();
   char ** argv = new char*[argc];
   int i = 0;
   for (auto k : keys)
      {
   //
      argv[i] = new char[k.size()+1];
      memset(argv[i], 0, k.size()+1);
      strncpy(argv[i], k.c_str(), k.size()+1);
      i++;
      }
   bool result = parse(argc, argv);
   //
   for (unsigned i = 0; i < keys.size(); i++) delete[] argv[i];
   delete[] argv;
   //
   return result;
}

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
}

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
   clp.registry("-reset",  reset,      0, "clear testing mode, reset to watchman mode");
   clp.registry("-test",   test,       0, "enter to testing mode");
   clp.registry("-status", status,     0, "get status");

   if ( clp.parse(argc-1, argv+1) ) clp.run();
   else clp.printusage();

   return 0;
}
