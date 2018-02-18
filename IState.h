#pragma once

#include <ciso646>

#include <string>
#include <vector>
#include <memory>

#include "ISubject.h"

class Context;

class IState
{
public:
    IState( ISubject* subject );
    virtual ~IState() = default;
    virtual void AppendCmd( Context* context, const std::string& cmd ) = 0;
protected:
    void ChangeState( Context* context, IState* next_state );
    void AppendCmdToSubject( const std::string& cmd );

    ISubject* m_subject = nullptr;
};

class Context
{
    friend class IState;
public:
    enum State_T
    {
        STATE_WAIT_N_CMD = 0,
        STATE_WAIT_END_BLOCK = 1
    };

    Context( ISubject* subject, size_t N );

    void AppendCmd( const std::string& cmd );
    void ChangeState( IState* next_state );
    IState* GetState( State_T id_state );
private:
    std::vector< std::unique_ptr<IState> > m_states;
    IState* m_state = nullptr;
    ISubject* m_subject = nullptr;
};

/// Состояние с блоками комманд
class StateWaitEndBlock : public IState
{
public:
    StateWaitEndBlock( ISubject* subject );
    void AppendCmd( Context* context, const std::string& cmd ) override;
private:
    void Uplvl();
    void Downlvl( Context* context );
    void CopyCmd( const std::string& cmd );

    int32_t m_lvl = 1;
    std::string m_buffer;
};

/// Состояние с ожиданием N комманд
class StateWaitNCmd : public IState
{
public:
    StateWaitNCmd( ISubject* subject, size_t N );
    ~StateWaitNCmd();
    void AppendCmd( Context* conext, const std::string& cmd ) override;
private:
    void Flush();

    int32_t m_N = 0;    
    int32_t m_curr = 0;
    std::string m_buffer;
};