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
 * jcmess.cpp
 */

#include "jcmess.h"

#include <stdlib.h>
#include <fstream>
#include <sstream> 
#include <sys/stat.h>

#define SEPARATOR " -#- "

//-------------------------------------------------------------------------------
/*! \brief Constructs a JcMess object that has a jack client.
 *
 */
//-------------------------------------------------------------------------------
JcMess::JcMess()
{
  //Open a client connection to the JACK server.  Starting a
  //new server only to list its ports seems pointless, so we
  //specify JackNoStartServer. 
  mClient = jack_client_open ("lsp", JackNoStartServer, &mStatus);
  if (mClient == NULL) {
    if (mStatus & JackServerFailed) {
      cerr << "JACK server not running" << endl;
    } else {
      cerr << "jack_client_open() failed, " 
	   << "status = 0x%2.0x\n" << mStatus << endl;
    }
    exit(1);
  }
}


//-------------------------------------------------------------------------------
/*! \brief Distructor closes the jcmess jack audio client.
 *
 */
//-------------------------------------------------------------------------------
JcMess::~JcMess()
{
  if (jack_client_close(mClient))
    cerr << "ERROR: Could not close the hidden jcmess jack client." << endl;
}


//-------------------------------------------------------------------------------
/*! \brief Write an XML file with the name specified at OutFile.
 *
 */
//-------------------------------------------------------------------------------


// Function: fileExists
/**
    Check if a file exists
@param[in] filename - the name of the file to check

@return    true if the file exists, else false

*/
bool fileExists(const std::string& filename)
{
    struct stat buf;
    if (stat(filename.c_str(), &buf) != -1)
    {
        return true;
    }
    return false;
}

void JcMess::writeOutput(string OutFile)
{
  stringstream ss;
  vector<string> connectionPair(2);

  this->getActiveConnectedPorts();

  for (vector<vector<string> >::iterator it = mConnectedPorts.begin(); it != mConnectedPorts.end(); ++it) {
    connectionPair = *it;
    //cout << "Output ===> " << connectionPair[0] << endl;
    //cout << "Input ===> " << connectionPair[1] << endl;
    //cout << connectionPair[0] << SEPARATOR << connectionPair[1] << endl;
    ss << connectionPair[0] << SEPARATOR << connectionPair[1] << endl;

  }

  ofstream file;
  if ( OutFile != "" )
  {
    file.clear(file.failbit);
    file.clear(file.badbit);

    //Write output file
    string answer = "";
    //Check for existing file first, and confirm before overwriting
    if (fileExists(OutFile)) {
        while ((answer != "yes") && (answer != "no")) {
        cout << "WARNING: The File " << OutFile
        << " exists. Do you want to overwrite it? (yes/no): ";
        cin >> answer;
        }
    }
    else {
        answer = "yes";
    }

    if (answer == "yes") {
        //file.open(OutFile.c_str(),fstream::out);
        file.open(OutFile.c_str());
        if (file.fail()) {
        cerr << "Cannot open file for writing: " << endl;
        exit(1);
        }

        file << ss.rdbuf();
        file.close();
        cout << OutFile << " written/saved to disk." << endl;
    }
  }
  else
  {
    cout << ss.str() ;
  }
}
  

//-------------------------------------------------------------------------------
/*! \brief Set list of ouput ports that have connections.
 *
 */
//-------------------------------------------------------------------------------
void JcMess::getActiveConnectedPorts()
{
  mConnectedPorts.clear();

  const char **ports, **connections; //vector of ports and connections
  vector<string> connectionPair(2); //helper variable

  //Get active output ports.
  ports = jack_get_ports (mClient, NULL, NULL, JackPortIsOutput);
  
  for (unsigned int out_i = 0; ports[out_i]; ++out_i) {
    if ((connections = jack_port_get_all_connections(mClient, jack_port_by_name(mClient, ports[out_i]))) != 0) {
      for (unsigned int in_i = 0; connections[in_i]; ++in_i) {
	connectionPair[0] = ports[out_i];
	//cout << "Output ===> " << connectionPair[0] << endl;
	connectionPair[1] = connections[in_i];
	//cout << "Input ===> " << connectionPair[1] << endl;
	mConnectedPorts.push_back(connectionPair);
      }
    }
  }

  free(ports);
}


//-------------------------------------------------------------------------------
/*! \brief Disconnect all the clients.
 *
 */
