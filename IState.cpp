#include "IState.h"

#include <memory>
#include <cassert>

IState::IState( ISubject* subject ) 
    : m_subject( subject )
{
    assert( subject );
}

void IState::ChangeState( Context* context, IState* next_state )
{
    context->ChangeState( next_state );
}

void IState::AppendCmdToSubject( const std::string& cmd )
{
    m_subject->Notify( cmd );
}

Context::Context( ISubject* subject, size_t N )
    : m_subject( subject )
{
    m_states.emplace_back( std::make_unique<StateWaitNCmd>( subject, N ) );
    m_states.emplace_back( std::make_unique<StateWaitEndBlock>( subject ) );

    m_state = m_states[size_t( STATE_WAIT_N_CMD )].get();
}

void Context::AppendCmd( const std::string& cmd )
{
    m_state->AppendCmd( this, cmd );
}

void Context::ChangeState( IState* next_state )
{
    m_state = next_state;
}

IState* Context::GetState( State_T id_state )
{
    return m_states[size_t( id_state )].get();
}

StateWaitEndBlock::StateWaitEndBlock( ISubject* subject ) 
    : IState( subject )
{

}

void StateWaitEndBlock::AppendCmd( Context* context, const std::string& cmd )
{
    if( cmd == "{" )
    {
        Uplvl();
    }
    else if( cmd == "}" )
    {
        Downlvl( context );
    }
    else
    {
        CopyCmd( cmd );
    }
}

void StateWaitEndBlock::Uplvl()
{
    ++m_lvl;
}

void StateWaitEndBlock::Downlvl( Context* context )
{
    --m_lvl;
    if( m_lvl == 0 )
    {
        AppendCmdToSubject( m_buffer );
        ChangeState( context, context->GetState(Context::STATE_WAIT_N_CMD) );
        m_lvl = 1; // при следующем вызове
    }
}

void StateWaitEndBlock::CopyCmd( const std::string& cmd )
{
    if( not m_buffer.empty() )
        m_buffer += ", ";
    m_buffer += cmd;
}

StateWaitNCmd::StateWaitNCmd( ISubject* subject, size_t N )
    : IState( subject ), m_N(N)
{

}

StateWaitNCmd::~StateWaitNCmd()
{
    if( not m_buffer.empty() )
        AppendCmdToSubject( m_buffer );
}

void StateWaitNCmd::AppendCmd( Context* conext, const std::string& cmd )
{
    if( cmd == "{" )
    {
        Flush();
        ChangeState( conext, conext->GetState(Context::STATE_WAIT_END_BLOCK) );
    }
    else
    {
        if( not m_buffer.empty() )
            m_buffer += ", ";
        m_buffer += cmd;
        ++m_curr;
        if( m_curr >= m_N )
        {
            Flush();
        }
    }
}

void StateWaitNCmd::Flush()
{
    if( m_buffer.empty() )
        return;

    AppendCmdToSubject( m_buffer );
    m_buffer.clear();
    m_curr = 0;
}
