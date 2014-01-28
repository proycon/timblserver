/*
  $Id$
  $URL$

  Copyright (c) 1998 - 2013
  ILK   - Tilburg University
  CLiPS - University of Antwerp
 
  This file is part of timblserver

  timblserver is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  timblserver is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.

  For questions and suggestions, see:
      http://ilk.uvt.nl/software.html
  or send mail to:
      timbl@uvt.nl
*/

#ifndef SERVERBASE_H
#define SERVERBASE_H

#include "ticcutils/LogStream.h"
#include "ticcutils/Configuration.h"
#include "timblserver/SocketBasics.h"

namespace TimblServer {

  class childArgs;

  class ServerBase {
  public:
    bool doDebug() { return debug; };
    virtual ~ServerBase(){};
    static std::string VersionInfo( bool );
    static int daemonize( int , int );
    int maxConn() const { return _maxConn; };
    ServerBase( const TiCC::Configuration * );
    void setDebug( bool d ){ debug = d; };
    Sockets::ServerSocket *TcpSocket() const { return tcp_socket; };
    static void *callChild( void * ); 
    int Run();
    virtual void socketChild( childArgs * );
    virtual void callback( childArgs* ) = 0;
    TiCC::LogStream myLog;
    std::string logFile;
    std::string pidFile;
    std::string name;
    bool doDaemon;
    bool debug;
    int _maxConn;
    int serverPort;
    void *callback_data;
    Sockets::ServerSocket *tcp_socket;
    std::string serverProtocol;
    std::string serverConfigFile;
    const TiCC::Configuration *config;
    std::map<std::string, std::string> serverConfig;
  };

  class childArgs {
  public:
    childArgs( ServerBase *, Sockets::ServerSocket * );
    ~childArgs();
    int id() const { return _id; };
    fdostream& os() { return _os; };
    fdistream& is() { return _is; };
    ServerBase *mother() const { return _mother; };
    TiCC::LogStream& logstream() { return _mother->myLog; }
    Sockets::ServerSocket *socket() const { return _socket; };
    bool debug() const { return _mother->doDebug(); };
  private:
    ServerBase *_mother;
    Sockets::ServerSocket *_socket;
    int _id;
    fdistream _is;
    fdostream _os;
  };
  
  void *ServerBase::callChild( void *a ) { 
    childArgs* ca = (childArgs*)a;
    ca->mother()->socketChild( ca );
    return 0;
  }
  
  class TcpServerBase : public ServerBase {
  public:
  TcpServerBase( const TiCC::Configuration *c ):ServerBase( c ){};
  };
  
  class HttpServerBase : public ServerBase {
  public:
    void socketChild( childArgs * );
  HttpServerBase( const TiCC::Configuration *c ): ServerBase( c ){};
  };

  std::string Version();
  std::string VersionName();

  int daemonize( int noCD , int noClose ){
    return ServerBase::daemonize( noCD, noClose);
  }

}
#endif // SERVERBASE_H
