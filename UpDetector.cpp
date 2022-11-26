#include <unistd.h>

#include <concepts>
#include <coroutine>
#include <iostream>
#include <string>
#include <type_traits>

#include "RefreshingGenerator.hpp"
#include "httplib.h"


httplib::Server svr;

template <long Timeout_v = 20, chrono::Duration_Type Timeout_t = chrono::seconds,
          long RefreshInterval_v = 30, chrono::Duration_Type RefreshInverval_t = chrono::seconds>
class UpDetector {

    static constexpr Timeout_t Timeout_duration{Timeout_v};
    static constexpr time_t Timeout_sec =
        chrono::duration_cast<chrono::seconds>(Timeout_duration).count();

    httplib::Client clt;
    const std::string host;
    const std::string path;

    RefreshingGenerator<bool, RefreshInterval_v, RefreshInverval_t> isUp() {
        while (true) {
            auto res = clt.Get(path);
            co_yield res.error() == httplib::Error::Success;
        }
    }

    RefreshingGenerator<bool, RefreshInterval_v, RefreshInverval_t> generator;

   public:
    auto operator()() { return generator(); }

    UpDetector(const std::string& host) : host{host}, clt{host}, path{"/"}, generator{isUp()} {
        clt.set_connection_timeout(Timeout_sec);
    }

    UpDetector(const std::string& host, const std::string& path)
        : host{host}, clt{host}, path{path}, generator{isUp()} {
        clt.set_connection_timeout(Timeout_sec);
    }
};

int main() {
    UpDetector<1, chrono::seconds, 5, chrono::seconds> vfGen("http://vexforum.com");

    std::cout << vfGen() << std::endl;
    std::cout << vfGen() << std::endl;
    std::cout << vfGen() << std::endl;
}