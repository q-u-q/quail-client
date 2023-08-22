#ifndef SRC_MY_QUIC_EPOLL_CLIENT_FACTORY_H_
#define SRC_MY_QUIC_EPOLL_CLIENT_FACTORY_H_

#include <memory>

#include "quiche/quic/core/io/quic_event_loop.h"
#include "quiche/quic/tools/quic_client_factory.h"

namespace quic {

class MyQuicEpollClientFactory : public ClientFactoryInterface {
 public:
  MyQuicEpollClientFactory();

  std::unique_ptr<QuicSpdyClientBase> CreateClient(
      std::string host_for_handshake, std::string host_for_lookup,
      int address_family_for_lookup, uint16_t port,
      ParsedQuicVersionVector versions, const QuicConfig& config,
      std::unique_ptr<ProofVerifier> verifier,
      std::unique_ptr<SessionCache> session_cache) override;

  void HandleEventsForever();

 private:
  std::unique_ptr<QuicEventLoop> event_loop_;
};

}

#endif /* SRC_MY_QUIC_EPOLL_CLIENT_FACTORY_H_ */
