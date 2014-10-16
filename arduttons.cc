#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "toptparser.h"

using namespace std;

const int BUTTON1 = 1;
const int BUTTON2 = 2;
const int BUTTON3 = 4;
const int BUTTON4 = 8;
const int TSTMODE = 16;
const int DORESET = 32;
const int GETSTAT = 64;

//
void die(const char * msg)
{
//
   cerr << "error: " << msg << endl;
   exit(1);
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
   char cwd[MAXPATHLEN];
   if (getcwd(cwd, MAXPATHLEN) == NULL) die("getcwd");
   cout << "cwd=" << cwd << endl;

   //TODO: find exist daemon (exist file socket - bind())
   sockaddr_un addrs;
   int sock, conn;
   size_t addrls;

   if ((sock = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) die("socket");

   addrs.sun_family = AF_UNIX; //unix domain socket
   strcpy(addrs.sun_path, "/tmp/sockuttons");
   addrls = sizeof(addrs.sun_family) + strlen(addrs.sun_path);

   if (bind(sock, (struct sockaddr*) &addrs, addrls)) die("bind");

   if (listen(sock, 1)) die("listen");

   socklen_t addrlc;
   sockaddr addrc;
   conn = accept(sock, (struct sockaddr*) &addrc, &addrlc);
   if (conn < 0) die("accept");

   char buf; //buf[1024]
   int r = read(conn, &buf, sizeof(buf));
   if (r < 0) die("read");
   if (r != 1) die("read1");
   switch(buf)
      {
   //
      case TSTMODE: cout << "get TSTMODE" << endl; break;
      case DORESET: cout << "get DORESET" << endl; break;
      case GETSTAT: cout << "get GETSTAT" << endl; break;
      default:      cout << "get fail byte" << endl;
      }

   close(sock);
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

   //
   sockaddr_un addrc;
   int sock;
   socklen_t addrlc;

   if ((sock = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) die("socket");

   addrc.sun_family = AF_UNIX; //unix domain socket
   strcpy(addrc.sun_path, "/tmp/sockuttons");
   addrlc = sizeof(addrc.sun_family) + strlen(addrc.sun_path);

   if (connect(sock, (struct sockaddr*) &addrc, addrlc)) die("connect");

   char buf = DORESET;
   int w = write(sock, &buf, sizeof(buf));
   if (w < 0) die("write");
   if (w != 1) die("write1");
   close(sock);
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
   clp.registry("-stop",   stop,       0, "stop daemon");
   clp.registry("-reset",  reset,      0, "clear testing mode, reset to watchman mode");
   clp.registry("-test",   test,       0, "enter to testing mode");
   clp.registry("-status", status,     0, "get status");

   if ( clp.parse(argc-1, argv+1) ) clp.run();
   else clp.printusage();

   cout << "argv[0]=" << argv[0] << endl;
   return 0;
}
