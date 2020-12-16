#include"Petri.h"
namespace Prog5 {
	int ID = 0;
	std::atomic<bool> que_flag{ false };
	std::atomic<bool> main_flag{ false };
	std::atomic<bool> wait{ false };
	std::atomic<bool> stop_flag{ false };
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
	void Petri::ActiveTransition(int trans_id, ofstream& file) {
		std::unique_lock<std::mutex> lk(lock_activ);
		while (wait)
			activate.wait(lk);
		vector<std::thread> thr;
		vector<int> marker_transport;
		vector<int> check_marker;
		int count=0;
		std::random_device rd;
		std::mt19937 gen(rd());
		std::normal_distribution<float> d(500, 250);
		for (int i = 0; i < transition[trans_id].position_entrance.size(); i++)
			if (transition[trans_id].position_entrance[i] > 0) {
				check_marker.clear();
				lock_marker.lock();
				for (int j = 0; j < marker.size(); j++) {
					if (marker[j].Position == i)
						check_marker.push_back(marker[j].id);
				}
				lock_marker.unlock();
				//std::this_thread::sleep_for(10ms);
				if (check_marker.size() > transition[trans_id].position_entrance[i]) {
					vector<int> need_marker;
					lock_marker.lock();
					for (int k = 0; k < marker_q.size(); k++)
						for (int m = 0; m < check_marker.size(); m++)
							if (marker[marker_q[k]].id == check_marker[m])
								need_marker.push_back(check_marker[m]);
					lock_marker.unlock();
					//std::this_thread::sleep_for(10ms);
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
					for (auto v = need_marker.begin(); v != need_marker.end(); ++v)
						marker_transport.push_back((*v));
					need_marker.clear();
				}
				else
					for (auto v = check_marker.begin(); v != check_marker.end(); ++v)
						marker_transport.push_back((*v));
			}
		count = 0;
		for (int i = 0; i < transition[trans_id].position_exit.size(); i++)
			count += transition[trans_id].position_exit[i];
		lock_marker.lock();
		while (marker_transport.size() > count)
		{
			int found = -1;
			std::string str(std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - Timer).count()));
			str += "ms";
			file << "Time: " << str << " | " << " Marker " << marker_transport[0]<< ": deleted " << std::endl;
			for(int i=0;i<marker.size();i++)
				if (marker_transport[0] == marker[i].id)
				{
					found = i;
					break;
				}
			position[marker[found].Position].markers--;
			for (int i = 0; i < marker_q.size(); i++)
				if (marker_q[i] == found)
				{
					marker_q.erase(marker_q.begin() + i);
					break;
				}
			for (int i = 0; i < marker_q.size(); i++)
				if (marker_q[i] > found)
					marker_q[i]--;
			marker.erase(marker.begin() + found);
			marker_transport.erase(marker_transport.begin());
		}
		while (marker_transport.size()<count)
		{
			std::string str(std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - Timer).count()));
			str += "ms";
			marker.push_back(Marker(0));
			file << "Time: " << str << " | " << " Marker " << marker[marker.size() - 1].id << ": created " << std::endl;
			marker_transport.push_back(marker[marker.size()-1].id);
			position[0].markers++;
		}
		lock_marker.unlock();
		for (int i = 0; i < transition[trans_id].position_exit.size(); i++)
			if (transition[trans_id].position_exit[i] > 0) {
				count = transition[trans_id].position_exit[i];
				lock_marker.lock();
				while (count > 0)
				{
					int found = -1;
					for (int i = 0; i < marker.size(); i++)
						if (marker_transport[0] == marker[i].id)
						{
							found = i;
							break;
						}
					std::string str(std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - Timer).count()));
					str += "ms";
					file << "Time: " << str << " | " << " Marker " << marker_transport[0]<< ": moved from " << marker[found].Position << " to " << i << " | use transition "<< trans_id << std::endl;
					std::cout << "Time: " << str << std::endl;
					position[marker[found].Position].markers--;
					marker[found].Position = i;
					position[i].markers++;
					marker[found].Timer = std::chrono::system_clock::now();
					marker[found].Current_Duration = std::chrono::milliseconds(int(d(gen)) % 1000);
					for (int k = 0; k < marker_q.size(); k++)
						if (marker[marker_q[k]].id == marker_transport[0]) {
							marker_q.erase(marker_q.begin() + k);
							break;
						}
					marker_transport.erase(marker_transport.begin());
					count--;
					
				}
				lock_marker.unlock();
			}
	}
	void Petri::MainFunc(ofstream& file) {
		vector<int> transition_use;
		while (!stop_flag && marker.size() > 0 && marker.size() < 1000 && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - Timer).count() <= std::chrono::duration_cast<std::chrono::milliseconds>(Current_Duration).count()) {
			std::this_thread::sleep_for(100ms);
			std::unique_lock<std::mutex> lk(lock);
			vector<std::thread> thr;
			while (!que_flag)
				not_full.wait(lk);
			main_flag = true;
			int thread_use = 3;
			transition_use.clear();
			wait = true;
			UpdateTransitions(file);
			lock_marker.lock();
			for(auto i = marker_q.begin(); i != marker_q.end() && thread_use < std::thread::hardware_concurrency() ;i++)
				if (marker[(*i)].AvailibleTransitions.size()>0 && transition_use.size()==0) {
					thr.push_back(std::thread(&Petri::ActiveTransition, ref(*this), marker[(*i)].AvailibleTransitions[rand() % marker[(*i)].AvailibleTransitions.size()], std::ref(file)));
					transition_use = marker[(*i)].AvailibleTransitions;
					thread_use++;
				}
				else
				{
					if (marker[(*i)].AvailibleTransitions.size() > 0) {
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
							thr.push_back(std::thread(&Petri::ActiveTransition, ref(*this), marker[(*i)].AvailibleTransitions[rand() % marker[(*i)].AvailibleTransitions.size()], std::ref(file)));
							for (auto v = marker[(*i)].AvailibleTransitions.begin(); v != marker[(*i)].AvailibleTransitions.end(); v++)
								transition_use.push_back((*v));
							thread_use++;
						}
					}
				}
			lock_marker.unlock();
			if (transition_use.size() == 0) {
				stop_flag = true;
				break;
			}
			main_flag = false;
			que.notify_one();
			wait = false;
			activate.notify_all();
			for (auto& t : thr)
				t.join();
			thr.clear();
			std::this_thread::sleep_for(100ms);
		}
		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - Timer).count() > std::chrono::duration_cast<std::chrono::milliseconds>(Current_Duration).count()) {
			std::cout << "Time out." << std::endl;
			return;
		}
		if (marker.size() >= 1000) {
			std::cout << "The Petri net is stopped because there are more than 1000 markers in the network" << std::endl;
			return;
		}
		if (stop_flag) {
			std::cout << "The Petri net is stopped and all markers are blocked." << std::endl;
			return;
		}
	}

	void Petri::UpdateMarker()
	{
		while (!stop_flag && marker.size() > 0 && marker.size() < 1000 && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - Timer).count() <= std::chrono::duration_cast<std::chrono::milliseconds>(Current_Duration).count()) {
			std::unique_lock<std::mutex> lk(lock_que);
			while (main_flag)
				que.wait(lk);
			int i = 0;
			bool flag = false;
			que_flag = false;
			bool flag_update = false;
			lock_marker.lock();
			while (i < marker.size()) {
				flag = false;
				for (int k = 0; k < marker_q.size(); k++)
					if (marker_q[k] == i) {
						flag = true;
						continue;
					}
				if (!flag && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - marker[i].Timer).count() >= std::chrono::duration_cast<std::chrono::milliseconds>(marker[i].Current_Duration).count()) {
					marker_q.push_back(i);
					flag_update = true;
				}
				i++;
			}
			int a = marker_q.size();
			/*
			if(flag_update)
				UpdateTransitions();
			*/
			lock_marker.unlock();
			if (a > 0)
				que_flag = true;
			not_full.notify_one();
			std::this_thread::sleep_for(100ms);
		}
		que_flag = true;
		not_full.notify_one();
	}

	void Petri::UpdateTransitions(ofstream& file) {
		bool flag;
		lock_marker.lock();
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
					if (!flag && marker_q[i] < marker.size()) {
						marker[marker_q[i]].AvailibleTransitions.push_back(k);
					}
				}
			}
			if (marker[marker_q[i]].AvailibleTransitions.size() == 0) {
				std::string str(std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - Timer).count()));
				str += "ms";
				file << "Time: " << str << " | " << " Marker " << marker[marker_q[i]].id << ": blocked " << std::endl;
			}

		}
		lock_marker.unlock();

	}


	void Petri::Start(int s, ofstream& file) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::normal_distribution<float> d(500, 250);
		stop_flag = false;
		que_flag = false;
		main_flag = false;
		for (std::vector<Marker>::iterator it = marker.begin(); it != marker.end(); it++) {
			it->Timer = std::chrono::system_clock::now();
			it->Current_Duration = std::chrono::milliseconds(int(d(gen)) % 1000);
		}
		Current_Duration = std::chrono::seconds(s);
		Timer = std::chrono::system_clock::now();
		std::thread MainThread(&Petri::MainFunc, ref(*this), std::ref(file));
		std::thread UpdateThread(&Petri::UpdateMarker, ref(*this));
		MainThread.join();
		UpdateThread.join();
	}
	Marker::Marker(int pl):id(ID), Position(pl) {
		ID++;
	}
}