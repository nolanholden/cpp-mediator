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

namespace detail {

// strategy to extract the template argument of a template type
template<typename T>
struct template_type {
  typedef T arg;
};

// specialization for multiple template arguments
template<template<typename, typename ...> class X, typename T,
  typename ...Args>
struct template_type<X<T, Args...>> {
  typedef T arg;
};

} // namespace detail

template <typename TResponse>
class request {
 public:
   virtual ~request() {}
};

template <typename TRequest>
class request_handler {
 public:
  typedef typename
    detail::template_type<TRequest>::arg handle(TRequest message);
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

  template<typename TRequest> typename detail::template_type<TRequest>::arg
  send(const TRequest& r) {
    std::cout << "sending...";
    auto hash = std::type_index{ typeid(r) };
    for (auto i = 0; i < handlers.size(); ++i) {
    }
    std::cout << "sent\n";
  }

  virtual ~mediator() {}
};


class req : request<int> {};
class req_handler : request_handler<req> {};

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
