//
//  splitter_ims.h
//  P2PSP
//
//  This code is distributed under the GNU General Public License (see
//  THE_GENERAL_GNU_PUBLIC_LICENSE.txt for extending this information).
//  Copyright (C) 2014, the P2PSP team.
//  http://www.p2psp.org
//
//  IMS: IP Multicast Set of rules.
//

#ifndef P2PSP_CORE_SPLITTER_IMS_H_
#define P2PSP_CORE_SPLITTER_IMS_H_

#include <stdio.h>
#include <string>
#include <tuple>
#include <sstream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/thread/thread.hpp>
#include <iostream>
#include "../util/trace.h"

namespace p2psp {

class SplitterIMS {
 private:
  const int kBufferSize;          // Buffer size in chunks
  const std::string kChannel;     // Default channel
  const int kChunkSize;           // Chunk size in bytes (larger than MTU)
  const int kHeaderSize;          // Chunks/header
  const int kPort;                // Listening port
  const std::string kSourceAddr;  // Streaming server's host
  const int kSourcePort;          // Streaming server's listening port
  const std::string kMCastAddr;   // All Systems on this subnet
  const int kTTL;                 // Time To Live of multicast packets

  /*
   An IMS splitter runs 2 threads. The main one serves the
   chunks to the team. The other controls peer arrivals. This
   variable is true while the player is receiving data.
   */
  bool alive_;

  // Number of the served chunk.
  int chunk_number_;

  // Service for I/O operations
  boost::asio::io_service io_service_;

  // Used to listen to the incomming peers.
  boost::asio::ip::tcp::socket peer_connection_socket_;

  // Acceptor used to listen for incoming connections.
  boost::asio::ip::tcp::acceptor acceptor_;

  // Used to listen the team messages.
  boost::asio::ip::udp::socket team_socket_;

  // Used to talk to the source
  boost::asio::ip::tcp::socket source_socket_;

  // The video header
  boost::asio::streambuf header_;

  // Some other useful definitions.
  std::tuple<std::string, int> source_;
  std::string GET_message_;
  std::string chunk_number_format_;
  boost::asio::ip::udp::endpoint mcast_channel_;

  int recvfrom_counter_;
  int sendto_counter_;
  int header_load_counter_;

  // Thread management
  void Run();

 public:
  SplitterIMS();
  ~SplitterIMS();
  void SendTheHeader(int peer_serve_socket);
  void SendTheBufferSize(int peer_serve_socket);
  void SendTheChunkSize(int peer_serve_socket);
  void SendTheMcastChannel(int peer_serve_socket);
  void SendTheHeaderSize(int peer_serve_socket);
  void SendConfiguration(boost::asio::ip::tcp::socket &sock);
  void HandleAPeerArrival(boost::asio::ip::tcp::socket &serve_socket);
  void HandleArrivals();
  void SetupPeerConnectionSocket();
  void SetupTeamSocket();
  void RequestTheVideoFromTheSource();
  void ConfigureSockets();
  void LoadTheVideoHeader();
  size_t ReceiveNextChunk(boost::asio::streambuf &chunk);
  void ReceiveChunk();  // TODO: Return chunk
  void SendChunk(boost::asio::streambuf &message,
                 boost::asio::ip::udp::endpoint destination);
  void ReceiveTheHeader();

  // Thread management
  void Start();
};
}

#endif  // defined P2PSP_CORE_SPLITTER_IMS_H_