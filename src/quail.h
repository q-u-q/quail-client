#ifndef SRC_QUAIL_H_
#define SRC_QUAIL_H_

#include <string>

namespace quail {

class QuailClient {
 public:
  QuailClient(const std::string& url);
  void Start();

  std::string url_;
};

}  // namespace quail

#endif /* SRC_QUAIL_H_ */
