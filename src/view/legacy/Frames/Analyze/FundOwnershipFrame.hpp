#ifndef FundOwnershipFrame_hpp
#define FundOwnershipFrame_hpp

#include "../Frame.hpp"

class FundOwnershipFrame : public Frame
{
private:
    bool data_active;
    ArrayList<iex::FundOwnership> fund_ownership;

public:

    FundOwnershipFrame();

    void update();
};

#endif