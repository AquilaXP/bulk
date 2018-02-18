#pragma once

#include <string>
#include <vector>

class IObserver
{
public:
    virtual ~IObserver() = default;
    virtual void Update( const std::string& cmd ) = 0;
};
