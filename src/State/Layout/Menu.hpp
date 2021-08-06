#ifndef Layout_Menu_hpp
#define Layout_Menu_hpp

#include "../../core.hpp"
#include "../Manager.hpp"

void draw_imgui_menu( Manager *premia, boost::shared_ptr<tda::TDAmeritrade> tda_data_interface, std::string title_string );

#endif