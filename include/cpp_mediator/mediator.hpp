#ifndef _HOLDEN_MEDIATOR_HPP_
#define _HOLDEN_MEDIATOR_HPP_

#include <stdexcept>
#include <memory>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <utility>

namespace holden {

namespace detail {
  struct request_base {};
} // namespace detail

template <typename TResponse, typename THandler>
struct request : detail::request_base {
 public:
  using response_type = TResponse;
  using handler_type = THandler;

  virtual ~request() {}
};

template <typename TRequest>
class request_handler {
 public:
  using request_type = TRequest;
  using response_type = typename TRequest::response_type;

  virtual typename TRequest::response_type handle(const TRequest& r) = 0;

  virtual ~request_handler() {}
};

class mediator {
 public:
  mediator() {}
  static std::unordered_map<int, int> col;
  std::unordered_map<size_t, std::shared_ptr<void>> handlers_by_type{};

  template <typename THandler>
  void register_handler(std::shared_ptr<THandler> handler) {
    handlers_by_type.emplace(
      typeid(*handler).hash_code(), static_cast<std::shared_ptr<void>>(handler)
    );
  }

  // Send a message to be sent handleded by the registered request_handler.
  // Throw if no handler is found.
  template<typename TRequest,
    typename = std::enable_if<std::is_base_of<detail::request_base, TRequest>::value>>
  typename TRequest::response_type send(const TRequest& r) {
    using handler_t = typename TRequest::handler_type;

    auto hash = typeid(handler_t).hash_code();
    for (auto& a : handlers_by_type) {
      if (handlers_by_type.find(hash) != handlers_by_type.end()) {
        auto handler = std::static_pointer_cast<handler_t>(handlers_by_type[hash]);
        return handler->handle(r);
      }
    }

    // could not find handler; throw.
    std::string err_msg
      = std::string{ "no matching handler found for the provided request of typeid.name: '" }
      + typeid(TRequest).name() + "' with expected handler of type '"
      + typeid(handler_t).name() + "'.";
    throw std::invalid_argument(err_msg);
  }

  virtual ~mediator() {}
};

} // namespace holden

#endif // _HOLDEN_MEDIATOR_HPP_
