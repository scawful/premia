// core class
#ifndef core_hpp
#define core_hpp

// C++ Standard Library
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>
#include <array>
#include <cmath>
#include <random>
#include <map>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <exception>
#include <ctime>
#include <sstream>
#include <filesystem>
#include <functional>
#include <memory>
#include <thread>
#include <iomanip>
#include <mutex>

// SDL2
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

// Boost 
#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/make_shared.hpp>
#include <boost/tokenizer.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
#define BOOST_ASIO_ENABLE_HANDLER_TRACKING

// OpenSSL
#include <openssl/ssl.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>

// QuantLib
// #include <ql/quantlib.hpp>
// #include <ql/option.hpp>
// #include <ql/instruments/stock.hpp>
// #include <ql/quotes/all.hpp>

// libcURL
#include <curl/curl.h>

// ImGui, ImGuiSDL, ImPlot
#include "../include/imgui/imgui.h"
#include "../include/imgui/backends/imgui_impl_sdl.h"
#include "../include/imgui/backends/imgui_impl_sdlrenderer.h"
#include "../include/implot/implot.h"
#include "../include/implot/implot_internal.h"

#define SCREEN_WIDTH 782
#define SCREEN_HEIGHT 543

#endif