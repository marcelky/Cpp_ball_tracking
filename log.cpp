/*TIPS
 * In case the compilation of logs fail, add the following configuration to
 * Project -> Properties -> GCC++ builder ->
 *
 * C/C++ Build -> C/C++Build -> Settings -> GCC C++ Linker -> General ->
	Set Support for pthread (-pthread)
 *
 *
 * This I guess is not necessary because it is working without set it:
 * C/C++ Build -> C/C++Build -> Settings -> GCC C++ Compiler -> Miscellaneous ->
	Set Support for pthread (-pthread)
	At Other flags add "-lz". It will looks like this:
	Other flags:   "-c -fmessage-length=0 -lz"
 */


#include "log.hpp"
#include <cassert>
#include "log.hpp"

namespace logging
{		

void file_log_policy::open_out_stream(const std::string& name)
{
    out_stream.open( name.c_str(), std::ios_base::binary|std::ios_base::out );
    assert( out_stream.is_open() == true );
    out_stream.precision( 20 );
}

void file_log_policy::close_out_stream()
{
    if( out_stream )
    {
        out_stream.close();
    }
}

void file_log_policy::write(const std::string& msg)
{
    out_stream<<msg<<std::endl<<std::flush;
}
}

