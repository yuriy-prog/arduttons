#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <mutex>
#include <string>

#include "toptparser.h"

using namespace std;

mutex m;

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

const char * ud_sock  = "/tmp/sockuttons";
const char * a_ipaddr = "192.168.12.80";
const unsigned short a_port = 2323;

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
void run_script(const char * s)
{
//
   char* argv[2];
   argv[1] = nullptr;
   argv[0] = new char[strlen(s)];
   strcpy(argv[0], s);
   execve(s, argv, nullptr);
   //delete[] argv[0];
}

//
void print_cmd(char cmd)
{
//
   cout << "cmd=" << int(cmd) << endl;
   if (cmd & TSTMODE) cout << "TEST MODE is active." << endl;
   if (cmd & DORESET) cout << "DORESET command." << endl;
   if (cmd & GETSTAT) cout << "GETSTAT command." << endl;
   if (cmd & BUTTON1) cout << "BUTTON 1 is down." << endl;
   if (cmd & BUTTON2) cout << "BUTTON 2 is down." << endl;
   if (cmd & BUTTON3) cout << "BUTTON 3 is down." << endl;
   if (cmd & BUTTON4) cout << "BUTTON 4 is down." << endl;
}

//
char get_status_from_a(char cmd)
{
//
   int sock;
   struct sockaddr_in addr;

   if ((sock=socket(AF_INET, SOCK_STREAM, 0)) < 0) die("socket");

   memset(&addr, 0, sizeof(addr));
   addr.sin_family = AF_INET;
   inet_pton(AF_INET, a_ipaddr, &addr.sin_addr);
   addr.sin_port = htons(a_port);

   if (connect(sock, (struct sockaddr*)&addr, sizeof(addr))) die("connect");

   write(sock, &cmd, 1);
   read(sock, &cmd, 1);

   close(sock);
   return cmd;
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

      m.lock();
      buf = get_status_from_a(buf);
      m.unlock();

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
   strcpy(addrc.sun_path, ud_sock);
   addrlc = sizeof(addrc.sun_family) + strlen(addrc.sun_path) + 1;

   if (connect(sock, (struct sockaddr*) &addrc, addrlc)) run_script(shdown.c_str());//die("connect");

   char buf = cmd;
   int w = write(sock, &buf, sizeof(buf));
   if (w <= 0) die("write");

   int r = read(sock, &buf, sizeof(buf));
   if (r <= 0) die("read");

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
   strcpy(addrs.sun_path, ud_sock);
   addrls = sizeof(addrs.sun_family) + strlen(addrs.sun_path) + 1;

   if (bind(sock, (struct sockaddr*) &addrs, addrls)) die("bind");
   if (listen(sock, 1)) die("listen");

   //TODO: connect to arduino
   //TODO: get one status (in thread)
   m.lock();
   char res = get_status_from_a(GETSTAT);
   m.unlock();
   //TODO: up.sh if good, else down
   if (res&TSTMODE) system(shup.c_str());
   else
      if (res&(BUTTON1|BUTTON2|BUTTON3|BUTTON4)) run_script(shdown.c_str());
      else system(shup.c_str());

   if (daemon(0,0) == -1) die("daemon");

   //TODO: get command from command line circle
   thread t1(bind(get_command_from_cli,sock));

   //TODO: get status circle
   bool reportTSTMODE = false;
   while (true)
      {
   //
      m.lock();
      char res = get_status_from_a(GETSTAT);
      m.unlock();

      if (!(res&TSTMODE)) reportTSTMODE = false;
      if (res&(BUTTON1|BUTTON2|BUTTON3|BUTTON4))
         {
      //
         if (res&TSTMODE)
            {
         //
            if (!reportTSTMODE)
               {
            //
               reportTSTMODE = true;
               system(shtest.c_str());
               }
            }
         else
            {
         //
            run_script(shdown.c_str());
            //exit(1);
            }
         }

      //
      usleep(1000000);
      }
}

//
void reset(lstring params)
{
//
   cout << "drop testing mode, reset to watchman mode" << endl;
   print_cmd(get_status_from_a(DORESET|GETSTAT));
}

//
void test(lstring params)
{
//
   cout << "enter to testing mode" << endl;
   print_cmd(get_status_from_daemon(TSTMODE|GETSTAT));
}

//
void status(lstring params)
{
//
   cout << "get status from daemon" << endl;
   char stat = get_status_from_daemon(GETSTAT);
   print_cmd(stat);
   //if (stat&TSTMODE && stat&(BUTTON1|BUTTON2|BUTTON3|BUTTON4)) run_script(shtest.c_str());
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
   clp.registry("-reset",  reset,      0, "clear testing mode, reset to watchman mode");
   clp.registry("-test",   test,       0, "enter to testing mode");
   clp.registry("-status", status,     0, "get status");

   if ( clp.parse(argc-1, argv+1) ) clp.run();
   else clp.printusage();

   return 0;
}
