#ifndef FundOwnershipFrame_hpp
#define FundOwnershipFrame_hpp

#include "../Frame.hpp"

class FundOwnershipFrame : public Frame
{
private:
    bool data_active;
    std::vector<iex::FundOwnership> fund_ownership;

public:

    FundOwnershipFrame();

    void Update();
};

#endif