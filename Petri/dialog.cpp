#include"dialog.h"

void Prog5::menu()
{
	std::cout << "1. Connect Platform to Transition" << std::endl;
	std::cout << "2. Connect Transition to Platform" << std::endl;
	std::cout << "3. AddMarker" << std::endl;
	std::cout << "4. Start" << std::endl;
}

void Prog5::ConnectPT(Petri& oc)
{
	int tid;
	int pid;
	int count;
	bool flag = true;
	do {
		std::cout << "Enter Transition id: ";
		std::cin >> tid;
		std::cout << "Enter Platform id: ";
		std::cin >> pid;
		std::cout << "Enter Count: ";
		std::cin >> count;
		if (!std::cin.good()) {
			std::cin.clear();
			std::cin.ignore(INT_MAX, '\n');
			flag = true;
		}
		else
			flag = false;
		std::cout << std::endl;
	} while (flag || pid< 0 || tid < 0 || count<0);
	oc.ConnectPT(pid, tid, count);
}
void Prog5::ConnectTP(Petri& oc)
{
	int tid;
	int pid;
	int count;
	bool flag = true;
	do {
		std::cout << "Enter Transition id: ";
		std::cin >> tid;
		std::cout << "Enter Platform id: ";
		std::cin >> pid;
		std::cout << "Enter Count: ";
		std::cin >> count;
		if (!std::cin.good()) {
			std::cin.clear();
			std::cin.ignore(INT_MAX, '\n');
			flag = true;
		}
		else
			flag = false;
		std::cout << std::endl;
	} while (flag || pid < 0 || tid < 0 || count < 0);
	oc.ConnectTP(tid, pid, count);
}

void Prog5::AddMarker(Petri& oc)
{
	int pid;
	int count;
	bool flag = true;
	do {
		std::cout << "Enter Platform id: ";
		std::cin >> pid;
		std::cout << "Enter Count: ";
		std::cin >> count;
		if (!std::cin.good()) {
			std::cin.clear();
			std::cin.ignore(INT_MAX, '\n');
			flag = true;
		}
		else
			flag = false;
		std::cout << std::endl;
	} while (flag || pid < 0 || count < 0);
	oc.AddMarker(pid, count);
}

void Prog5::Start(Petri& oc)
{
	int count;
	bool flag = true;
	do {
		std::cout << "Enter Timer: ";
		std::cin >> count;
		if (!std::cin.good()) {
			std::cin.clear();
			std::cin.ignore(INT_MAX, '\n');
			flag = true;
		}
		else
			flag = false;
		std::cout << std::endl;
	} while (flag || count < 0);
	oc.Start(count);
}
