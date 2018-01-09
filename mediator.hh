#ifndef _HOLDEN_MEDIATOR_H_
#define _HOLDEN_MEDIATOR_H_

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

  std::

  template<typename TResponse>
  TResponse send(const request<TResponse>& r) {}

  virtual ~mediator() {}
};

class GetAltitude : public request<int> {};
class AltitudeHandler : public request_handler<GetAltitude> {
 public:
  int handle(GetAltitude m) {}
};

void go() {
  mediator m{};
  GetAltitude g{};
  GetAltitude message{ };

  m.send(message);
  auto x = m.send(g);
}


} // namespace mediator
} // namespace holden

#endif // _HOLDEN_MEDIATOR_H_

int main() { return 0; }
