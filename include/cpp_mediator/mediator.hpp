#ifndef _HOLDEN_MEDIATOR_HPP_
#define _HOLDEN_MEDIATOR_HPP_

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

namespace holden {

namespace detail {
namespace tuple_searching {

template<class...>
struct voider { using type = void; };

template<class...Ts>
using void_t = typename voider<Ts...>::type;

template<template<class...> class Test, class Tuple>
struct get_first_pass;

template<template<class...> class Test, class Tuple>
using get_first_pass_t = typename get_first_pass<Test, Tuple>::type;

template<template<class...> class, class, class...>
struct first_pass {};

template<template<class...> class Test, class T0, class...Ts>
struct first_pass<Test, std::enable_if_t<!Test<T0>::value>, T0, Ts...> :
  first_pass<Test, void, Ts...>
{};

template<template<class...> class Test, class T0, class...Ts>
struct first_pass<Test, std::enable_if_t<Test<T0>::value>, T0, Ts...> {
  using type = T0;
};

template<template<class...> class Test, template<class...> class Tuple, class...Ts>
struct get_first_pass<Test, Tuple<Ts...>> : first_pass<Test, void, Ts...>
{};

template<class Base>
struct is_derived_from {
  template<class Derived>
  using test = std::is_base_of<std::decay_t<Base>, std::decay_t<Derived>>;
};

template<class Base, class Tuple>
using get_first_derived =
  get_first_pass_t<is_derived_from<Base>::template test, Tuple>;

template<class Base, class Tuple>
auto get_from_base(Tuple&& tuple)
-> decltype(std::get< get_first_derived<Base, std::decay_t<Tuple>> >(
                                                  std::forward<Tuple>(tuple))) {
  return std::get< get_first_derived<Base, std::decay_t<Tuple>> >(
                                                  std::forward<Tuple>(tuple));
}

} // namespace tuple_searching

template<typename T> inline
T& ref(T& x) { return x; }
template<typename T> inline
T& ref(T* x) { return *x; }

} // namespace detail


template <typename TResponse, typename THandler>
struct request {
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

class mediator_base {};

template <typename ...Handlers>
class mediator final : public mediator_base {
  std::tuple<Handlers...> handlers_;

 public:
  mediator(Handlers... handlers)
    : handlers_(handlers...) {}

  template<typename TRequest>
  auto send(const TRequest& r) -> typename TRequest::response_type {
    using namespace detail;
    using namespace detail::tuple_searching;
    using request_t = std::decay_t<TRequest>;
    using handler_t = request_handler<request_t>;
    return ref(get_from_base<handler_t>(handlers_)).handle(r);
  }
};

template <typename... Args>
mediator<Args&...> make_mediator(Args&... args) {
  return mediator<Args&...>(args...);
}

} // namespace holden

#endif // _HOLDEN_MEDIATOR_HPP_
