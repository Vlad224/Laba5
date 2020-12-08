#include"Petri.h"
namespace Prog5 {
	int ID = 0;
	Petri::Petri(int size_position, int size_transition)
	{
		for (int i = 0; i < size_position; i++) {
			position.push_back(Position());
			for (int j = 0; j < size_transition; j++)
				position[i].transition_exit.push_back(0);
		}
		for (int j = 0; j < size_transition; j++) {
			transition.push_back(Transition());
			for (int i = 0; i < size_position; i++) {
				transition[j].position_entrance.push_back(0);
				transition[j].position_exit.push_back(0);
			}
		}
	}
	void Petri::ConnectPT(int pid, int tid, int count)
	{
		position[pid].transition_exit[tid] += count;
		transition[tid].position_entrance[pid] += count;
	}
	void Petri::ConnectTP(int tid, int pid, int count)
	{
		transition[tid].position_exit[pid] += count;
	}
	void Petri::AddMarker(int pid, int count)
	{
		position[pid].markers += count;
		for (int i = 0; i < count; i++)
			marker.push_back(Marker(pid));
	}
	void Petri::ActiveTransition(int trans_id) {
		vector<int> marker_transport;
		vector<int> check_marker;
		int count=0;
		std::random_device rd;
		std::mt19937 gen(rd());
		std::normal_distribution<float> d(50, 25);
		for (int i = 0; i < position.size(); i++)
			if (transition[trans_id].position_entrance[i] > 0) {
				check_marker.clear();
				for (int j = 0; j < marker.size(); j++) {
					if (marker[j].Position == i)
						check_marker.push_back(j);
				}
				if (check_marker.size() > transition[trans_id].position_entrance[i]) {
					vector<int> need_marker;
					for (int k = 0; k < marker_q.size(); k++)
						for (int m = 0; m < check_marker.size(); m++)
							if (marker_q[k] == check_marker[m])
								need_marker.push_back(marker_q[k]);
					while (need_marker.size() > transition[trans_id].position_entrance[i])
						need_marker.erase(need_marker.end() - 1);
					if (need_marker.size() < transition[trans_id].position_entrance[i])
						for (int k = 0; k < check_marker.size() && need_marker.size() < transition[trans_id].position_entrance[i]; k++) {
							bool flag = false;
							for (int m = 0; m < need_marker.size(); m++)
								if (need_marker[m] == check_marker[k]) {
									flag = true;
									break;
								}
							if (!flag)
								need_marker.push_back(check_marker[k]);
						}
					for (int j = 0; j < need_marker.size(); j++)
						marker_transport.push_back(need_marker[j]);
					need_marker.clear();
				}
				else
					for (int j = 0; j < check_marker.size(); j++)
						marker_transport.push_back(check_marker[j]);
			}
		count = 0;
		for (int i = 0; i < position.size(); i++)
			count += transition[trans_id].position_exit[i];
		while (marker_transport.size() > count)
		{
			lock_marker.lock();
			marker.erase(marker.begin() + marker_transport[0]);
			for (int i = 0; i < marker_q.size(); i++)
				if (marker_q[i] == marker_transport[0])
				{
					marker_q.erase(marker_q.begin() + i);
					break;
				}
			marker_transport.erase(marker_transport.begin());
			lock_marker.unlock();
		}
		while (marker_transport.size()<count)
		{
			lock_marker.lock();
			marker.push_back(Marker(0));
			marker_transport.push_back(marker.size() - 1);
			position[0].markers++;
			lock_marker.unlock();
		}
		for (int i = 0; i < position.size(); i++)
			if (transition[trans_id].position_exit[i] > 0) {
				count = transition[trans_id].position_exit[i];
				while (count > 0)
				{
					lock_marker.lock();
					position[marker[marker_transport[0]].Position].markers--;
					marker[marker_transport[0]].Position = i;
					marker[marker_transport[0]].Ready = false;
					position[i].markers++;
					marker[marker_transport[0]].Timer = std::chrono::system_clock::now();
					marker[marker_transport[0]].Current_Duration = std::chrono::milliseconds(int(d(gen)) % 100);
					for (int k = 0; k < marker_q.size(); k++)
						if (marker_q[k] == marker_transport[0]) {
							marker_q.erase(marker_q.begin() + k);
							break;
						}
					marker_transport.erase(marker_transport.begin());
					lock_marker.unlock();
					count--;
				}
			}
		
	}
	void Petri::MainFunc() {
		vector<std::thread> thr;
		vector<int> transition_use;
		while (!stop_flag && marker.size()> 0 &&marker.size()<1000 && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - Timer).count() <= std::chrono::duration_cast<std::chrono::milliseconds>(Current_Duration).count()) {
			std::unique_lock<std::mutex> lk(lock);
			not_full.wait(lk);
			UpdateTransitions();
			int thread_use = 1;
			transition_use.clear();
			lock_marker.lock();
			for(auto i = marker_q.begin(); i != marker_q.end() && thread_use != std::thread::hardware_concurrency() ;i++)
				if (marker[(*i)].AvailibleTransitions.size()>0 && transition_use.size()==0) {
					thr.push_back(std::thread(&Petri::ActiveTransition, std::ref(*this), marker[(*i)].AvailibleTransitions[rand() % marker[(*i)].AvailibleTransitions.size()]));
					transition_use = marker[(*i)].AvailibleTransitions;
					thread_use++;
				}
				else
				{
					bool flag = false;
					for (int k = 0; k < transition_use.size(); k++) {
						for (int j = 0; j < marker[(*i)].AvailibleTransitions.size(); j++)
						{
							int check1 = transition_use[k];
							int check2 = marker[(*i)].AvailibleTransitions[j];
							if (check1 == check2) {
								flag = true;
								break;
							}
						}
						if (flag)
							break;
					}
					if (!flag)
					{
						thr.push_back(std::thread(&Petri::ActiveTransition, std::ref(*this), marker[(*i)].AvailibleTransitions[rand() % marker[(*i)].AvailibleTransitions.size()]));
						for (auto v = marker[(*i)].AvailibleTransitions.begin(); v != marker[(*i)].AvailibleTransitions.end(); v++)
							transition_use.push_back((*v));
						thread_use++;
					}
				}
			lock_marker.unlock();
			if (transition_use.size() == 0)
				stop_flag = true;
			for (auto& t : thr)
				t.join();
			thr.clear();
		}
		if (marker.size() >= 1000)
			std::cout << "The Petri net is stopped because there are more than 1000 markers in the network" << std::endl;
		if(stop_flag)
			std::cout<< "The Petri net is stopped and all markers are blocked." << std::endl;
		if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - Timer).count() > std::chrono::duration_cast<std::chrono::milliseconds>(Current_Duration).count())
			std::cout << "Time out." << std::endl;
	}

		void Petri::UpdateMarker() {
		while (!stop_flag && marker.size() > 0 && marker.size() < 1000 && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - Timer).count() <= std::chrono::duration_cast<std::chrono::milliseconds>(Current_Duration).count()) {
			int i = 0;
			while (i < marker.size()) {
				if (marker[i].Ready == true) {
					i++;
					continue;
				}
				lock_marker.lock();
				if (std::chrono::milliseconds((std::chrono::system_clock::now() - marker[i].Timer).count()) >= std::chrono::milliseconds(marker[i].Current_Duration.count())) {
					marker_q.push_back(i);
					marker[i].Ready = true;
				}
				i++;
				lock_marker.unlock();
			}
			not_full.notify_one();
		}
	}

	void Petri::UpdateTransitions() {
		bool flag;
		for (int i = 0; i < marker.size(); i++) {
			marker[i].AvailibleTransitions.clear();
		}
		for (int i = 0; i < marker_q.size(); i++)
		{
			Position p = position[marker[marker_q[i]].Position];
			for (int k = 0; k < transition.size(); k++)
			{
				flag = false;
				if (p.transition_exit[k] > 0 && p.markers >= transition[k].position_entrance[marker[marker_q[i]].Position]) {
					for (int j = 0; j < position.size(); j++)
						if (transition[k].position_entrance[j] > 0 && transition[k].position_entrance[j] > position[j].markers) {
							flag = true;
							break;
						}
					if (!flag)
						marker[marker_q[i]].AvailibleTransitions.push_back(k);
				}
			}
		}

	}


	void Petri::Start(int s) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::normal_distribution<float> d(50, 25);
		for (std::vector<Marker>::iterator it = marker.begin(); it != marker.end(); it++) {
			it->Timer = std::chrono::system_clock::now();
			it->Current_Duration = std::chrono::milliseconds(int(d(gen)) % 100);
		}
		Current_Duration = std::chrono::seconds(s);
		Timer = std::chrono::system_clock::now();
		std::thread MainThread(&Petri::MainFunc, this);
		std::thread UpdateThread(&Petri::UpdateMarker, this);
		MainThread.join();
		UpdateThread.join();
	}
	Marker::Marker(int pl):id(ID), Position(pl), Ready(false) {
		ID++;
	}
}