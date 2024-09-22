#include "string.hh"

int main() {
    String s("你好，世界!");
    std::cout << s << std::endl;
    for (auto i : s) std::cout << i << "\n";
    std::cout << "\n\n";
    std::cout << s.substr(0, 2) << "\n" << s.substr(3) << "\n";
    std::cout << s[1] << "\n\n\n\n\n\n";

    return 0;
}