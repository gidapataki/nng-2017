#include "Hypno.h"


Hypno::Hypno()
{}

void Hypno::Process()
{
}

CLIENT* CreateClient()
{
	return new Hypno();
}

std::vector<int> Hypno::GetOurHeros() {
	std::vector<int> vec;
	for (auto& cc : mParser.Controllers) {
		if (cc.controller_id == 0) {
			vec.push_back(cc.hero_id);
		}
	}

	return vec;
}