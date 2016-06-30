//
//  peer_core.h - P2PSP's core stuff
//
//  This code is distributed under the GNU General Public License (see
//  THE_GENERAL_GNU_PUBLIC_LICENSE.txt for extending this information).
//
//  Copyright (C) 2016, the P2PSP team.
//  http://www.p2psp.org
//
//

#ifndef P2PSP_CORE_PEER_CORE_H
#define P2PSP_CORE_PEER_CORE_H

#include <arpa/inet.h>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <ctime>
#include <fstream>
#include <string>
#include <tuple>
#include <vector>
#include "../util/trace.h"
#include "common.h"

using namespace boost::asio;

namespace p2psp {

  struct Chunk {
    std::vector<char> data;
    bool received;
  };

  class Peer_core {

  protected:

    static constexpr char kSplitterAddr[] = "127.0.0.1";  // Default address of the splitter.
    static const uint16_t kSplitterPort = 4552;           // Default port of the splitter.
    static const uint16_t kTeamPort = 0;                  // Default TCP->UDP port used to communicate.
    static const bool kUseLocalhost = false;              // Default use localhost instead the IP of the addapter
    static const int kBufferStatus = 0;                   // Default ?
    static const bool kShowBuffer = false;                // Default
    static const int kChunkIndexSize = 2;

    char magic_flags_;
    uint16_t player_port_;                                // Port used to serve the player.
    ip::address splitter_addr_;                           // Address of the splitter.
    uint16_t splitter_port_;                              // Port of the splitter.
    uint16_t team_port_;                                  // TCP->UDP port used to communicate.
    bool use_localhost_;                                  // Use localhost instead the IP of the addapter
    int buffer_status_;                                   // ?
    int sendto_counter_;                                  // Initialized to -1 in clases that don't use it
    bool show_buffer_;
    int buffer_size_;
    unsigned int message_size_;
    int chunk_size_;
    std::vector<Chunk> chunks_;
    int header_size_in_chunks_;
    ip::address mcast_addr_;
    uint16_t mcast_port_;
    int played_chunk_;
    bool player_alive_;
    int received_counter_;
    std::vector<bool> received_flag_;
    int recvfrom_counter_;
    ip::udp::endpoint splitter_;
    io_service io_service_;                               // Service for I/O operations
    ip::tcp::acceptor acceptor_;                          // Acceptor used to listen to incoming connections.
    ip::tcp::socket player_socket_;                       // Used to listen to the player
    ip::tcp::socket splitter_socket_;                     // Used to listen to the splitter
    ip::udp::socket team_socket_;                         // Used to communicate with the rest of the team
    boost::thread_group thread_group_;                    // Thread group to join all threads
    std::vector<ip::udp::endpoint> peer_list_;            // DBS variables
    int previous_chunk_number_=0;
    int latest_chunk_number_ = 0;
    bool kLogging = false;
    std::string kLogFile;
    bool logging_;
    std::ofstream log_file_;
    ip::udp::endpoint me_;
    int number_of_peers_;

  public:

    PeerIMS();
    ~PeerIMS();

    /**
     *  This function must be called after constructing a new object.
     */
    virtual void Init();

    /**
     *  Setup "player_socket" and wait for the player
     */
    virtual void WaitForThePlayer();

    /**
     *  Setup "splitter" and "splitter_socket"
     */
    virtual void ConnectToTheSplitter() throw(boost::system::system_error);
    virtual void DisconnectFromTheSplitter();
    virtual void ReceiveTheMcastChannel();
    virtual void ReceiveTheHeader();
    virtual void ReceiveTheChunkSize();
    virtual void ReceiveTheHeaderSize();
    virtual void ReceiveTheBufferSize();
    void ReceiveMagicFlags();
    virtual void ReceiveMyEndpoint();
    //virtual void ReceiveTheNumberOfPeers();

    /**
     *  Create "team_socket" (UDP) for using the multicast channel
     */
    virtual void ListenToTheTeam();
    virtual void ReceiveTheNextMessage(std::vector<char>&, ip::udp::endpoint&);
    virtual int ProcessMessage(const std::vector<char>&,
                               const ip::udp::endpoint&);
    virtual int ProcessNextMessage();

    /**
     *  Buffering
     */
    virtual void BufferData();
    virtual int FindNextChunk();
    virtual void PlayChunk(int);
    virtual void PlayNextChunk(int chunk_number);
    virtual void KeepTheBufferFull();

    /**
     *  Thread management
     */
    virtual void Run();
    virtual void Start();

    /**
     * Log Messages
     */
    virtual void LogMessage(const std::string&);
    virtual std::string BuildLogMessage(const std::string&);

    /**
     *  Getters/setters
     */
    virtual char GetMagicFlags();
    //virtual std::string GetMcastAddr();
    virtual ip::address GetMcastAddr();
    virtual bool IsPlayerAlive();
    virtual int GetPlayedChunk();
    virtual int GetChunkSize();
    virtual std::vector<ip::udp::endpoint>* GetPeerList();
    virtual int GetRecvfromCounter();
    virtual void SetShowBuffer(bool);
    virtual void SetSendtoCounter(int);
    virtual int  GetSendtoCounter();
    virtual void     SetPlayerPort(uint16_t);
    virtual uint16_t GetPlayerPort();
    //virtual void       SetSplitterAddr(std::string);
    virtual void        SetSplitterAddr(ip::address splitter_addr);
    virtual ip::address GetSplitterAddr();
    virtual void     SetSplitterPort(uint16_t);
    virtual uint16_t GetSplitterPort();
    virtual void     SetTeamPort(uint16_t);
    virtual uint16_t GetTeamPort();
    virtual void SetUseLocalHost(bool);
    bool GetUseLocalHost();
    virtual int GetHeaderSize();
    virtual int GetBufferSize();
    int GetNumberOfPeers() { return 0; }
    // bool AmIAMonitor() { return false; }
    //void ReceiveTheListOfPeers() {}
    
    static uint16_t GetDefaultPlayerPort();
    static uint16_t GetDefaultTeamPort();
    static ip::address GetDefaultSplitterAddr();
    static uint16_t GetDefaultSplitterPort();

  };
}

#endif  // P2PSP_CORE_PEER_CORE_H