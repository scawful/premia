add_library(model-lib
    Model.hpp
    Model.cpp
)

target_link_libraries(model-lib
    controller-lib
)