#ifndef SRC_QUIC_CLIENT_NETWORK_HELPER_H_
#define SRC_QUIC_CLIENT_NETWORK_HELPER_H_

#include "quiche/quic/tools/quic_client_default_network_helper.h"

#include <cstdint>
#include <memory>
#include <string>

#include "absl/types/optional.h"
#include "quiche/common/quiche_linked_hash_map.h"
#include "quiche/quic/core/io/quic_event_loop.h"
#include "quiche/quic/core/quic_default_packet_writer.h"
#include "quiche/quic/core/quic_packet_reader.h"
#include "quiche/quic/core/quic_udp_socket.h"
#include "quiche/quic/tools/quic_client_base.h"

namespace quic {

// An implementation of the QuicClientBase::NetworkHelper interface that is
// based on the QuicEventLoop API.
class QuicClientNetworkHelper : public QuicClientBase::NetworkHelper,
                                public QuicSocketEventListener,
                                public ProcessPacketInterface {
 public:
  QuicClientNetworkHelper(QuicEventLoop* event_loop);
  QuicClientNetworkHelper(const QuicClientNetworkHelper&) = delete;
  QuicClientNetworkHelper& operator=(const QuicClientNetworkHelper&) = delete;

  ~QuicClientNetworkHelper() override;

  // From QuicSocketEventListener.
  void OnSocketEvent(QuicEventLoop* event_loop,
                     QuicUdpSocketFd fd,
                     QuicSocketEventMask events) override;

  // From ProcessPacketInterface. This will be called for each received
  // packet.
  void ProcessPacket(const QuicSocketAddress& self_address,
                     const QuicSocketAddress& peer_address,
                     const QuicReceivedPacket& packet) override;

  // From NetworkHelper.
  void RunEventLoop() override;
  bool CreateUDPSocketAndBind(QuicSocketAddress server_address,
                              QuicIpAddress bind_to_address,
                              int bind_to_port) override;
  void CleanUpAllUDPSockets() override;
  QuicSocketAddress GetLatestClientAddress() const override;
  QuicPacketWriter* CreateQuicPacketWriter() override;

  // Accessors provided for convenience, not part of any interface.
  QuicEventLoop* event_loop() { return event_loop_; }
  const quiche::QuicheLinkedHashMap<SocketFd, QuicSocketAddress>&
  fd_address_map() const {
    return fd_address_map_;
  }

  // If the client has at least one UDP socket, return the latest created one.
  // Otherwise, return -1.
  SocketFd GetLatestFD() const;

  // Create a socket for connection to |server_address| with default socket
  // options. Returns the FD of the resulting socket.
  virtual SocketFd CreateUDPSocket(QuicSocketAddress server_address,
                                   bool* overflow_supported);

  void SetClient(QuicClientBase* client){client_ = client;}
  QuicClientBase* client() { return client_; }

  void set_max_reads_per_event_loop(int num_reads) {
    max_reads_per_event_loop_ = num_reads;
  }
  // If |fd| is an open UDP socket, unregister and close it. Otherwise, do
  // nothing.
  void CleanUpUDPSocket(SocketFd fd);

  // Used for testing.
  void SetClientPort(int port);

  // Indicates that some of the FDs owned by the network helper may be
  // unregistered by the external code by manually calling
  // event_loop()->UnregisterSocket() (this is useful for certain scenarios
  // where an external event loop is used).
  void AllowFdsToBeUnregisteredExternally() {
    fds_unregistered_externally_ = true;
  }

  // Bind a socket to a specific network interface.
  bool BindInterfaceNameIfNeeded(SocketFd fd);

  // Actually clean up |fd|.
  virtual void CleanUpUDPSocketImpl(SocketFd fd);

 private:
  // Listens for events on the client socket.
  QuicEventLoop* event_loop_;

  // Map mapping created UDP sockets to their addresses. By using linked hash
  // map, the order of socket creation can be recorded.
  quiche::QuicheLinkedHashMap<SocketFd, QuicSocketAddress> fd_address_map_;

  // If overflow_supported_ is true, this will be the number of packets dropped
  // during the lifetime of the server.
  QuicPacketCount packets_dropped_;

  // True if the kernel supports SO_RXQ_OVFL, the number of packets dropped
  // because the socket would otherwise overflow.
  bool overflow_supported_;

  // Point to a QuicPacketReader object on the heap. The reader allocates more
  // space than allowed on the stack.
  std::unique_ptr<QuicPacketReader> packet_reader_;

  QuicClientBase* client_;

  int max_reads_per_event_loop_;

  // If true, some of the FDs owned by the network helper may be unregistered by
  // the external code.
  bool fds_unregistered_externally_ = false;
};

}  // namespace quic

#endif /* SRC_QUIC_CLIENT_NETWORK_HELPER_H_ */
