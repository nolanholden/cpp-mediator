#include "../include/cpp_mediator/mediator.hpp"
#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>


#define USE(x) do { (void)x; } while (0)

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



#define M() holden::mediator m{}
#define REG_HAN(id) \
  auto handler##id = std::make_shared<ReqHandler##id>(); \
  m.register_handler(handler##id)
#define SendForWithArgs(id, request_args) \
  M(); \
  REG_HAN(id); \
  Req##id request_##id{request_args}; \
  auto actual = m.send(request_##id)

#define SendFor(id) \
  M(); \
  REG_HAN(id); \
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

TEST(cpp_mediator, assert_throw_on_no_register) {
  M();
  ASSERT_THROW(m.send(Reqa{}), std::invalid_argument);
}

TEST(cpp_mediator, assert_proper_resolution) {
  M();
  Reqa request_a{};
  Reqb request_b{3};
  Reqc request_c{3};
  int three = 3;
  Reqd request_d{&three};

  ASSERT_THROW(m.send(request_a), std::invalid_argument);
  ASSERT_THROW(m.send(request_b), std::invalid_argument);
  ASSERT_THROW(m.send(request_c), std::invalid_argument);
  ASSERT_THROW(m.send(request_d), std::invalid_argument);

  REG_HAN(a);
  ASSERT_THROW(m.send(request_b), std::invalid_argument);
  ASSERT_THROW(m.send(request_c), std::invalid_argument);
  ASSERT_THROW(m.send(request_d), std::invalid_argument);

  REG_HAN(b);
  ASSERT_THROW(m.send(request_c), std::invalid_argument);
  ASSERT_THROW(m.send(request_d), std::invalid_argument);

  REG_HAN(c);
  ASSERT_THROW(m.send(request_d), std::invalid_argument);

  REG_HAN(d);
  
  ASSERT_EQ(127,    m.send(request_a));
  ASSERT_EQ(3,      m.send(request_b));
  ASSERT_EQ(3 + 1,  m.send(request_c));
  ASSERT_EQ(3,      m.send(request_d));
  ASSERT_EQ(4, *request_d.x);
  ASSERT_EQ(4, three);
  ASSERT_EQ(4, m.send(request_d));
  ASSERT_EQ(5, *request_d.x);
  ASSERT_EQ(5, three);
}
