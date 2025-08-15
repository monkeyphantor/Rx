#pragma once
#include "Global.hpp"

namespace Rx {
namespace Asset{

        template<typename T>
        inline void loadData(std::vector<char>& data, T& value, uint64_t& offset){
            value = *reinterpret_cast<T*>(data.data() + offset);
            offset += sizeof(T);
        }

        inline void loadString(std::vector<char>& data, char* value, uint32_t multiplicity, uint64_t& offset){
            memcpy(value, data.data() + offset, multiplicity);
            offset += multiplicity;
        }

    }
}