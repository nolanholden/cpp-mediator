#include "../include/cpp_mediator/mediator.hpp"
#include <gtest/gtest.h>

#define USE(x) do { (void)x; } while (0)

struct GetA : holden::request<int>{};
class AHandler1 : public holden::request_handler<GetA> {
  public:
   int handle(const GetA& m) { USE(m); return 1; }
};
class AHandler2 : public holden::request_handler<GetA> {
  public:
   int handle(const GetA& m) { USE(m); return 2; }
};

struct GetB : holden::request<int>{};
class BHandler : public holden::request_handler<GetB> {
  public:
   int handle(const GetB& m) { USE(m); return 1; }
};


TEST(cpp_mediator, send_receive) {
  AHandler1 a1{};
  AHandler2 a2{};
  BHandler b{};

  // do not compile!:
  // auto m = holden::make_mediator(a1, a1, b);
  auto m = holden::make_mediator(a1, a2, b);

  //auto m = holden::make_mediator(a1, b);
  (void)m;
  
  m.send(GetA{});
}
