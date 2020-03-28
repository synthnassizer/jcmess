
/*
  jcmess: A simple utility so save your jack-audio mess. Fully written in c++.

          Based on Juan-Pablo Caceres JcMess , but without Qt hooks.

          Also jcmess does not save to an XML file but to a simple text file as 
                client:port -#- client:port 
          so the separator is " -#- "

  Copyright (C) 2014-2015 Athanasios Silis.

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation
  files (the "Software"), to deal in the Software without
  restriction, including without limitation the rights to use,
  copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following
  conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  OTHER DEALINGS IN THE SOFTWARE.
*/


#include <iostream>
#include <cstdlib>
#include <string>
#include <getopt.h> // for command line parsing
#include <stdio.h>

#include "jcmess.h"
//#include "anyoption.h"

using namespace std;

void main_dialog( int argc, char* argv[] );
void printUsage();
string version  = "0.2";


//*******************************************************************************
int main(int argc, char** argv)
{
  main_dialog( argc, argv ); 
  return 0;
}


//*******************************************************************************
void main_dialog( int argc, char* argv[] )
{
  
  //Create JcMess Object for the following flags
  JcMess jcmessClient;

  // If no command arguments are given, print instructions
  if(argc == 1) {
    //printUsage();
    jcmessClient.writeOutput( "" );
    std::exit(0);
  }

  // Usage example at:
  // http://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html#Getopt-Long-Option-Example
  // options descriptor
  //----------------------------------------------------------------------------
  static struct option longopts[] = {
    // These options don't set a flag.
    { "disconnectall", no_argument, NULL,  'd' },
    { "unmount", required_argument, NULL, 'u' },
    { "mount", required_argument, NULL, 'm' },
    { "load", required_argument, NULL, 'l' },
    { "save", required_argument, NULL,  's' },
    { "DisconnectAll", no_argument, NULL,  'D' },
    { "version", no_argument, NULL, 'v' }, // Version Number
    { "print", no_argument, NULL,  'p' },  // print to stdout
    { "verbose", no_argument, NULL, 'V' }, // print to stdout
    { "help", no_argument, NULL, 'h' },    // Print Help
    { NULL, 0, NULL, 0 }
  };


  // Parse Command Line Arguments
  //----------------------------------------------------------------------------
  /// \todo Specify mandatory arguments
  string answer = "";
  int ch;
  ///flag followed by "::" -> optional to place flag in CLI
  ///flag followed by ":" -> required to place flag in CLI
  ///flag followed by ""(=nothing) -> no argument
  while ( (ch = getopt_long(argc, argv,
                            "du:m:l:s:pDvVh", longopts, NULL)) != -1 )
    switch (ch) {
    //-------------------------------------------------------
    case 'd':
    //Confirm before disconnection
    while ((answer != "yes") && (answer != "no")) {
      cout << "Are you sure you want to disconnect all? (yes/no): ";
      cin >> answer;
    }
    if (answer == "yes") {
      jcmessClient.disconnectAll();
    }
    break;
    //-------------------------------------------------------
    case 'l':
    jcmessClient.connectPorts( optarg , "null" );
    break;
    //-------------------------------------------------------
    case 's':
    jcmessClient.writeOutput( optarg );
    break;
    //-------------------------------------------------------
    case 'p':
    case 'V':
    jcmessClient.writeOutput( "" );
    break;
    //-------------------------------------------------------
    case 'D':
    jcmessClient.disconnectAll();
    break;
    //-------------------------------------------------------
    case 'u':
    jcmessClient.disconnectClient( optarg );
    break;
    //-------------------------------------------------------
    case 'm':
    //see NOT the selected answer in http://stackoverflow.com/questions/3939157/c-getopt-multiple-value 
    if (( optind < argc ) && (*argv[optind] != '-')) //optind already points to next arg than the one we current parse
    {
        cout << "client name = " << optarg << " , inputfilename = " << argv[optind] << endl;
        jcmessClient.connectPorts( string(argv[optind]) , optarg );
        optind++; //advance to next arg
    }
    else
        cout << " command reguires 2 args:  -m <clientname> <inputfile>" << endl;
    break;
    //-------------------------------------------------------
    case 'v':
    cout << "JcMess VERSION: " << version << endl;
    cout << "Copyright (c) 2014-2015 Athanasios Silis." << endl;
    cout << "Copyright (c) 2007-2010 Juan-Pablo Caceres." << endl;
    cout << "SoundWIRE group at CCRMA, Stanford University" << endl;
    cout << "" << endl;
    std::exit(0);
    break;
    case 'h':
    //-------------------------------------------------------
    printUsage();
    break;
  }

  // Warn user if undefined options where entered
  //----------------------------------------------------------------------------
  if (optind < argc) {
    cout << "------------------------------------------------------" << endl;
    cout << "WARNING:  The following entered options have no effect" << endl;
    cout << "          They will be ignored!" << endl;
    cout << "          Type jcmess to see options." << endl;
    for( ; optind < argc; optind++) {
      printf("argument: %s\n", argv[optind]);
    }
    cout << "------------------------------------------------------" << endl;
  }
}


//*******************************************************************************
void printUsage()
{
  cout << "" << endl;
  cout << "JcMess: A simple utility so save your jack-audio mess." << endl;
  cout << "Copyright (c) 2014-2015 Athanasios Silis." << endl;
  cout << "Copyright (C) 2007-2010 Juan-Pablo Caceres." << endl;
  cout << "VERSION: " << version << endl;
  cout << "" << endl;
  cout << "Usage: " << endl;
  cout << "--------------------------------------------" << endl;
  cout << " -h  --help                    Prints this help" << endl;
  cout << " -v  --version                 Prints version information" << endl;
  cout << " -l  --load inputfile          Load the connections specified at inputfile" << endl;
  cout << " -s  --save outputfile         Save current connections in output" << endl;
  cout << " -u  --umount \"client name\"  Disconnect all connections to a specific jack client" << endl;
  cout << " -m  --mount \"client name\"   inputfile   Connect all referrences to jack client, found in inputfile" << endl;
  cout << " -d  --disconnectall           Disconnect all the connections" << endl;
  cout << " -D  --DisconnectAll           Disconnect all the connections without confirmation" << endl;
  cout << " -p  --print                   Same as --verbose" << endl;
  cout << " -V  --verbose                 Same as --save but dumps the connections to stdout" << endl;
  cout << "" << endl;
}
