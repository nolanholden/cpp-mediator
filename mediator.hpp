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

class request_handler_base {};

template <typename TRequest>
class request_handler : public request_handler_base {
 public:
  using handler_response_type = typename TRequest::response_type;
  virtual typename TRequest::response_type handle(const TRequest& r) = 0;
  virtual ~request_handler() {}
};

class mediator {
 public:
  mediator() {}
  std::unordered_map<size_t,
    std::shared_ptr<request_handler<request<void*>>>> handlers_by_type{};

  template <typename THandler>
  void register_handler(std::shared_ptr<THandler> handler) {
    std::cout << "registering...";
    handlers_by_type.emplace(
      typeid(*handler).hash_code(),
      reinterpret_cast<std::shared_ptr<request_handler<request<void*>>>>(handler)
    );
    std::cout << "registered\n";
  }

  template<typename TRequest>
  typename TRequest::response_type send(const TRequest& r) {
    std::cout << "sending...";
    auto hash = typeid(r).hash_code();
    for (decltype(handlers_by_type.size())
      i = 0; i < handlers_by_type.size(); ++i) {
      if (handlers_by_type.find(hash) != handlers_by_type.end()) {
        auto handler_ptr = handlers_by_type[hash];
        (reinterpret_cast<std::shared_ptr<request_handler<request<typename TRequest::response_type>>>>(handler_ptr))->handle;
      }
    }
    std::cout << "sent\n";
    return 1;
  }

  virtual ~mediator() {}
};

class req : public request<int> {};
class req_handler : public request_handler<req> {
 public:
  int handle(const req& r) {
    return 7;
  }
};

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
  std::cout << "final value: " << x << "\n\ndone.\n";
  return 0;
}
