#ifndef Indicators_hpp
#define Indicators_hpp

#include "Metatypes.hpp"
#include "Style.hpp"
#include "Library/ImPlot.hpp"

namespace Indicators 
{
    class Indicator {
    public:
        Indicator()=default;
        virtual ~Indicator()=default;

        virtual void importData(ArrayList<double> data) = 0;
        virtual void update() = 0;
    };

    class Volume 
        : public Indicator {
        ArrayList<double> volume;
        
    public:
        void importData(ArrayList<double> data);
        void update();
    };

    class BollingerBands {
    
    public:
        void importData();
        void update();
    };


}

#endif 