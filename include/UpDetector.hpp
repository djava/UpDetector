#pragma once

#include <concepts>
#include <coroutine>
#include <iostream>
#include <string>
#include <type_traits>

#include "RefreshingGenerator.hpp"
#include "chrono.hpp"
#include "httplib.hpp"

namespace UpDetector {

using namespace std::chrono_literals;

template <long RefreshInterval_v = 30l, chrono::Duration_Type RefreshInterval_t = chrono::seconds>
class WebsiteUpDetector {
    using GeneratorType = RefreshingGenerator<bool, RefreshInterval_v, RefreshInterval_t>;

    chrono::seconds timeout;

    httplib::Client clt;
    const std::string host;
    const std::string path;

    GeneratorType generator;

    GeneratorType generatorFn() {
        while (true) {
            auto res = clt.Get(path);
            co_yield res.error() == httplib::Error::Success;
        }
    }

   public:
    WebsiteUpDetector(const std::string& host, const chrono::seconds& timeout = 10s)
        : host{host}, clt{host}, path{"/"}, generator{generatorFn()}, timeout{timeout} {
        clt.set_connection_timeout(timeout);
    }

    WebsiteUpDetector(const std::string& host, const std::string& path,
                      const chrono::seconds& timeout = 10s)
        : host{host}, clt{host}, path{path}, generator{generatorFn()}, timeout{timeout} {
        clt.set_connection_timeout(timeout.count());
    }

    [[nodiscard]] auto operator()() { return generator(); }

    [[nodiscard]] const auto& get_timeout() const { return timeout; }
    
    void set_timeout(const chrono::seconds& timeout) {
        this->timeout = timeout;
        clt.set_connection_timeout(timeout.count());
    }
};

}  // namespace UpDetector
