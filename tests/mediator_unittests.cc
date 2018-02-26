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
