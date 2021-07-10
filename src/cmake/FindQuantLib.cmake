# - Find the QuantLib Library
#
# This module defines the following variables:
#
#  QuantLib_INCLUDE_DIR, where to find ql/quantlib.h
#  QuantLib_LIBRARY_DIR, where to find the QuantLib library.
#  QuantLib_LIBRARIES, the libraries (no flags!) to link with.
#  QuantLib_FOUND, if false, do not try to use QuantLib
#
#---------------------------------------------------------------------

# create a prefix from the path of the current file
string ( REPLACE "FindQuantLib.cmake" "../.." prefix ${CMAKE_CURRENT_LIST_FILE} )
# [for some reason this does not work: set(prefix "../..")]
#message ( STATUS "prefix=${prefix}" )

set ( QuantLib_NAMES QuantLib QuantLib-1.2 quantlib quantlib-1.2 )

if ( UNIX )

    set ( home "$ENV{HOME}" )

    set ( unixpaths
        ${prefix}
        ${home}
        ${home}/local
        /opt
        /usr/local
        /usr
    )

    set ( subpaths )
    foreach ( p ${unixpaths} )
        foreach ( n ${QuantLib_NAMES} )
            list ( APPEND subpaths "${p}/${n}" )
        endforeach ()
    endforeach ()
    list ( APPEND unixpaths ${subpaths} )

    set ( includepaths )
    set ( libpaths )
    foreach ( p ${unixpaths} )
        list ( APPEND includepaths "${p}/include" )
        list ( APPEND libpaths "${p}/lib" )
    endforeach ()

elseif ( WIN32 )

    set ( programfiles "$ENV{ProgramFiles}" )
    set ( programfilesx86 "$ENV{ProgramFiles(x86)}" )

    set ( windowspaths )
    foreach ( p ${QuantLib_NAMES} )
        list ( APPEND windowspaths "${programfiles}/${p}" )
        list ( APPEND windowspaths "${programfilesx86}/${p}" )
    endforeach ()

    set ( includepaths ${windowspaths} )
    set ( libpaths ${windowspaths} )

    foreach ( p ${windowspaths} )
        list ( APPEND libpaths "${p}/QuantLib" )
    endforeach ()

endif ()


#foreach ( p ${includepaths} )
#  message ( STATUS "includepaths: ${p}" )
#endforeach ()
#foreach ( p ${libpaths} )
#  message ( STATUS "libpaths: ${p}" )
#endforeach()

find_path ( QuantLib_INCLUDE_DIR ql/quantlib.hpp PATHS ${includepaths} )

find_library ( QuantLib_LIBRARY NAMES ${QuantLib_NAMES} PATHS ${libpaths} )
set ( QuantLib_LIBRARY_DIR ${QuantLib_LIBRARY} CACHE FILEPATH "The QuantLib library" )

if ( QuantLib_LIBRARY AND QuantLib_INCLUDE_DIR )
   set ( QuantLib_INCLUDE_DIR ${QuantLib_INCLUDE_DIR} CACHE FILEPATH "The directory where ql/quantlib.hpp is located" )
   # do platform stuff in here
endif ( QuantLib_LIBRARY AND QuantLib_INCLUDE_DIR )

# handle the QUIETLY and REQUIRED arguments and set QuantLib_FOUND to TRUE if
# all listed variables are TRUE
include ( FindPackageHandleStandardArgs )
find_package_handle_standard_args ( QuantLib DEFAULT_MSG QuantLib_LIBRARY QuantLib_INCLUDE_DIR )

mark_as_advanced ( QuantLib_LIBRARY )

if ( QuantLib_LIBRARY ) 
   # because cmake automatically exports QUANTLIB_FOUND
   # and not the lower case QuantLib_FOUND
   set ( QuantLib_FOUND TRUE )
endif ()

# - END

