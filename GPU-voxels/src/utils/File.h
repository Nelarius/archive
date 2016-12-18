#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

#include <string>
#include <sstream>
#include <fstream>
#include <sys/stat.h>

namespace pg {

    /// @brief Check whether a file of the given name exists.
    inline bool fileExists( const std::string& file ) {
        struct stat buffer;
        return ( stat( file.c_str(), &buffer) == 0 );
    }

    /// @brief Get the contents of a file as a string.
    inline std::string fileToString( const std::string& file ) {
        std::ifstream fin;
        fin.open( file, std::ios::in );
        if ( !fileExists( file ) ) {
            throw std::exception( (std::string("File not found: ") + file).c_str() );
        }

        std::stringstream buffer;
        buffer << fin.rdbuf() << '\0';
        return buffer.str();
    }

}

#endif // FILE_H_INCLUDED
