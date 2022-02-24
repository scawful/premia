#ifndef FundOwnershipFrame_hpp
#define FundOwnershipFramee_hpp

#include "Frame.hpp"

class FundOwnershipFrame : public Frame
{
private:
    bool data_active;
    std::vector<iex::FundOwnership> fund_ownership;

public:

    FundOwnershipFrame();
    ~FundOwnershipFrame();

    void update();
};

#endif