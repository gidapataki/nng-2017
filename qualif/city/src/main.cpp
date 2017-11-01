#include <iostream>
#include <string>


void readMap() {
	int n;
	std::cin >> n;
	for (int i = 0; i < n; ++i) {
		std::string line;
		std::cin >> line;
	}

	int k;
	int g, w, e;

	std::cin >> k;
	for (int i = 0; i < k; ++i) {
		std::cin >> g >> w >> e;
	}

	std::cout << n << std::endl;
	std::cout << k << std::endl;
	std::cout << g << std::endl;
	std::cout << w << std::endl;
	std::cout << e << std::endl;
}


int main() {
	readMap();
	return 0;
}
