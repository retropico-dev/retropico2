//
// Created by cpasjuste on 29/11/24.
//

#ifndef UTILITY_H
#define UTILITY_H

#include <string>

namespace retropico {
    class Utility {
    public:
        static bool Unzip(const std::string &src, const std::string &dst);
    };
}

#endif //UTILITY_H
