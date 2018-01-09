#ifndef _HOLDEN_MEDIATOR_H_
#define _HOLDEN_MEDIATOR_H_

#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>

namespace holden {
namespace mediator {

template <typename TResponse>
class request {
 public:
  using response_type = TResponse;
  virtual ~request() {}
};

template <typename TRequest>
class request_handler {
 public:
  virtual ~request_handler() {}
};

class mediator {
 public:
  mediator() {}
  std::vector<std::type_index> handlers{};
  std::vector<size_t> hashes{};

  template <typename THandler>
  void register_handler(std::shared_ptr<THandler> handler) {
    std::cout << "registering...";
    std::type_index t{ typeid(*handler) };
    hashes.emplace_back(t.hash_code());
    handlers.emplace_back(t);
    std::cout << "registered\n";
  }

  template<typename TRequest, typename TResponse = TRequest::response_type>
  TResponse send(const TRequest& r) {
    std::cout << "sending...";
    auto hash = std::type_index{ typeid(r) };
    for (decltype(handlers.size()) i = 0; i < handlers.size(); ++i) {
    }
    std::cout << "sent\n";
    return 0;
  }

  virtual ~mediator() {}
};

class req : public request<int> {};
class req_handler : public request_handler<req> {};

} // namespace mediator
} // namespace holden

#endif // _HOLDEN_MEDIATOR_H_

using namespace holden::mediator;

int main() {
  std::cout << "go\n";
  mediator m{};
  req r{};
  auto h = std::make_shared<req_handler>();
  m.register_handler(h);

  auto x = m.send(r);
  std::cout << "\ndone.\n";
  return 0;
}
