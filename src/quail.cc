
#include "quail.h"

#include "my_quic_toy_client.h"
#include "my_quic_epoll_client_factory.h"

namespace quail {

QuailClient::QuailClient(const std::string& url): url_(url){}

void QuailClient::Start(){
  quic::MyQuicEpollClientFactory factory;
  quic::MyQuicToyClient client(&factory);
  // return client.SendRequestsAndPrintResponses();
  client.SendWebtransport(url_);
}


}  // namespace quail