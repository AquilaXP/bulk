#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <chrono>
#include "IState.h"
#include "IObserver.h"

/// Наблюдатель выводящий в консоль
class ConsoleObserver : public IObserver
{
public:    
    void Update( const std::string& cmd ) override
    {
        std::cout << "bulk: " << cmd << std::endl;
    }
};

/// Наблюдатель сохраняющий в файл
class FileObserver : public IObserver
{
public:
    void Update( const std::string& cmd ) override
    {
        namespace sc = std::chrono;
        auto t = sc::system_clock::now();
        std::stringstream ss;
        ss << 
            "bulk" << 
            sc::duration_cast<sc::seconds>(t.time_since_epoch()).count() << 
            ".log";
        std::ofstream f( ss.str(), std::ios::binary );
        f << "bulk: " << cmd;
    }
};

/// Издатель
class ConsoleSubject : public ISubject
{
public:
    ConsoleSubject( int32_t N ) 
        : m_context( this, N )
    {
    }
    void Run()
    {      
        std::string cmd;
        while( true )
        {
            std::getline( std::cin, cmd );
            if( cmd.empty() )
                return;
            m_context.AppendCmd( cmd );
        }
    }
private:
    AppenderCmd m_context;
};


int main( int ac, char* av[] )
{
    if( ac != 2 )
    {
        std::cout << "Not set size block command!\n";
        return 1;
    }
    int32_t N = std::atoi(av[1]);

    ConsoleObserver co;
    FileObserver fo;
    ConsoleSubject cs( N );
    
    cs.Attach( &co );
    cs.Attach( &fo );
    cs.Run();

    return 0;
}