// The code below demonstrates simple usage of the mediator library.

#include "../include/cpp_mediator/mediator.hpp"

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
