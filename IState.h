#pragma once

#include <ciso646>

#include <string>
#include <vector>
#include <memory>

#include "ObserverBase.h"

class AppenderCmd;

/// Базовый класс состояния
class IState
{
public:
    IState( ObserverBase* ob_base );
    virtual ~IState() = default;
    virtual void AppendCmd( AppenderCmd* context, const std::string& cmd ) = 0;
protected:
    void ChangeState( AppenderCmd* context, IState* next_state );
    void AppendPackCmd( const std::string& cmd );
    void EventAddCmdtoBlock( const std::string& cmd, uint32_t num_cmd )
    {

    }

    uint32_t m_num_cmd = 0;
    ObserverBase* m_ob = nullptr;
};

/// Класс для добавления комманд.
/// Может быть в двух состояниях:
/// 1 - накапливать N команд, после отсылать.
/// 2 - накапливать блок команд между символами {}.
class AppenderCmd
{
    friend class IState;
public:
    enum State_T
    {
        STATE_WAIT_N_CMD = 0,
        STATE_WAIT_END_BLOCK = 1
    };

    AppenderCmd( ObserverBase* ob_base, size_t N );
    AppenderCmd( const AppenderCmd& ) = delete;
    AppenderCmd& operator = ( const AppenderCmd& ) = delete;

    void AppendCmd( const std::string& cmd );
    void ChangeState( IState* next_state );
    IState* GetState( State_T id_state );
private:
    std::vector< std::unique_ptr<IState> > m_states;
    IState* m_state = nullptr;
};

/// Состояние с блоками {} команд
class StateWaitEndBlock : public IState
{
public:
    StateWaitEndBlock( ObserverBase* ob_base );
    void AppendCmd( AppenderCmd* context, const std::string& cmd ) override;
private:
    void Uplvl();
    void Downlvl( AppenderCmd* context );
    void CopyCmd( const std::string& cmd );

    int32_t m_lvl = 1;
    std::string m_buffer;
};

/// Состояние с ожиданием N комманд
class StateWaitNCmd : public IState
{
public:
    StateWaitNCmd( ObserverBase* ob_base, size_t N );
    ~StateWaitNCmd();
    void AppendCmd( AppenderCmd* conext, const std::string& cmd ) override;
private:
    void Flush();

    int32_t m_N = 0;    
    std::string m_buffer;
};