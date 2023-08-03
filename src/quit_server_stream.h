#ifndef SRC_QUIT_SERVER_STREAM_H_
#define SRC_QUIT_SERVER_STREAM_H_

#include <cstdint>

#include "absl/strings/string_view.h"
#include "absl/types/optional.h"
#include "quiche/quic/core/http/quic_spdy_server_stream_base.h"
#include "quiche/quic/core/quic_error_codes.h"
#include "quiche/quic/core/quic_packets.h"
#include "quiche/quic/tools/quic_backend_response.h"
#include "quiche/quic/tools/quic_simple_server_backend.h"
#include "quiche/spdy/core/http2_header_block.h"
#include "quiche/spdy/core/spdy_framer.h"

using namespace quic;

namespace quit {

// All this does right now is aggregate data, and on fin, send an HTTP
// response.
class QuitServerStream : public QuicSpdyServerStreamBase,
                               public QuicSimpleServerBackend::RequestHandler {
 public:
  QuitServerStream(QuicStreamId id, QuicSpdySession* session,
                         StreamType type,
                         QuicSimpleServerBackend* quic_simple_server_backend);
  QuitServerStream(PendingStream* pending, QuicSpdySession* session,
                         QuicSimpleServerBackend* quic_simple_server_backend);
  QuitServerStream(const QuitServerStream&) = delete;
  QuitServerStream& operator=(const QuitServerStream&) = delete;
  ~QuitServerStream() override;

  // QuicSpdyStream
  void OnInitialHeadersComplete(bool fin, size_t frame_len,
                                const QuicHeaderList& header_list) override;
  void OnCanWrite() override;

  // QuicStream implementation called by the sequencer when there is
  // data (or a FIN) to be read.
  void OnBodyAvailable() override;

  void OnInvalidHeaders() override;

  // The response body of error responses.
  static const char* const kErrorResponseBody;
  static const char* const kNotFoundResponseBody;

  // Implements QuicSimpleServerBackend::RequestHandler callbacks
  QuicConnectionId connection_id() const override;
  QuicStreamId stream_id() const override;
  std::string peer_host() const override;
  QuicSpdyStream* GetStream() override;
  void OnResponseBackendComplete(const QuicBackendResponse* response) override;
  void SendStreamData(absl::string_view data, bool close_stream) override;
  void TerminateStreamWithError(QuicResetStreamError error) override;

  void Respond(const QuicBackendResponse* response);

 protected:
  // Handles fresh body data whenever received when method is CONNECT.
  void HandleRequestConnectData(bool fin_received);

  // Sends a response using SendHeaders for the headers and WriteData for the
  // body.
  virtual void SendResponse();

  // Sends a basic 500 response using SendHeaders for the headers and WriteData
  // for the body.
  virtual void SendErrorResponse();
  virtual void SendErrorResponse(int resp_code);

  // Sends a basic 404 response using SendHeaders for the headers and WriteData
  // for the body.
  void SendNotFoundResponse();

  // Sends the response header (if not `absl::nullopt`) and body, but not the
  // fin.
  void SendIncompleteResponse(
      absl::optional<spdy::Http2HeaderBlock> response_headers,
      absl::string_view body);

  void SendHeadersAndBody(spdy::Http2HeaderBlock response_headers,
                          absl::string_view body);
  void SendHeadersAndBodyAndTrailers(
      absl::optional<spdy::Http2HeaderBlock> response_headers,
      absl::string_view body, spdy::Http2HeaderBlock response_trailers);

  spdy::Http2HeaderBlock* request_headers() { return &request_headers_; }

  // Returns true iff the request (per saved `request_headers_`) is a CONNECT or
  // Extended CONNECT request.
  bool IsConnectRequest() const;

  const std::string& body() { return body_; }

  // Writes the body bytes for the GENERATE_BYTES response type.
  void WriteGeneratedBytes();

  void set_quic_simple_server_backend_for_test(
      QuicSimpleServerBackend* backend) {
    quic_simple_server_backend_ = backend;
  }

  bool response_sent() const { return response_sent_; }
  void set_response_sent() { response_sent_ = true; }
  // The parsed headers received from the client.
  spdy::Http2HeaderBlock request_headers_;
  int64_t content_length_;
  std::string body_;

 private:
  uint64_t generate_bytes_length_;
  // Whether response headers have already been sent.
  bool response_sent_ = false;

  std::unique_ptr<QuicAlarm> delayed_response_alarm_;

  QuicSimpleServerBackend* quic_simple_server_backend_;  // Not owned.
};

}

#endif /* SRC_QUIT_SERVER_STREAM_H_ */
