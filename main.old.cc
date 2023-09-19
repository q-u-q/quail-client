#include <unistd.h>
#include <iostream>
#include <memory>
#include <string>

#include "quiche/quic/core/io/quic_default_event_loop.h"
#include "quiche/quic/core/quic_connection.h"
#include "quiche/quic/core/quic_connection_id.h"
#include "quiche/quic/core/quic_default_clock.h"
#include "quiche/quic/core/quic_default_connection_helper.h"
#include "quiche/quic/core/quic_default_packet_writer.h"
#include "quiche/quic/core/quic_server_id.h"
#include "quiche/quic/core/quic_utils.h"
#include "quiche/quic/core/quic_versions.h"
#include "quiche/quic/platform/api/quic_socket_address.h"
#include "quiche/quic/tools/fake_proof_verifier.h"
#include "quiche/quic/tools/quic_client_default_network_helper.h"
#include "quiche/quic/tools/quic_name_lookup.h"
#include "quiche/quic/tools/quic_spdy_client_base.h"


using namespace quic;

int main(int argc, char* argv[]) {
  // version
  quic::ParsedQuicVersionVector versions = quic::CurrentSupportedVersions();
  for (const quic::ParsedQuicVersion& version : versions) {
    quic::QuicEnableVersion(version);
  }

  // verifier
  std::unique_ptr<quic::ProofVerifier> proof_verifier =
      std::make_unique<FakeProofVerifier>();

  //
  int address_family_for_lookup = AF_INET;  // ipv6 : AF_INET6
  std::string host_for_lookup = "192.168.97.138";
  std::string host_for_handshake = "192.168.97.138";
  uint16_t port = 4433;

  QuicSocketAddress addr = tools::LookupAddress(
      address_family_for_lookup, host_for_lookup, absl::StrCat(port));
  if (!addr.IsInitialized()) {
    QUIC_LOG(ERROR) << "Unable to resolve address: " << host_for_lookup;
  }
  QuicServerId server_id(host_for_handshake, port, false);

  // event loop
  auto event_loop_ = GetDefaultEventLoop()->Create(QuicDefaultClock::Get());

  int client_connection_id_length_ = 0;
  QuicConnectionId client_connection_id =
      QuicUtils::CreateRandomConnectionId(client_connection_id_length_);
  QuicConnectionId server_connection_id =
      QuicUtils::CreateRandomConnectionId(kQuicDefaultConnectionIdLength);

  auto helper = new QuicDefaultConnectionHelper();
  auto alarm_factory = event_loop_->CreateAlarmFactory().release();

  // fd
  QuicUdpSocketApi api;
  SocketFd fd = api.Create(addr.host().AddressFamilyToInt(),
                           /*receive_buffer_size =*/kDefaultSocketReceiveBuffer,
                           /*send_buffer_size =*/kDefaultSocketReceiveBuffer);
  if (fd == kInvalidSocketFd) {
    return fd;
  }

  api.EnableDroppedPacketCount(fd);
  api.EnableReceiveTimestamp(fd);

  // QuicClientDefaultNetworkHelper::CreateUDPSocketAndBind(
  //  register fd

  // writer

  auto writer = new QuicLevelTriggeredPacketWriter(fd, event_loop_.get());

  DeterministicConnectionIdGenerator connection_id_generator_{
      kQuicDefaultConnectionIdLength};
  // connection

  auto connection =
      new QuicConnection(server_connection_id, QuicSocketAddress(), addr,
                         helper, alarm_factory, writer,
                         /* owns_writer= */ false, Perspective::IS_CLIENT,
                         versions, connection_id_generator_);

  connection->set_client_connection_id(client_connection_id);

  QuicConfig config_;
  std::unique_ptr<quic::SessionCache> session_cache;
  QuicCryptoClientConfig crypto_config_(std::move(proof_verifier), std::move(session_cache));
  QuicClientPushPromiseIndex push_promise_index_;

  auto session = std::make_unique<QuicSpdyClientSession>(
      config_, ParsedQuicVersionVector{versions},
      connection,  // session_ takes ownership of connection_ here.
      server_id, &crypto_config_, &push_promise_index_);
  session->Initialize();
  session->CryptoConnect();

  // create session
  // std::make_unique<QuicSpdyClientSession>(
  //     *config(), supported_versions, connection, server_id(),
  //     crypto_config(), &push_promise_index_);

  while (true) {
    event_loop_->RunEventLoopOnce(QuicTime::Delta::FromMilliseconds(50));
  }

  return 0;
}
