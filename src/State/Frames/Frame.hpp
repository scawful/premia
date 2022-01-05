#ifndef Frame_hpp
#define Frame_hpp

class Frame 
{
private:

public:
    Frame();

    virtual void update() = 0;
    virtual void render() = 0;

};

#endif