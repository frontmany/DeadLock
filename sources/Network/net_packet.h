#pragma once

#include <vector>     
#include <cstdint>     
#include <string>    
#include <stdexcept>   

namespace net {
    class Packet {
    private:
        struct PacketHeader {
            uint32_t type = 0;
            uint32_t size = 0;
        };

    public:
        uint32_t size() const;
        static size_t sizeOfHeader();
        void add(const std::string& str);
        std::string get();
        void clear();

        uint32_t type() const;
        void setType(uint32_t type);

        const PacketHeader& header() const;
        PacketHeader& header_mut();

        const std::vector<uint8_t>& body() const;
        std::vector<uint8_t>& body_mut();

    private:
        size_t calculateSize() const;
        void putSize(uint32_t value);
        void extractSize(uint32_t& value);

    private:
        PacketHeader m_header{};
        std::vector<uint8_t> m_body{};
    };
}