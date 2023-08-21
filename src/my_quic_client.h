#ifndef SRC_MY_QUIC_CLIENT_H_
#define SRC_MY_QUIC_CLIENT_H_

#include <cstdint>
#include <memory>
#include <string>

#include "quiche/quic/core/io/quic_event_loop.h"
#include "quiche/quic/core/quic_config.h"
#include "quiche/quic/tools/quic_client_default_network_helper.h"
#include "quiche/quic/tools/quic_spdy_client_base.h"

namespace quic {

class QuicServerId;

class MyQuicClient : public QuicSpdyClientBase {
 public:
  // These will create their own QuicClientDefaultNetworkHelper.
  MyQuicClient(QuicSocketAddress server_address,
                    const QuicServerId& server_id,
                    const ParsedQuicVersionVector& supported_versions,
                    QuicEventLoop* event_loop,
                    std::unique_ptr<ProofVerifier> proof_verifier);
  MyQuicClient(QuicSocketAddress server_address,
                    const QuicServerId& server_id,
                    const ParsedQuicVersionVector& supported_versions,
                    QuicEventLoop* event_loop,
                    std::unique_ptr<ProofVerifier> proof_verifier,
                    std::unique_ptr<SessionCache> session_cache);
  MyQuicClient(QuicSocketAddress server_address,
                    const QuicServerId& server_id,
                    const ParsedQuicVersionVector& supported_versions,
                    const QuicConfig& config, QuicEventLoop* event_loop,
                    std::unique_ptr<ProofVerifier> proof_verifier,
                    std::unique_ptr<SessionCache> session_cache);
  // This will take ownership of a passed in network primitive.
  MyQuicClient(
      QuicSocketAddress server_address, const QuicServerId& server_id,
      const ParsedQuicVersionVector& supported_versions,
      QuicEventLoop* event_loop,
      std::unique_ptr<QuicClientDefaultNetworkHelper> network_helper,
      std::unique_ptr<ProofVerifier> proof_verifier);
  MyQuicClient(
      QuicSocketAddress server_address, const QuicServerId& server_id,
      const ParsedQuicVersionVector& supported_versions,
      const QuicConfig& config, QuicEventLoop* event_loop,
      std::unique_ptr<QuicClientDefaultNetworkHelper> network_helper,
      std::unique_ptr<ProofVerifier> proof_verifier);
  MyQuicClient(
      QuicSocketAddress server_address, const QuicServerId& server_id,
      const ParsedQuicVersionVector& supported_versions,
      const QuicConfig& config, QuicEventLoop* event_loop,
      std::unique_ptr<QuicClientDefaultNetworkHelper> network_helper,
      std::unique_ptr<ProofVerifier> proof_verifier,
      std::unique_ptr<SessionCache> session_cache);
  MyQuicClient(const MyQuicClient&) = delete;
  MyQuicClient& operator=(const MyQuicClient&) = delete;

  ~MyQuicClient() override;

  // QuicSpdyClientBase overrides.
  std::unique_ptr<QuicSession> CreateQuicClientSession(
      const ParsedQuicVersionVector& supported_versions,
      QuicConnection* connection) override;

  // Exposed for QUIC tests.
  int GetLatestFD() const { return default_network_helper()->GetLatestFD(); }

  QuicClientDefaultNetworkHelper* default_network_helper();
  const QuicClientDefaultNetworkHelper* default_network_helper() const;
};

}  // namespace quic

#endif /* SRC_MY_QUIC_CLIENT_H_ */
