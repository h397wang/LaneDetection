// main.cpp
#include <string.h>

#include "utils.hpp"

using namespace std;

int main( int argc, char* argv[] )
{
    const int vNumExpArgs = 2;
    if ( argc != vNumExpArgs )
    {
        cerr << "Invalid number of args. Expected " << vNumExpArgs << ". Got " << argc << endl;
        cerr << "./ProgramName <FilePath>" << endl;
        return -1;
    }

    string vFilePathString = argv[1];

    if ( vFilePathString.find( ".mp4" ) <= vFilePathString.length() )
    {
        processVideo( argv[1] );
    }
    else if ( vFilePathString.find( ".jpg" ) <= vFilePathString.length() )
    {
        processStaticImage( argv[1] );
    } else {
        cerr << "Invalid file name extension. Expected: .mp4, .jpg" << endl;
        return -1;
    }

    return 0;
}
