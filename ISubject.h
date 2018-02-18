#pragma once

#include <vector>
#include <string>

#include "IObserver.h"

class ISubject
{
public:
    virtual ~ISubject() = default;

    void Attach( IObserver* obs )
    {
        m_Obs.push_back( obs );
    }
    void Detach( IObserver* obs )
    {
        m_Obs.erase( std::find( m_Obs.begin(), m_Obs.end(), obs ) );
    }
    virtual void Notify( const std::string& cmd )
    {
        for( auto& o : m_Obs )
        {
            try
            {
                o->Update( cmd );
            }
            catch( ... )
            {
                // кому-то сообщаем
            }
        }
    }

protected:
    std::vector<IObserver*> m_Obs;
};