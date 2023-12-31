// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "my_quic_client.h"

#include <utility>

#include "quiche/quic/core/quic_connection.h"
#include "quiche/quic/core/quic_default_connection_helper.h"
#include "quiche/quic/core/quic_server_id.h"
#include "quiche/quic/platform/api/quic_socket_address.h"
#include "quiche/quic/tools/quic_simple_client_session.h"


namespace quic {

MyQuicClient::MyQuicClient(
    QuicSocketAddress server_address, const QuicServerId& server_id,
    const ParsedQuicVersionVector& supported_versions,
    QuicEventLoop* event_loop, std::unique_ptr<ProofVerifier> proof_verifier)
    : MyQuicClient(
          server_address, server_id, supported_versions, QuicConfig(),
          event_loop,
          std::make_unique<QuicClientDefaultNetworkHelper>(event_loop, this),
          std::move(proof_verifier), nullptr) {}

MyQuicClient::MyQuicClient(
    QuicSocketAddress server_address, const QuicServerId& server_id,
    const ParsedQuicVersionVector& supported_versions,
    QuicEventLoop* event_loop, std::unique_ptr<ProofVerifier> proof_verifier,
    std::unique_ptr<SessionCache> session_cache)
    : MyQuicClient(
          server_address, server_id, supported_versions, QuicConfig(),
          event_loop,
          std::make_unique<QuicClientDefaultNetworkHelper>(event_loop, this),
          std::move(proof_verifier), std::move(session_cache)) {}

MyQuicClient::MyQuicClient(
    QuicSocketAddress server_address, const QuicServerId& server_id,
    const ParsedQuicVersionVector& supported_versions, const QuicConfig& config,
    QuicEventLoop* event_loop, std::unique_ptr<ProofVerifier> proof_verifier,
    std::unique_ptr<SessionCache> session_cache)
    : MyQuicClient(
          server_address, server_id, supported_versions, config, event_loop,
          std::make_unique<QuicClientDefaultNetworkHelper>(event_loop, this),
          std::move(proof_verifier), std::move(session_cache)) {}

MyQuicClient::MyQuicClient(
    QuicSocketAddress server_address, const QuicServerId& server_id,
    const ParsedQuicVersionVector& supported_versions,
    QuicEventLoop* event_loop,
    std::unique_ptr<QuicClientDefaultNetworkHelper> network_helper,
    std::unique_ptr<ProofVerifier> proof_verifier)
    : MyQuicClient(server_address, server_id, supported_versions,
                        QuicConfig(), event_loop, std::move(network_helper),
                        std::move(proof_verifier), nullptr) {}

MyQuicClient::MyQuicClient(
    QuicSocketAddress server_address, const QuicServerId& server_id,
    const ParsedQuicVersionVector& supported_versions, const QuicConfig& config,
    QuicEventLoop* event_loop,
    std::unique_ptr<QuicClientDefaultNetworkHelper> network_helper,
    std::unique_ptr<ProofVerifier> proof_verifier)
    : MyQuicClient(server_address, server_id, supported_versions, config,
                        event_loop, std::move(network_helper),
                        std::move(proof_verifier), nullptr) {}

MyQuicClient::MyQuicClient(
    QuicSocketAddress server_address, const QuicServerId& server_id,
    const ParsedQuicVersionVector& supported_versions, const QuicConfig& config,
    QuicEventLoop* event_loop,
    std::unique_ptr<QuicClientDefaultNetworkHelper> network_helper,
    std::unique_ptr<ProofVerifier> proof_verifier,
    std::unique_ptr<SessionCache> session_cache)
    : QuicSpdyClientBase(server_id, supported_versions, config,
                         new QuicDefaultConnectionHelper(),
                         event_loop->CreateAlarmFactory().release(),
                         std::move(network_helper), std::move(proof_verifier),
                         std::move(session_cache)) {
  set_server_address(server_address);
}

MyQuicClient::~MyQuicClient() = default;

std::unique_ptr<QuicSession> MyQuicClient::CreateQuicClientSession(
    const ParsedQuicVersionVector& supported_versions,
    QuicConnection* connection) {
  return std::make_unique<QuicSimpleClientSession>(
      *config(), supported_versions, connection, this, network_helper(),
      server_id(), crypto_config(), push_promise_index(), drop_response_body(),
      true);
}

QuicClientDefaultNetworkHelper* MyQuicClient::default_network_helper() {
  return static_cast<QuicClientDefaultNetworkHelper*>(network_helper());
}

const QuicClientDefaultNetworkHelper*
MyQuicClient::default_network_helper() const {
  return static_cast<const QuicClientDefaultNetworkHelper*>(network_helper());
}

}  // namespace quic
