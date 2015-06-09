#jcmess#

##About##

A simple utility so save your jack-audio mess. Fully written in c++.
Based on Juan-Pablo Caceres "JMess" utility

https://github.com/jcacerec/jmess-jack

https://ccrma.stanford.edu/groups/soundwire/software/jmess/

JcMess is similar to jmess, but without Qt hooks (or requirement for qmake).

jcmess does not save to an XML file but rather to a simple text file as
        `client:port -#- client:port`
Connections appear as *one liners* so the file can easily be edited 
with bash commands (such as grep, sed, awk)

In the latest version it can also disconnect any clients that go by a certain name.

`jcmess -u <client name>`

This is useful when netjack clients connect (from) remotely and any automatic connections that are established (e.g. by persistent connections of qjackctl) need to be disconnected before a setup is to be loaded.

The rest of the functionality follows that of JMess

`jcmess -s filename` saves to a file

`jcmess -l filename` loads from a file

`jcmess -d` disconnect all current connections (with user input)

`jcmess -D` disconnect all current connections (without user input)

