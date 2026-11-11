#include "server/config.hpp"
#include <string_view>
#include <charconv>
#include <string>
namespace server
{
    unsigned int ParseNumber(std::string_view value, unsigned int maximum){
        unsigned int number =0;
        const auto [ptr,error] = std::from_chars(value.data(), value.data()+value.size(), number);
        if (number==0 || number>maximum){
            throw std::invalid_argument("invalid argument: " + std::string(value));
        }
        return number;
    }
    Config Parse(int argc, char* argv[]){
        Config config;
        for (int i=0;i<argc;i++){
            std::string_view argument = argv[i];
            if (argument!="--port" && argument!="--threads"){
                throw std::invalid_argument("unknown argument: " + std::string(argument));
            }
            if (i+1>=argc){
                throw std::invalid_argument("miss value for: " + std::string(argument));
            }
            const std::string_view value = argv[++i];
            if (argument == "--port"){
              config.port = static_cast<uint16_t>(ParseNumber(value, 65535));
            }
            if (argument == "--threads"){
              config.threads =
                  ParseNumber(value, std::numeric_limits<unsigned int>::max());
            }
        }
        return config;
    }
} // namespace server
