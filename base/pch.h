//
//  pch.h
//  SoftwareRenderer
//
//  Created by Duc Ho on 2/12/17.
//  Copyright © 2017 ducho. All rights reserved.
//

#ifndef pch_h
#define pch_h
#include <iostream>
#include <string>
#ifdef _WIN32
    //define something for Windows (32-bit and 64-bit, this part is common)
    #define INFILE  "..\\Resources\\ppot.asc"
    #define OUTFILE "..\\Output\\output.ppm"
    #define SEPARATOR '\'
#else //*nix
    #define INFILE  "../Resources/ppot.asc"
    #define OUTFILE "../Output/output.ppm"
    #define SEPARATOR '/'
    // Other kinds of Mac OS
#endif /* pch_h */
#endif
