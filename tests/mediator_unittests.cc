#include "../include/cpp_mediator/mediator.hpp"
#include <gtest/gtest.h>

#define USE(x) do { (void)x; } while (0)

struct GetA : holden::request<int>{};
class AHandler : public holden::request_handler<GetA> {
  public:
   int handle(const GetA& m) { USE(m); return 1; }
};

struct GetB : holden::request<int>{};
class BHandler : public holden::request_handler<GetB> {
  public:
   int handle(const GetB& m) { USE(m); return 1; }
};


TEST(cpp_mediator, send_receive) {
  AHandler a{};
  BHandler b{};

  // do not compile!:
  // auto m = holden::make_mediator(a, a, b);

  auto m = holden::make_mediator(a, b);
  (void)m;
  
  m.send(GetA{});
}
