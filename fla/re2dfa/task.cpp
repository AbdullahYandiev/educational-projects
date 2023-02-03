#include "api.hpp"
#include <string>
#include <set>
#include <algorithm>

#define START_CONCAT(nullable, first_pos, last_pos)										\
	if (*it == '(')																		\
		unions(++it, last_it, alpha, pos, nullable, first_pos, last_pos, follow_pos);	\
	else if (alpha.has_char(*it))														\
		nullable = false, first_pos.insert(pos), last_pos.insert(pos);					\
	++it;																				\
	if (it != last_it and *it == '*') {													\
		iteration(nullable, first_pos, last_pos, follow_pos);							\
		++it;																			\
	}


std::string make_state(std::set <int> &state);
void make_follow_pos(std::string::const_iterator &it, std::string::const_iterator last_it, Alphabet alpha, int &pos, std::set <int> &first_pos, std::set <int> *follow_pos);
void concat(std::string::const_iterator &it, std::string::const_iterator last_it, Alphabet alpha, int &pos, bool &nullable, std::set <int> &first_pos, std::set <int> &last_pos, std::set <int> *follow_pos);
void unions(std::string::const_iterator &it, std::string::const_iterator last_it, Alphabet alpha, int &pos, bool &nullable, std::set <int> &first_pos, std::set <int> &last_pos, std::set <int> *follow_pos);
void iteration(bool &nullable, std::set <int> &first_pos, std::set <int> &last_pos, std::set <int> *follow_pos);

DFA re2dfa(const std::string &s) {
	DFA res = DFA(Alphabet(s));
	const std::string reg = '(' + s + ")#";
	Alphabet alpha = Alphabet(reg);

	int pos = 0, num_pos = 0;
	for (const char *it=alpha.begin(); it!=alpha.end(); ++it)
		num_pos += count(reg.begin(), reg.end(), *it);

	std::set <int> root_first_pos, *follow_pos = new std::set <int>[num_pos];

	std::string::const_iterator it = reg.begin();
	if (it != reg.end())
		make_follow_pos(it, reg.end(), alpha, pos, root_first_pos, follow_pos);

	std::string poses = reg;
	const std::string chars = "|*()";
	poses.erase(remove_if(poses.begin(), poses.end(), [&chars](const char& c) {
		return chars.find(c) != std::string::npos;
	}), poses.end());

	std::set <std::set <int>> Q, M;
	std::set <int> S, R;
	Q.insert(root_first_pos);
	M.insert(root_first_pos);
	std::string state = make_state(root_first_pos);
	root_first_pos.find(num_pos-1) != root_first_pos.end() ? res.create_state(state, true) : res.create_state(state);
	res.set_initial(state);

	while (not M.empty()) {
		R = *M.begin();
		M.erase(M.begin());
		for (const char *a=alpha.begin(); a!=alpha.end(); ++a) {
			S.clear();
			for (auto pi: R)
				if (poses[pi] == *a)
					S.insert(follow_pos[pi].begin(), follow_pos[pi].end());
			if (not S.empty()) {
				if (Q.find(S) == Q.end())
					Q.insert(S), M.insert(S);
				S.find(num_pos-1) != S.end() ? res.create_state(make_state(S), true) : res.create_state(make_state(S));
				res.set_trans(make_state(R), *a, make_state(S));
			}
		}
	}

	delete [] follow_pos;
	return res;
}

void make_follow_pos(std::string::const_iterator &it, std::string::const_iterator last_it, Alphabet alpha, int &pos, std::set <int> &first_pos, std::set <int> *follow_pos) {
	bool nullable;
	std::set <int> last_pos;

	unions(it, last_it, alpha, pos, nullable, first_pos, last_pos, follow_pos);
}

void unions(std::string::const_iterator &it, std::string::const_iterator last_it, Alphabet alpha, int &pos, bool &nullable, std::set <int> &first_pos, std::set <int> &last_pos, std::set <int> *follow_pos) {
	if (*it == '|')
		nullable = true, --pos;
	else
		concat(it, last_it, alpha, pos, nullable, first_pos, last_pos, follow_pos);

	while (it != last_it and (*it == '|' or *it == '(')) {
		bool r_nullable;
		std::set <int> r_first_pos, r_last_pos;
		
		++it;
		if (it == last_it or *it == ')')
			r_nullable = true;
		else if (*it == '|') {
			r_nullable = true;
			++it;
		}
		else
			concat(it, last_it, alpha, ++pos, r_nullable, r_first_pos, r_last_pos, follow_pos);

		nullable = nullable or r_nullable;
		first_pos.insert(r_first_pos.begin(), r_first_pos.end());
		last_pos.insert(r_last_pos.begin(), r_last_pos.end());
	}
}

void concat(std::string::const_iterator &it, std::string::const_iterator last_it, Alphabet alpha, int &pos, bool &nullable, std::set <int> &first_pos, std::set <int> &last_pos, std::set <int> *follow_pos) {
	START_CONCAT(nullable, first_pos, last_pos);

	while (it != last_it and (alpha.has_char(*it) or *it == '(')) {
		++pos;
		bool r_nullable;
		std::set <int> r_first_pos, r_last_pos;

		START_CONCAT(r_nullable, r_first_pos, r_last_pos);

		for (auto i: last_pos)
			follow_pos[i].insert(r_first_pos.begin(), r_first_pos.end());

		if (nullable)
			first_pos.insert(r_first_pos.begin(), r_first_pos.end());
		if (r_nullable)
			r_last_pos.insert(last_pos.begin(), last_pos.end());
		last_pos = r_last_pos;
		nullable = nullable and r_nullable;
	}
}

void iteration(bool &nullable, std::set <int> &first_pos, std::set <int> &last_pos, std::set <int> *follow_pos) {
	nullable = true;
	for (auto i: last_pos)
		follow_pos[i].insert(first_pos.begin(), first_pos.end());
}

std::string make_state(std::set <int> &state) {
	std::string str;
	for (auto i: state)
		str.append(std::to_string(i));
	return str;
}
