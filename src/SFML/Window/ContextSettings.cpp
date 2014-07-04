////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2014 Laurent Gomila (laurent.gom@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Window/ContextSettings.hpp>
#include <SFML/System/Err.hpp>


namespace sf
{
////////////////////////////////////////////////////////////
ContextSettings::ContextSettings(unsigned int depth, unsigned int stencil, unsigned int antialiasing, unsigned int major, unsigned int minor, bool compatibility, bool debug) :
depthBits        (depth),
stencilBits      (stencil),
antialiasingLevel(antialiasing),
majorVersion     (major),
minorVersion     (minor),
compatibilityFlag(compatibility),
debugFlag        (debug)
{
    // Perform sanity checks
    if ((majorVersion == 3) && ((minorVersion == 0) || (minorVersion == 1)))
    {
        if (compatibilityFlag)
        {
            err() << "Compatibility profile does not exist for version "
                  << majorVersion << "." << minorVersion
                  << ", attempting to create a core profile context instead" << std::endl;

            compatibilityFlag = false;
        }
    }
}

} // namespace sf
