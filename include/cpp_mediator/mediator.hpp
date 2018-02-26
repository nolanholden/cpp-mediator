#ifndef _HOLDEN_MEDIATOR_HPP_
#define _HOLDEN_MEDIATOR_HPP_

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

namespace holden {

namespace detail {
namespace tuple_searching {


template<class...>  struct voider { using type = void; };
template<class...Ts> using void_t = typename voider<Ts...>::type;

template<template<class...> class Test, class Tuple>
struct get_first_passing_type;
template<template<class...> class Test, class Tuple>
using get_first_passing_type_t
  = typename get_first_passing_type<Test, Tuple>::type;

template<template<class...> class, class, class...> struct first_passing_type;
template<template<class...> class Test, class T0, class...Ts>
struct first_passing_type<Test, std::enable_if_t<!Test<T0>::value>, T0, Ts...>
  : first_passing_type<Test, void, Ts...>
{};

template <bool... Ts>
struct any_of : public std::false_type {  };

template <bool b, bool... Ts>
struct any_of<b, Ts...>
  : public std::conditional_t<b, std::true_type, any_of<Ts...>> {};

template<template<class...> class Test, class T0, class...Ts>
struct first_passing_type<Test, std::enable_if_t<Test<T0>::value>, T0, Ts...> {
  using type = T0;
  static_assert(!any_of<Test<Ts>::value...>::value, 
                "multiple types are registered for a given request");
};

template<template<class...> class Test,
  template<class...> class Tuple, class...Ts>
struct get_first_passing_type<Test, Tuple<Ts...>>
  : first_passing_type<Test, void, Ts...>
{};

template<class Base>
struct is_derived_from {
  template<class Derived>
  using test = std::is_base_of<std::decay_t<Base>, std::decay_t<Derived>>;
};

template<class Base, class Tuple>
using get_first_derived =
  get_first_passing_type_t<is_derived_from<Base>::template test, Tuple>;

template<class Base, class Tuple>
auto get_from_base(Tuple&& tuple)
-> decltype(std::get< get_first_derived<Base,
      std::decay_t<Tuple>> >(std::forward<Tuple>(tuple))) {
  return std::get< get_first_derived<Base,
      std::decay_t<Tuple>> >(std::forward<Tuple>(tuple));
}

} // namespace tuple_searching

template<typename T> inline
T& ref(T& x) { return x; }
template<typename T> inline
T& ref(T* x) { return *x; }

} // namespace detail

// An optional pure-virtual struct to mark request types
template <typename TResponse>
struct request {
  using response_type = TResponse;
  virtual ~request() = 0;
};

template <typename TResponse>
request<TResponse>::~request() {}


template <typename TRequest>
struct request_handler {
  virtual ~request_handler() = 0;
};

template <typename TRequest>
request_handler<TRequest>::~request_handler() {}


template <typename ...Handlers>
class mediator {
 protected:
  std::tuple<Handlers...> handlers_;

 public:
  mediator(Handlers... handlers) : handlers_(handlers...) {}

  template<typename TRequest>
  auto send(const TRequest& r) -> typename TRequest::response_type {
    using namespace detail;
    using namespace detail::tuple_searching;
    using handler_t = request_handler<std::decay_t<TRequest>>;
    return ref(get_from_base<handler_t>(handlers_)).handle(r);
  }

  virtual ~mediator() {}
};

template <typename... Args>
mediator<Args&...> make_mediator(Args&... args) {
  return mediator<Args&...>(args...);
}

} // namespace holden

#endif // _HOLDEN_MEDIATOR_HPP_
