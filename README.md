# cpp-mediator
[![Build Status](https://travis-ci.org/nolanholden/cpp-mediator.svg?branch=master)](https://travis-ci.org/nolanholden/cpp-mediator)
[![Coverage Status](https://coveralls.io/repos/github/nolanholden/cpp-mediator/badge.svg?branch=master)](https://coveralls.io/github/nolanholden/cpp-mediator?branch=master)

A simple mediator implementation in C++ 

### Example _hello, world_ program using `cpp-mediator` below.
```c++
// A simple hello world program using `cpp-mediator`.
#include <cpp_mediator/mediator.hpp>

#include <iostream>

class Speaker;

// Define a request.
struct SayHello : public holden::request<void, Speaker> {
 public:
  SayHello(std::ostream& out) : output_stream(out) {}

  // the output stream through which the handler should "say hello."
  std::ostream& output_stream;
};

// Configure a request handler.
class Speaker : public holden::request_handler<SayHello> {
 public:
  // Say hello on the provided output stream.
  void handle(const SayHello& r) {
    r.output_stream << "Hello, world!\n";
  }
};


int main() {
  holden::mediator m{};

  auto speaker = std::make_shared<Speaker>();
  m.register_handler(speaker);

  SayHello request{ std::cout };
  m.send(request);

  return 0;
}

/* output:
 *
 * $ Hello, world!
*/
```
