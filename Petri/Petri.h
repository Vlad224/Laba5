#pragma once
#ifndef _PETRI_H_
#define _PETRI_H_
#include <iostream>
#include <vector>
#include <queue>
#include <time.h>
#include <random>
#include<thread>
#include <chrono>
#include <mutex>
#include<string>
#include <atomic>
#include<fstream>
using namespace std;
namespace Prog5 {
	struct Transition
	{
		vector<int> position_exit;
		vector<int> position_entrance;
		Transition() {};
	};
	struct Position {
		vector<int> transition_exit;
		int markers;
		Position() : markers(0) {};
	};
	struct Marker {
		int id;
		int Position;
		vector<int> AvailibleTransitions;
		std::chrono::milliseconds Current_Duration;
		std::chrono::time_point<std::chrono::system_clock> Timer;
		Marker(int pl);
	};
	class Petri {
	private:
		vector<Position> position;
		vector<Transition> transition;
		vector<Marker> marker;
		vector<int> marker_q;
		std::mutex lock;
		std::mutex lock_marker;
		std::mutex lock_activ;
		std::mutex lock_que;
		std::condition_variable not_full;
		std::condition_variable que;
		std::condition_variable activate;
		std::chrono::milliseconds Current_Duration;
		std::chrono::time_point<std::chrono::system_clock> Timer;
	public:
		Petri(int size_position, int size_transition);
		void ConnectPT(int pid, int tid, int count = 1);
		void ConnectTP(int tid, int pid, int count = 1);
		void AddMarker(int pid, int count = 1);
		void MainFunc(ofstream& file);
		void UpdateMarker();
		void UpdateTransitions(ofstream& file);
		void ActiveTransition(int i, ofstream& file);
		void Start(int s,ofstream& file);
	};
}
#endif // !_PETRI_H_
