#pragma once

#include "IObserver.h"

class ObserverBase : public IObserver
{
public:
    virtual void EventAddCmdToBlock( const std::string& cmd, uint32_t num_cmd ) = 0;
    virtual void AppendPackCmd( const std::string& pack_cmd ) = 0;
};