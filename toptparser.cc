#include "toptparser.h"

//
void TCommandLineParser::run()
{
//
   for (Targs::iterator a=args.begin(); a != args.end(); a++) if (a->isset) a->func(a->vals);
}

//
void TCommandLineParser::print()
{
//
   for( Targs::iterator k=args.begin(); k != args.end(); k++ ) if (k->isset)
      {
   //
      cout << k->key ;
      for ( lstring::iterator v=k->vals.begin(); v != k->vals.end(); v++ ) cout << " " << *v;
      cout << endl;
      }
}

//
void TCommandLineParser::printusage()
{
//
   cout << usage << endl;
   for (Targs::iterator i=args.begin(); i != args.end(); i++)
      {
   //
      cout << "\t" << i->key;
      for (int v = 0; v < i->nvals; v++) cout << " PARAM";
      cout << " " << i->help << endl;
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
   for (lstring::iterator k=keys.begin(); k != keys.end(); k++)
      {
   //
      argv[i] = new char[k->size()+1];
      memset(argv[i], 0, k->size()+1);
      strncpy(argv[i], k->c_str(), k->size()+1);
      i++;
      }
   bool result = parse(argc, argv);
   //
   for (unsigned i = 0; i < keys.size(); i++) delete[] argv[i];
   delete[] argv;
   //
   return result;
}
