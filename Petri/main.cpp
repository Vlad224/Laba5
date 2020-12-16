#include"dialog.h"
using namespace Prog5;
int main() {
	void(*dp[10])(Petri & oc) = { nullptr, ConnectPT,ConnectTP,AddMarker,Start,CheckInfo };
	int tid_count;
	int pid_count;
	bool flag = true;
	do {
		std::cout << "Enter Amount Transition: ";
		std::cin >> tid_count;
		std::cout << "Enter Amount Position: ";
		std::cin >> pid_count;
		if (!std::cin.good()) {
			std::cin.clear();
			std::cin.ignore(INT_MAX, '\n');
			flag = true;
		}
		else
			flag = false;
		std::cout << std::endl;
	} while (flag || pid_count < 0 || tid_count < 0);	
	Petri petri(pid_count, tid_count);
	int fl1 = 1;
	int c;
	while (fl1) {
		menu();
		std::cin >> c;
		if (!std::cin.good() || c < 0 || c > 6) {

			std::cin.clear();

			std::cin.ignore(INT_MAX, '\n');

			std::cout << "Incorrect input!" << std::endl;
		}
		else
			if (c == 0)
				fl1 = 0;
			else
				dp[c](petri);

	}
}