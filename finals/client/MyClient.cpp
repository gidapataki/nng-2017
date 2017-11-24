#include "stdafx.h"
#include "Client.h"
#include "parser.h"

// sample

class MYCLIENT : public CLIENT
{
public:
	MYCLIENT();
protected:
	virtual std::string GetPassword() { return std::string("c6gR92#1"); }
	virtual std::string GetPreferredOpponents() { return std::string("test"); }
	virtual bool NeedDebugLog() { return true; }
	virtual void Process();
};

MYCLIENT::MYCLIENT()
{
}

void MYCLIENT::Process()
{
}


CLIENT *CreateClient()
{
	return new MYCLIENT();
}
