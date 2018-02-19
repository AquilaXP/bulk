#include "IState.h"

#include <memory>
#include <cassert>

IState::IState( ObserverBase* ob_base )
    : m_ob( ob_base )
{
    assert( ob_base );
}

void IState::ChangeState( AppenderCmd* context, IState* next_state )
{
    context->ChangeState( next_state );
}

void IState::AppendPackCmd( const std::string& cmd )
{
    m_ob->AppendPackCmd( cmd );
}

AppenderCmd::AppenderCmd( ObserverBase* ob_base, size_t N )
{
    m_states.emplace_back( std::unique_ptr<IState>( new StateWaitNCmd( ob_base, N ) ) );
    m_states.emplace_back( std::unique_ptr<IState>( new StateWaitEndBlock( ob_base ) ) );

    m_state = m_states[size_t( STATE_WAIT_N_CMD )].get();
}

void AppenderCmd::AppendCmd( const std::string& cmd )
{
    m_state->AppendCmd( this, cmd );
}

void AppenderCmd::ChangeState( IState* next_state )
{
    m_state = next_state;
}

IState* AppenderCmd::GetState( State_T id_state )
{
    return m_states[size_t( id_state )].get();
}

StateWaitEndBlock::StateWaitEndBlock( ObserverBase* ob_base )
    : IState( ob_base )
{
}

void StateWaitEndBlock::AppendCmd( AppenderCmd* context, const std::string& cmd )
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

void StateWaitEndBlock::Downlvl( AppenderCmd* context )
{
    --m_lvl;
    if( m_lvl == 0 )
    {
        if( not m_buffer.empty() )
        {
            AppendPackCmd( m_buffer );
            m_buffer.clear();
        }
        ChangeState( context, context->GetState(AppenderCmd::STATE_WAIT_N_CMD) );
        m_lvl = 1; // при следующем вызове
        m_num_cmd = 0;
    }
}

void StateWaitEndBlock::CopyCmd( const std::string& cmd )
{
    if( not m_buffer.empty() )
        m_buffer += ", ";
    m_buffer += cmd;
    m_ob->EventAddCmdToBlock( cmd, m_num_cmd );
    ++m_num_cmd;
}

StateWaitNCmd::StateWaitNCmd( ObserverBase* ob_base, size_t N )
    : IState( ob_base ), m_N(N)
{

}

StateWaitNCmd::~StateWaitNCmd()
{
    if( not m_buffer.empty() )
        AppendPackCmd( m_buffer );
}

void StateWaitNCmd::AppendCmd( AppenderCmd* conext, const std::string& cmd )
{
    if( cmd == "{" )
    {
        Flush();
        ChangeState( conext, conext->GetState(AppenderCmd::STATE_WAIT_END_BLOCK) );
    }
    else
    {
        if( not m_buffer.empty() )
            m_buffer += ", ";
        m_buffer += cmd;
        m_ob->EventAddCmdToBlock( cmd, m_num_cmd );
        ++m_num_cmd;

        if( m_num_cmd >= m_N )
        {
            Flush();
        }
    }
}

void StateWaitNCmd::Flush()
{
    if( m_buffer.empty() )
        return;

    AppendPackCmd( m_buffer );
    m_buffer.clear();
    m_num_cmd = 0;
}
