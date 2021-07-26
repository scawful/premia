// core class
#ifndef core_hpp
#define core_hpp

#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <cmath>
#include <random>
#include <map>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <exception>
#include <ctime>
#include <sstream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <boost/make_shared.hpp>
#include <boost/tokenizer.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>

// #include <ql/quantlib.hpp>
// #include <ql/option.hpp>
// #include <ql/instruments/stock.hpp>
// #include <ql/quotes/all.hpp>

#include <curl/curl.h>

#include "Libraries/imgui/imgui.h"
#include "Libraries/imgui/imgui_sdl.h"
#include "Libraries/imgui/imgui_impl_sdl.h"
#include "Libraries/imgui/implot.h"
#include "Libraries/imgui/implot_internal.h"

#define SCREEN_WIDTH 782
#define SCREEN_HEIGHT 543

#endif