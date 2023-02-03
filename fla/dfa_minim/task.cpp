#include "api.hpp"
#include <string>
#include <vector>
#include <queue>
#include <algorithm>

int get_group(std::vector <std::vector <std::string>> &groups, std::string member);

DFA dfa_minim(DFA &d) {
	std::set <std::string> states = d.get_states();
	Alphabet alpha = d.get_alphabet();
	std::queue <std::string> Q;
	std::string state;
	Q.push(d.get_initial_state());
	while (not Q.empty()) {
		state = Q.front();
		Q.pop();
		states.erase(state);
		for (const char *a=alpha.begin(); a!=alpha.end(); ++a)
			if (d.has_trans(state, *a) and std::find(states.begin(), states.end(), d.get_trans(state, *a)) != states.end())
				Q.push(d.get_trans(state, *a));
	}
	for (auto const &state: states)
		d.delete_state(state);

	d.create_state("void");
	states = d.get_states();
	std::set <std::string> finals = d.get_final_states();
	std::vector <std::vector <std::string>> groups;
	std::vector <std::vector <std::vector <int>>> partition;
	std::vector <std::string> old_group, new_group;
	int i = 0, l = 0, j, k;

	groups.push_back(std::vector <std::string>()), groups.push_back(std::vector <std::string>());
	groups[0].insert(groups[0].begin(), "void");
	for (auto const &state : states) {
		if (state != "void")
			groups[std::find(finals.begin(), finals.end(), state) == finals.end() ? 0 : 1].push_back(state);
		for (const char *a=alpha.begin(); a!=alpha.end(); ++a)
			if (not d.has_trans(state, *a))
				d.set_trans(state, *a, "void");
	}
	do {
		new_group.clear(), partition.clear();
		for (j=0, k=0; j<groups.size(); ++j, k=0)
		if (groups[j].size() > 1) {
			partition.push_back(std::vector <std::vector <int>>());
			for (const char *a=alpha.begin(); a!=alpha.end(); ++a, ++k) {
				partition[partition.size() - 1].push_back(std::vector <int>());
				for (auto const &state : groups[j])
					partition[partition.size() - 1][k].push_back(get_group(groups, d.get_trans(state, *a)));
			}
		}
		for (; i<groups.size() and new_group.empty(); ++i)
			if (groups[i].size() > 1) {
				old_group = groups[i];
				for (j=0; j<alpha.size() and new_group.empty(); ++j) {
					for (k=1; k<groups[i].size(); ++k)
						if (partition[l][j][k] != partition[l][j][0]) {
							old_group.erase(old_group.begin() + k - new_group.size());
							new_group.push_back(groups[i][k]);
						}
				}
				++l;
			}
		if (not new_group.empty()) {
			groups[--i] = old_group;
			groups.push_back(new_group);
			--l;
		}
	} while (not new_group.empty());

	for (i=0, j=0; i<groups.size(); ++i, j=0)
	if (groups[i].size() > 1) {
		for (auto const &state : states)
			for (const char *a=alpha.begin(); a!=alpha.end(); ++a)
			if (std::find(groups[i].begin(), groups[i].end(), d.get_trans(state, *a)) != groups[i].end())
				d.set_trans(state, *a, groups[i][0]);
	}
	for (auto const &group : groups)
		for (i=1; i<group.size(); ++i)
			d.delete_state(group[i]);
	d.delete_state("void");
	return d;
}

int get_group(std::vector <std::vector <std::string>> &groups, std::string member) {
	for (int i=0; i<groups.size(); ++i)
		if (std::find(groups[i].begin(), groups[i].end(), member) != groups[i].end())
			return i;
	return -1;
}