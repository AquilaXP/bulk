#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <chrono>

#include "ISubject.h"
#include "IState.h"
#include "ObserverBase.h"

/// Наблюдатель выводящий в консоль
class ConsoleObserver : public ObserverBase
{
public:
    ConsoleObserver( size_t N ) 
        : m_ac(this, N)
    {}
    void Update( const std::string& cmd ) override
    {
        m_ac.AppendCmd( cmd );
    }
    void EventAddCmdToBlock( const std::string& cmd, uint32_t num_cmd ) override
    {

    }
    void AppendPackCmd( const std::string& pack_cmd )
    {
        std::cout << "bulk: " << pack_cmd << std::endl;
    }
private:
    AppenderCmd m_ac;
};

/// Наблюдатель сохраняющий в файл
class FileObserver : public ObserverBase
{
public:
    FileObserver( size_t N )
        : m_ac( this, N )
    {}
    void Update( const std::string& cmd ) override
    {
        m_ac.AppendCmd( cmd );
    }
    void EventAddCmdToBlock( const std::string& cmd, uint32_t num_cmd ) override
    {
        if( num_cmd == 0 )
        {
            namespace sc = std::chrono;
            auto t = sc::system_clock::now();
            std::stringstream ss;
            ss <<
                "bulk" <<
                sc::duration_cast<sc::seconds>( t.time_since_epoch() ).count() <<
                ".log";
            m_name_file = ss.str();
        }        
    }
    void AppendPackCmd( const std::string& pack_cmd )
    {
        std::ofstream f( m_name_file, std::ios::binary );
        f << "bulk: " << pack_cmd << std::endl;
        m_name_file.clear();
    }
private:
    std::string m_name_file;
    AppenderCmd m_ac;
};

/// Издатель
class ConsoleSubject : public ISubject
{
public:
    void Run()
    {      
        std::string cmd;
        while( std::getline( std::cin, cmd ) )
        {
            if( cmd.empty() )
                return;
            Notify( cmd );
        }
    }
};


int main( int ac, char* av[] )
{
    if( ac != 2 )
    {
        std::cout << "Not set size block command!\n";
        return 1;
    }
    int32_t N = std::atoi(av[1]);

    ConsoleObserver co( N );
    FileObserver fo( N );
    ConsoleSubject cs;
    
    cs.Attach( &co );
    cs.Attach( &fo );
    cs.Run();

    return 0;
}