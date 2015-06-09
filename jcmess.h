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


/*
 * jcmess.h
 */

#ifndef __JCMESS_H
#define __JCMESS_H

#include <iostream>
#include <string>
#include <errno.h>

#include <string>
#include <vector>

#include <jack/jack.h>

using namespace std;

const int Indent = 2;

//-------------------------------------------------------------------------------
/*! \brief Class to save and load all jack client connections.
 *
 * Saves an Text file with all the current jack connections. This same file can
 * be loaded to connect everything again. The Text file can also be edited with
 * any text editor, bash commands or otherwise.
 *
 * Has also an option to disconnect all the clients.
 */
//-------------------------------------------------------------------------------
class JcMess {

public: 
  JcMess();
  virtual ~JcMess();

  void disconnectAll();
  void writeOutput(string OutFile);
  void connectPorts(string InFile);
  void disconnectClient(string clientName);

private:
  void setConnectedPorts();
  int parseTextFile(string InFile);

  jack_client_t *mClient; //Class client
  jack_status_t mStatus; //Class client status

  //Vectors of Connected Ports and Ports to connects
  //This are a matrix (Nx2) of string like this:
  //OuputPort1 InputPort1
  // ...
  //OuputPortN InputPortN
  vector<vector<string> > mConnectedPorts;
  vector<vector<string> > mPortsToConnect;
};
#endif //__JCMESS_H
