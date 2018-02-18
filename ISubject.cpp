#include "ISubject.h"

#include <algorithm>

void ISubject::Attach( IObserver* obs )
{
    m_Obs.push_back( obs );
}

void ISubject::Detach( IObserver* obs )
{
    m_Obs.erase( std::find( m_Obs.begin(), m_Obs.end(), obs ) );
}

void ISubject::Notify( const std::string& cmd )
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

