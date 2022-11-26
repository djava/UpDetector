#include "UpDetector.hpp"

using namespace UpDetector;

int main() {
    WebsiteUpDetector vfGen ("http://vexforum.com");

    std::cout << vfGen() << std::endl;
    std::cout << vfGen() << std::endl;
    std::cout << vfGen() << std::endl;
}