//-------------------------------------------------------------------------------
void JcMess::disconnectAll()
{
  vector<string> connectionPair(2);
  
  this->getActiveConnectedPorts();
  
  for (vector<vector<string> >::iterator it = mConnectedPorts.begin(); it != mConnectedPorts.end(); ++it) {
    connectionPair = *it;
    
    jackDisconnect(connectionPair[0], connectionPair[1], false);
  }

}


//-------------------------------------------------------------------------------
/*! \brief Parse the XML input file.
 *
 * Returns 0 on success, or 1 if the file has an incorrect format or cannot 
 * read the file.
 */
//-------------------------------------------------------------------------------
int JcMess::parseTextFile(string InFile)
{
  mPortsToConnect.clear();
  string errorStr;
  
  ifstream file;
  file.clear(file.failbit);
  file.clear(file.badbit);

  file.open(InFile.c_str());
  if (file.fail()) {
    cerr << "Cannot open file for reading: " << endl;
    return 1;
  }

  vector<string> connectionPair(2);
  const string delimiter = SEPARATOR ;

  //TODO add error checking in getline and string operations
  while (!file.eof()) {
    string line; 
    getline(file,line);

    if (line.length() > 0) {
      size_t pos = 0;
      pos = line.find(delimiter);
      connectionPair[0] = line.substr(0, pos);
      line.erase(0, pos + delimiter.length());
      connectionPair[1] = line ;
      mPortsToConnect.push_back(connectionPair);

      //cout << connectionPair[0] << endl;
      //cout << connectionPair[1] << endl;
    }
  } 

  file.close();
  //cout << "vec size = " << mPortsToConnect.size() << endl; 

  return 0;
  
}


//-------------------------------------------------------------------------------
/*! \brief Connect ports specified in input Text file InFile
 *
 */
//-------------------------------------------------------------------------------
void JcMess::connectPorts(string InFile, string clientName)
{
  vector<string> connectionPair(2);

  if ( !(this->parseTextFile(InFile)) ) {  
    for (vector<vector<string> >::iterator it = mPortsToConnect.begin(); it != mPortsToConnect.end(); ++it) {
      connectionPair = *it;

      if (clientName.compare("null") != 0) { //if diff than "null"
        size_t cl0pos = connectionPair[0].find(clientName);
        size_t cl1pos = connectionPair[1].find(clientName);
        if (( cl0pos != string::npos ) || ( cl1pos != string::npos )) {
          //cout << "CON: " <<  connectionPair[0] << " <TO> " <<  connectionPair[1] << endl;
          jackConnect(connectionPair[0], connectionPair[1]);
        }
      }
      else {
        //cout << "CONA: " <<  connectionPair[0] << " <TO> " <<  connectionPair[1] << endl;
        jackConnect(connectionPair[0], connectionPair[1]);
      }
    }
  }
}

void JcMess::jackConnect(string connectionFrom, string connectionTo)
{
  if (jack_connect(mClient, connectionFrom.c_str(), connectionTo.c_str())) {
    //Display a warining only if the error is not because the ports are already
    //connected, in case the program doesn't display anyting.
    if (EEXIST !=
        jack_connect(mClient, connectionFrom.c_str(), connectionTo.c_str())) {
      cerr << "WARNING! port| " << connectionFrom
       << " |and port| " << connectionTo
       << " |could not be connected.\n";
    }
  }
}

//-------------------------------------------------------------------------------
/*! \brief Disconnect client with name "clientName"
 *
 */
//-------------------------------------------------------------------------------
void JcMess::disconnectClient(string clientName)
{
    vector<string> connectionPair(2);

    this->getActiveConnectedPorts();

    for (vector<vector<string> >::iterator it = mConnectedPorts.begin(); it != mConnectedPorts.end(); ++it){
        connectionPair = *it;

        size_t cl0pos = connectionPair[0].find(clientName);
        size_t cl1pos = connectionPair[1].find(clientName);
        if (( cl0pos != string::npos ) || ( cl1pos != string::npos )) {
            //cout << " DISCON: " << connectionPair[0] << " <FROM> " << connectionPair[1] << endl;
            jackDisconnect(connectionPair[0], connectionPair[1], true);
        }
    }
}

void JcMess::jackDisconnect(string connectionFrom, string connectionTo, bool verbose)
{
  if (jack_disconnect(mClient, connectionFrom.c_str(), connectionTo.c_str()))
  {
    cerr << "WARNING! port| " << connectionFrom
     << " |and port| " << connectionTo
     << " |could not be connected.\n";
  }
  else
  {
    if (verbose) 
      cout << "DISCON: " << connectionFrom << " <FROM> " << connectionTo << endl;
  }
}

