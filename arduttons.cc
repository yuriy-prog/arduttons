#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <thread>
#include <string>

#include "toptparser.h"

using namespace std;

string shup("/up.sh");
string shdown("/down.sh");
string shtest("/test.sh");

const int BUTTON1 = 1;
const int BUTTON2 = 2;
const int BUTTON3 = 4;
const int BUTTON4 = 8;
const int TSTMODE = 16;
const int DORESET = 32;
const int GETSTAT = 64;

char cliin=0, cliout=0;

//
void die(const char * msg)
{
//
   perror(msg);
   exit(1);
}

//
void check_script(const char * s)
{
//
   struct stat st;
   if (stat(s, &st) < 0) die(s);
   if (!(st.st_mode & S_IFREG && st.st_mode & S_IXUSR)) die(s);
   return;
}

//
void get_command_from_cli(int sock)
{
//
   socklen_t addrlc;
   sockaddr_un addrc;
   addrlc = sizeof(sockaddr_un);
   memset(&addrc, 0, addrlc);

   int conn;

   while ((conn = accept(sock, (struct sockaddr*) &addrc, &addrlc)) >= 0)
      {
   //
      char buf; //buf[1024]
      int r = read(conn, &buf, sizeof(buf));
      if (r < 0) die("read");
      if (r != 1) die("read1");
      switch(buf)
         {
      //
         case TSTMODE: buf = DORESET; break;
         case DORESET: buf = GETSTAT; break;
         case GETSTAT: buf = TSTMODE; break;
         default:      buf = 0;
         }

      int w = write(conn, &buf, sizeof(buf));
      if (w < 0) die("write");
      if (w != 1) die("write1");

      close(conn);
      addrlc = sizeof(sockaddr_un);
      memset(&addrc, 0, addrlc);
      }

   //
   if (conn < 0) die("accept");
}

//
char get_status_from_daemon(char cmd)
{
//
   sockaddr_un addrc;
   int sock;
   socklen_t addrlc;

   if ((sock = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) die("socket");

   addrc.sun_family = AF_UNIX; //unix domain socket
   strcpy(addrc.sun_path, "/tmp/sockuttons");
   addrlc = sizeof(addrc.sun_family) + strlen(addrc.sun_path);

   //TODO: !!! do down.sh ???
   if (connect(sock, (struct sockaddr*) &addrc, addrlc)) die("connect");

   char buf = cmd;
   int w = write(sock, &buf, sizeof(buf));
   if (w < 0) die("write");
   if (w != 1) die("write1");

   int r = read(sock, &buf, sizeof(buf));
   if (r < 0) die("read");
   if (r != 1) die("read1");

   close(sock);
   return buf;
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

   sockaddr_un addrs;
   int sock;
   size_t addrls;

   if ((sock = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) die("socket");

   addrs.sun_family = AF_UNIX; //unix domain socket
   strcpy(addrs.sun_path, "/tmp/sockuttons");
   addrls = sizeof(addrs.sun_family) + strlen(addrs.sun_path);

   if (bind(sock, (struct sockaddr*) &addrs, addrls)) die("bind");
   if (listen(sock, 1)) die("listen");

   //TODO: connect to arduino
   //TODO: get one status (in thread)
   //TODO: up.sh if good, else down
   if (daemon(0,1) == -1) die("daemon");
   //TODO: get status circle


   //TODO: get command from command line circle
   thread t1(bind(get_command_from_cli,sock));
   t1.join();
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

   switch(get_status_from_daemon(DORESET))
      {
   //
      case TSTMODE: cout << "get TSTMODE" << endl; break;
      case DORESET: cout << "get DORESET" << endl; break;
      case GETSTAT: cout << "get GETSTAT" << endl; break;
      default:      cout << "get fail byte" << endl;
                    exit(1);
      }
}

//
void test(lstring params)
{
//
   cout << "enter to testing mode" << endl;
   switch(get_status_from_daemon(TSTMODE))
      {
   //
      case TSTMODE: cout << "get TSTMODE" << endl; break;
      case DORESET: cout << "get DORESET" << endl; break;
      case GETSTAT: cout << "get GETSTAT" << endl; break;
      default:      cout << "get fail byte" << endl;
                    exit(1);
      }
}

//
void status(lstring params)
{
//
   cout << "get status from daemon" << endl;
   switch(get_status_from_daemon(GETSTAT))
      {
   //
      case TSTMODE: cout << "get TSTMODE" << endl; break;
      case DORESET: cout << "get DORESET" << endl; break;
      case GETSTAT: cout << "get GETSTAT" << endl; break;
      default:      cout << "get fail byte" << endl;
                    exit(1);
      }
}

//
//
int main(int argc, char *argv[])
{
//
   //TODO: get scripts
   char cwd[MAXPATHLEN];
   if (getcwd(cwd, MAXPATHLEN) == NULL) die("getcwd");
   //cout << "current directory: " << cwd << endl;

   shup = string(cwd) + shup;
   shdown = string(cwd) + shdown;
   shtest = string(cwd) + shtest;

   cout << "check scripts:" << endl;
   cout << shup;   check_script(shup.c_str());   cout << " OK" << endl;
   cout << shdown; check_script(shdown.c_str()); cout << " OK" << endl;
   cout << shtest; check_script(shtest.c_str()); cout << " OK" << endl;

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

   return 0;
}
