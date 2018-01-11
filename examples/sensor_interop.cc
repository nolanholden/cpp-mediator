// The code below demonstrates simple usage of the mediator library.

#include "../mediator.hpp"

#include <iostream>
#include <stdexcept>

// Let's shorten some names for convenience.
using mediator = holden::mediator::mediator;
template <typename T, typename U>
using request = holden::mediator::request<T, U>;
template <typename T>
using request_handler = holden::mediator::request_handler<T>;

enum class OperatingMode {
  Mode0,
  Mode1,
};

enum class Unit {
  Feet,
  Meters,
};

// We must forward declare our WeatherSensor because requests require their 
// handler as a template argument to permit template deduction when sending 
// requests through the mediator (due to fundamental limitations of C++).
class WeatherSensor;


// Define requests:

class GetHumidity : public request<double, WeatherSensor> {};

class GetAltitude : public request<int, WeatherSensor> {
 public:
  Unit unit = Unit::Meters;
};

class SetMode : public request<void, WeatherSensor> {
 public:
  OperatingMode mode;
};


// Configure a handler for your requests:

class WeatherSensor
  : public request_handler<GetHumidity>
  , public request_handler<GetAltitude>
  , public request_handler<SetMode> {
 public:
  // Do whatever is needed, and give the requestor the humidity.
  double handle(const GetHumidity& r) {
    return find_humidity();
  }

  // Do whatever is needed, and give the requestor the altitude.
  int handle(const GetAltitude& r) {
    auto meters = find_altitude_meters();

    switch (r.unit) {
      case Unit::Meters: return meters;
      case Unit::Feet:   return meters * 3.2808;
      default:
        throw new std::invalid_argument("unit not supported.");
    }
  }

  // Set the operating mode and return nothing
  void handle(const SetMode& s) {
    mode_ = s.mode;
  }

  // These would more-likely require special knowledge from WeatherSensor, 
  // but for demonstration we will just use literals.
  double find_humidity() { return 55.3; }

  int find_altitude_meters() {
    return 8848;
  }

  OperatingMode mode() const { return mode_; }

 private:
  OperatingMode mode_{};
};


int main() {
  mediator med{};

  auto weather_sensor = std::make_shared<WeatherSensor>();
  med.register_handler(weather_sensor);

  GetHumidity h{};
  auto humidity = med.send(h);
  std::cout << "humidity is: " << humidity << "\n";

  {
    GetAltitude a{};
    a.unit = Unit::Feet;
    auto altitude = med.send(a);
    std::cout << "altitude is: " << altitude << " feet\n";
  }

  {
    GetAltitude a{};
    a.unit = Unit::Meters;
    auto altitude = med.send(a);
    std::cout << "altitude is: " << altitude << " meters\n";
  }

  std::cout << "operating mode is: Mode" << (int)weather_sensor->mode() << "\n";
  SetMode m{};
  m.mode = OperatingMode::Mode1;
  med.send(m);
  std::cout << "operating mode is: Mode" << (int)weather_sensor->mode() << "\n";

  return 0;
}

/* output:
 *
 * $ humidity is: 55.3
 * $ altitude is: 29028 feet
 * $ altitude is: 8848 meters
 * $ operating mode is: Mode0
 * $ operating mode is: Mode1
*/