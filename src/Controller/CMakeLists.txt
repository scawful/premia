add_library(controller-lib
    Controller.hpp
    Controller.cpp
)

target_link_libraries(controller-lib
    model-lib
    ImPlot
)