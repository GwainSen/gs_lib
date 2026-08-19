#pragma once

#include <cstdint>
#include <ostream>

namespace gs {
    class uint512_t {
    private:
        std::uint64_t segments[8]; // 64*8 = 512

        void internal_addition(const std::uint8_t current_segment, const std::uint64_t number) {
            if (const std::uint64_t missing_to_limit = ( 0-1 ) - segments[current_segment]; missing_to_limit >= number) {
                segments[current_segment] += number;
            }
            else {
                segments[current_segment] = number - missing_to_limit;
                ++segments[current_segment + 1];
            }
        }

    public:
        /*Default Constructor fills with zeros*/
        uint512_t(): segments{0} {};

        //no idea if this can be moved to private or how

        /*Equals operation overload*/
        uint512_t& operator=(const int& number){
            for (auto& segment : this->segments)
                segment = 0;
            this->internal_addition(0, number);
            return *this;
        }

        /*Addition operator overload*/
        uint512_t operator+(const uint512_t& number) const {
            uint512_t output;
            for (std::int8_t i = 0; i < 8; i++)
                output.internal_addition(i, number.segments[i]);
            return output;
        }
        uint512_t operator+(const int& number) const {
            uint512_t output;
            output.internal_addition(0, number);
            return output;
        }

        /*To string conversion*/
        [[nodiscard]] std::string to_string() const {
            std::string output;
            for (std::int8_t i = 7; i > 0; i--)
                output += std::to_string(segments[i]) + " ";
            output += std::to_string(segments[0]);
            return output;
        }
    };
}