#pragma once
#include <vector>     
#include <cstdint>     
#include <cstring>  
#include <string>    
#include <type_traits> 
#include <stdexcept>   

namespace net {
    struct PacketHeader {
        uint32_t type = 0;
        uint32_t size = 0;
    };


    struct Packet {
        PacketHeader header{};
        std::vector<uint8_t> body{};

        size_t size() const {
            return sizeof(PacketHeader) + body.size();
        }

        template <typename DataType>
        friend Packet& operator << (Packet& msg, const DataType& data) {
            static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed");

            size_t i = msg.body.size();
            msg.body.resize(msg.body.size() + sizeof(DataType));
            memcpy(msg.body.data() + i, &data, sizeof(DataType));
            msg.header.size = msg.size();

            return msg;
        }

        friend Packet& operator << (Packet& msg, const std::string& str) {
            msg.body.insert(msg.body.end(), str.begin(), str.end());

            uint32_t size = static_cast<uint32_t>(str.size());
            msg << size;

            msg.header.size = msg.size();

            return msg;
        }

        template <typename DataType>
        friend Packet& operator >> (Packet& msg, DataType& data) {
            static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pulled");

            if (msg.body.size() < sizeof(DataType))
                throw std::runtime_error("Packet body too small for data type");

            size_t i = msg.body.size() - sizeof(DataType);
            memcpy(&data, msg.body.data() + i, sizeof(DataType));
            msg.body.resize(i);
            msg.header.size = msg.size();

            return msg;
        }

        friend Packet& operator >> (Packet& msg, std::string& str) {
            uint32_t size = 0;
            msg >> size;

            if (msg.body.size() < size)
                throw std::runtime_error("Packet body too small for string");

            str.assign(reinterpret_cast<const char*>(msg.body.data()), size);
            msg.body.erase(msg.body.begin(), msg.body.begin() + size);
            msg.header.size = msg.size();

            return msg;
        }
    };
}