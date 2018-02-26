#include "../include/cpp_mediator/mediator.hpp"
#include <gtest/gtest.h>

#include <memory>


#define USE(x) do { (void)x; } while (0)

// Caution, the macros below are very fragile, are used here only to 
// minimize test code, and are not recommended for production use.


// Declares and defines a request and request handler using the provided 
// information.
// 
// typenames resolve as:
//         request:  "Req[id]"
// request_handler:  "ReqHandler[id]"
#define DefReqHanCustomRequest( \
  id, \
  return_type, \
  request_param_name, \
  handle_code, \
  request_definition_scope, \
  ctor_after_typename) \
  \
  class ReqHandler##id; \
  struct Req##id : holden::request<return_type, ReqHandler##id> { \
    request_definition_scope \
    Req##id ctor_after_typename \
  }; \
  class ReqHandler##id : public holden::request_handler<Req##id> { \
  public: \
    typename Req##id::response_type \
    handle (const Req##id& request_param_name) { \
      /* handle the request */ \
      USE(request_param_name); \
      handle_code \
    } \
  }

#define DefReqHan( \
  id, \
  return_type, \
  request_param_name, \
  handle_code) \
  DefReqHanCustomRequest( \
    id, \
    return_type, \
    request_param_name, \
    handle_code, \
    /* no fields or methods */, \
    /* default c'tor */() {})



#define SendForWithArgs(id, request_args) \
  holden::mediator m{}; \
  auto handler = std::make_shared<ReqHandler##id>(); \
  m.register_handler(handler); \
  Req##id request{request_args}; \
  auto actual = m.send(request); \

#define SendFor(id) SendForWithArgs(id, /* no args */);




// TESTS BELOW:

DefReqHan(a, int, m, return 127;);
TEST(cpp_mediator, send_recv1) {
  SendFor(a);
  ASSERT_EQ(127, actual);
}

DefReqHanCustomRequest(b, int, m, return m.x;, int x;, (int _x) : x(_x) {});
TEST(cpp_mediator, send_recv2) {
  SendForWithArgs(b, 1000);
  ASSERT_EQ(1000, actual);
}

DefReqHanCustomRequest(c, int, m, return m.x + 1;, int x;, (int _x) : x(_x) {});
TEST(cpp_mediator, send_recv3) {
  SendForWithArgs(c, 1000);
  ASSERT_EQ(1001, actual);
}

DefReqHanCustomRequest(d, int, m, return (*m.x)++;, int* x;, (int* _x) : x(_x) {});
TEST(cpp_mediator, send_recv4) {
  int number = 100;
  SendForWithArgs(d, &number);
  ASSERT_EQ(100, actual);
  ASSERT_EQ(101, *request.x);
  ASSERT_EQ(101, number);
}
