#include "../include/cpp_mediator/mediator.hpp"
#include <gtest/gtest.h>

#define USE(x) do { (void)x; } while (0)

struct GetA : holden::request<int>{};
class AHandler1 : holden::request_handler<GetA> {
  public:
   int handle(const GetA& m) { USE(m); return 1; }
};
class AHandler2 : holden::request_handler<GetA> {
  public:
   int handle(const GetA& m) { USE(m); return 2; }
};

struct GetB : holden::request<int>{};
class BHandler : holden::request_handler<GetB> {
  public:
   int handle(const GetB& m) { USE(m); return 3; }
};

TEST(cpp_mediator, can_make_empty) {
  auto m = holden::make_mediator();
  USE(m);
}

TEST(cpp_mediator, send_receive) {
  AHandler1 a1{};
  AHandler2 a2{};
  BHandler b{};

  // None of these compile!:
  // 
  // auto m = holden::make_mediator(); // error: no handler for `GetA` exists
  // m.send(GetA{});
  // 
  // auto m = holden::make_mediator(b); // error: no handler for `GetA` exists
  // m.send(GetA{});
  // 
  // auto m = holden::make_mediator(a1, a2, b); // error: more than one handler 
  // m.send(GetA{});                            //  for a given request
  // 
  // auto m = holden::make_mediator(a1, a1, b); // error: more than one handler 
  // m.send(GetA{});                            //  for a given request
  // 

  {
    auto m = holden::make_mediator(b);
    auto b_res = m.send(GetB{});
    ASSERT_EQ(3, b_res);
  }

  {
    auto m = holden::make_mediator(a1, b);
    auto a_res =  m.send(GetA{});
    ASSERT_EQ(1, a_res);
    auto b_res = m.send(GetB{});
    ASSERT_EQ(3, b_res);
  }

  {
    auto m = holden::make_mediator(a2, b);
    auto a_res =  m.send(GetA{});
    ASSERT_EQ(2, a_res);
    auto b_res = m.send(GetB{});
    ASSERT_EQ(3, b_res);
  }
}



// Caution, the macros below are very fragile, are used here only to 
// minimize test code, and are not recommended for production use.


// Declares and defines a request and request handler using the provided 
// information.
// 
// typenames resolve as:
//         request:  "Req[id]"
// request_handler:  "ReqHandler[id]"
#define DEF_REQ_HAN_CUSTOM_REQUEST( \
  id, \
  return_type, \
  request_param_name, \
  handle_code, \
  request_definition_scope, \
  ctor_after_typename) \
  \
  struct Req##id : holden::request<return_type> { \
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

#define DEF_REQ_HAN( \
  id, \
  return_type, \
  request_param_name, \
  handle_code) \
  DEF_REQ_HAN_CUSTOM_REQUEST( \
    id, \
    return_type, \
    request_param_name, \
    handle_code, \
    void* dummy;, \
    /* default c'tor */ () {})



#define M(id) \
  ReqHandler##id handler##id{}; \
  auto m = holden::make_mediator(handler##id)
#define SendForWithArgs(id, request_args) \
  M(id); \
  Req##id request_##id{request_args}; \
  auto actual = m.send(request_##id)

#define SendFor(id) \
  M(id); \
  Req##id request_##id{}; \
  auto actual = m.send(request_##id)




// TESTS BELOW:

DEF_REQ_HAN(a, int, m, return 127;);
TEST(cpp_mediator, send_receive1) {
  SendFor(a);
  ASSERT_EQ(127, actual);
}

DEF_REQ_HAN_CUSTOM_REQUEST(b, int, m, return m.x;, int x;, (int _x) : x(_x) {});
TEST(cpp_mediator, send_receive2) {
  SendForWithArgs(b, 1000);
  ASSERT_EQ(1000, actual);
}

DEF_REQ_HAN_CUSTOM_REQUEST(c, int, m, return m.x + 1;, int x;, (int _x) : x(_x){});
TEST(cpp_mediator, send_receive3) {
  SendForWithArgs(c, 1000);
  ASSERT_EQ(1001, actual);
}

DEF_REQ_HAN_CUSTOM_REQUEST(d, int, m, return (*m.x)++;, int* x;, (int* _x) : x(_x){});
TEST(cpp_mediator, request_mutations_preserved) {
  int number = 100;
  SendForWithArgs(d, &number);
  ASSERT_EQ(100, actual);
  ASSERT_EQ(101, *request_d.x);
  ASSERT_EQ(101, number);
  actual = m.send(request_d);
  ASSERT_EQ(101, actual);
  ASSERT_EQ(102, *request_d.x);
  ASSERT_EQ(102, number);
}
