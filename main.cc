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

#include "quic_client_network_helper.h"

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

  QuicConfig config;
  std::unique_ptr<quic::SessionCache> session_cache;

  auto helper = std::make_unique<QuicClientNetworkHelper>(event_loop_.get());
  auto helper_ptr = helper.get();
  auto client = std::make_unique<QuicSpdyClientBase>(
      server_id, versions, config, new QuicDefaultConnectionHelper(),
      event_loop_->CreateAlarmFactory().release(), std::move(helper),
      std::move(proof_verifier), std::move(session_cache));
  
  helper_ptr->SetClient(client.get());

  client->set_server_address(addr);

 //

  if (!client->Initialize()) {
    std::cerr << "Failed to initialize client." << std::endl;
    return 1;
  }
  if (!client->Connect()) {
    quic::QuicErrorCode error = client->session()->error();
    if (error == quic::QUIC_INVALID_VERSION) {
      std::cerr << "Failed to negotiate version with " << host_for_handshake << ":" << port
                << ". " << client->session()->error_details() << std::endl;

    }
    std::cerr << "Failed to connect to " << host_for_handshake << ":" << port << ". "
              << quic::QuicErrorCodeToString(error) << " "
              << client->session()->error_details() << std::endl;

  }


  while (true) {
    event_loop_->RunEventLoopOnce(QuicTime::Delta::FromMilliseconds(50));
  }

  return 0;
}
