#pragma once

#include <vector>
#include <string>

#include "IObserver.h"

class ISubject
{
public:
    virtual ~ISubject() = default;

    void Attach( IObserver* obs );
    void Detach( IObserver* obs );
    virtual void Notify( const std::string& cmd );

protected:
    std::vector<IObserver*> m_Obs;
};