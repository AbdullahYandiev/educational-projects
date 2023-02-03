#include "api.hpp"
#include <string>
#include <algorithm>
#include <set>
#include <vector>

int get_index(std::set <std::string> states, std::string state);
int min_index(int *X, int size);
std::string I(std::string re);
std::string B(std::string re);
std::string RM(std::string in, std::string out);

std::string dfa2re(DFA &d) {
	Alphabet alpha = d.get_alphabet();
	std::set <std::string> states = d.get_states(), final_states = d.get_final_states();
	std::set <std::string>::iterator it = states.begin();
	const int num_states = states.size();
	int pos, pos_to, *frequency = new int[num_states]();
	std::string state, regexp;
	std::vector<int> ins, outs;
	std::string **transes = new std::string*[num_states+2];
	for (pos=0; pos<num_states+2; ++pos)
		transes[pos] = new std::string[num_states+2]();

	for (pos=1; pos<=num_states; ++pos, ++it) {
		state = *it;
		if (state == d.get_initial_state())
			transes[0][pos] = '$', ++frequency[pos-1];
		if (std::find(final_states.begin(), final_states.end(), state) != final_states.end())
			transes[pos][num_states+1] = '#', ++frequency[pos-1];
		for (const char *a=alpha.begin(); a!=alpha.end(); ++a)
			if (d.has_trans(state, *a)) {
				pos_to = get_index(states, d.get_trans(state, *a));
				if (not transes[pos][pos_to].empty())
					transes[pos][pos_to] += '|';
				transes[pos][pos_to] += *a;
				++frequency[pos-1], ++frequency[pos_to-1];
			}
	}

	for (int i=0; i<num_states; ++i) {
		ins.clear(), outs.clear();
		pos = min_index(frequency, num_states);
		for (int k=0; k<num_states+2; ++k)
			if (k != pos) {
				if (not transes[k][pos].empty())
					ins.push_back(k);
				if (not transes[pos][k].empty())
					outs.push_back(k);
			}
		for (auto const &pos_from : ins) {
			for (auto const &pos_to : outs) {
				if (not transes[pos_from][pos_to].empty())
					transes[pos_from][pos_to] += '|';
				if (transes[pos_from][pos] == "$")
					transes[pos_from][pos].clear();
				if (transes[pos][pos_to] == "#") {
					transes[pos][pos_to].clear();
					if (not transes[pos][pos].empty())
						transes[pos_from][pos] = B(transes[pos_from][pos]);
				}
				else
					transes[pos_from][pos] = B(transes[pos_from][pos]);
				transes[pos_from][pos_to] += RM(transes[pos_from][pos] + I(transes[pos][pos]), transes[pos][pos_to]);
			}
			transes[pos_from][pos] = "";
		}
		for (pos_to=1; pos_to<num_states+2; ++pos_to)
			if (not transes[pos][pos_to].empty())
				transes[pos][pos_to].clear();
	}
	regexp = transes[0][num_states+1];

	if (std::find(final_states.begin(), final_states.end(), d.get_initial_state()) != final_states.end())
		if (not regexp.empty() and regexp[0] != '|')
			regexp = '|' + regexp;

	for (pos=0; pos<num_states+2; ++pos)
		delete [] transes[pos];
	delete [] transes;
	delete [] frequency;
	return regexp;
}

int get_index(std::set <std::string> states, std::string state) {
	return std::distance(states.begin(), std::find(states.begin(), states.end(), state)) + 1;
}

int min_index(int *X, int size) {
	int i, min, min_index;
	for (i=0; i<size and X[i] == -1; ++i);
	min = X[i], min_index = i;
	for (++i; i<size; ++i)
		if (X[i] != -1 and min > X[i])
			min = X[i], min_index = i;
	X[min_index] = -1;
	return min_index + 1;
}

std::string I(std::string re) {
	if (not re.size())
		return re;
	else if (re.size() == 1)
		return re + '*';
	return '(' + re + ")*";
}

std::string B(std::string re) {
	std::string::iterator l, r;
	if (re.empty())
		return re;
	for (l=re.begin(); l!=re.end(); ++l) {
		r = std::find(l, re.end(), '(');
		if (std::find(l, r, '|') != r)
			return re = '(' + re + ')';
		if (r == re.end())
			return re;
		l = std::find(r, re.end(), ')');
		for (int i=std::count(++r, l, '('); i>0; --i)
			l = std::find(l, re.end(), ')');
	}
	return re;
}

std::string RM(std::string in, std::string out) {
	if (out.empty())
		return in;
	if (out[0] != '#') {
		if (not in.empty())
			out = B(out);
		return in + out;
	}
	std::string tmp = B(out.substr(2));
	return in + '|' + in + tmp;
}
