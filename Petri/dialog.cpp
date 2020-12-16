#include"dialog.h"

void Prog5::menu()
{
	std::cout << "1. Connect Position to Transition" << std::endl;
	std::cout << "2. Connect Transition to Position" << std::endl;
	std::cout << "3. AddMarker" << std::endl;
	std::cout << "4. Start" << std::endl;
	std::cout << "5. Check Info" << std::endl;
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
		std::cout << "Enter Position id: ";
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
		std::cout << "Enter Position id: ";
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
		std::cout << "Enter Position id: ";
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
	ofstream file;
	file.open("info.txt", std::ofstream::out | std::ofstream::trunc);
	if (!file)
	{
		std::cout << "File not open" << std::endl;
		return;
	}
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
	oc.Start(count, file);
	file.close();
}

void Prog5::CheckInfo(Petri& oc)
{
	int count;
	bool flag = true;
	ifstream file("info.txt");
	if (!file)
	{
		std::cout << "File not open" << std::endl;
		return;
	}
	do {
		std::cout << "Enter Id Marker: ";
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
	string id_name = "Marker " + to_string(count) + ":";
	vector<string> info;
	string s;
	while (!file.eof())
	{
		getline(file, s);
		if (static_cast<int>(s.find(id_name)) >= 0)
			info.push_back(s);
		s.clear();
	}
	file.close();
	for (auto& t : info)
		std::cout << t << std::endl;
	info.clear();
}
