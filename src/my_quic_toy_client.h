// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// A toy client, which connects to a specified port and sends QUIC
// requests to that endpoint.

#ifndef SRC_QUIC_TOY_CLIENT_H_
#define SRC_QUIC_TOY_CLIENT_H_


#include "my_quic_epoll_client_factory.h"

namespace quic {

class MyQuicToyClient : public QuicSpdyStream::Visitor{
 public:
  // Constructs a new toy client that will use |client_factory| to create the
  // actual QuicSpdyClientBase instance.
  MyQuicToyClient(MyQuicEpollClientFactory* client_factory);

  // Connects to the QUIC server based on the various flags defined in the
  // .cc file, sends requests and prints the responses. Returns 0 on success
  // and non-zero otherwise.
  int SendRequestsAndPrintResponses();

  int SendWebtransport();

  void OnClose(QuicSpdyStream* stream) override;

 private:
  MyQuicEpollClientFactory* client_factory_;  // Unowned.
};

}  // namespace quic


#endif /* SRC_QUIC_TOY_CLIENT_H_ */
