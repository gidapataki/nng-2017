#pragma once
#include "Client.h"
#include "parser.h"
#include <vector>
#include <string>


class Hypno : public CLIENT
{
public:
	Hypno();

protected:
	virtual std::string GetPassword() { return std::string("c6gR92#1"); }
	virtual std::string GetPreferredOpponents() { return std::string("test"); }
	virtual bool NeedDebugLog() { return true; }
	virtual void Process();

	std::vector<int> GetOurHeroes();
};
