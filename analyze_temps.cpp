#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <fstream>
#include <vector>
#include <ctime>
#define MAX_N 8000
#define EARTH_RADIUS 3959.0
#define TIME_IT
//#define USE_HEURISTIC
using namespace std;

struct city {
	string name;
	int temps[12];
	char codes[12];
	int heur;
};

int N; //the total number of cities
city cities[MAX_N];
int min_badness; //the minimum total deviation in temperature that we can achieve
int opt_a, opt_b, opt_f1, opt_f2; //opt_a, opt_b are the indices of optimal pair a,b (starting at a) in cities, opt_f1 and opt_f2 are the switching months
int OPTIMAL; //this is our favorite temperature times 10
double MIN_LAT_DIFF;
string MONTHS[12] = {"January","February","March","April","May","June","July","August","September","October","November","December"};
vector<string> state_list;
bool state_required;
unordered_map<string, pair<string, string> > city_map;
unordered_map<string, double> latitudes, longitudes;
double MIN_LAT,MAX_LAT,MIN_LONG,MAX_LONG;
bool city_ok_dp[MAX_N];

//returns min possible total deviation 
//starting from city a and (not necessarily) going to city b
//runtime: O(M^3)
// best_score2 is faster; this is just here to test it for correctness.
int best_score1(city &a, city &b) {
	int ans = 1000000000, k;
	//never switch
	k = 0;
	//add temps, staying in a
	for (int i=0; i<12; i++)
		k += abs(a.temps[i]-OPTIMAL);
	//take it if best so far
	if (k < ans)
		ans = k;
	for (int s1=0; s1<12; s1++) {
		//switch before month s1
		k = 0;
		//temps for a
		for (int i=0; i<s1; i++) 
			k += abs(a.temps[i]-OPTIMAL);
		//temps for b
		for (int i=s1; i<12; i++)
			k += abs(b.temps[i]-OPTIMAL);
		//take it if best so far
		if (k < ans)
			ans = k;
		//try any switching time from s1+1 to 11
		for (int s2=s1+1; s2<12; s2++) {
			//switch before month s1 and back before month s2
			k = 0;
			//add temps for starting months in a
			for (int i=0; i<s1; i++)
				k += abs(a.temps[i]-OPTIMAL);
			//add temps for middle months in b
			for (int i=s1; i<s2; i++)
				k += abs(b.temps[i]-OPTIMAL);
			//add temps for final months in a
			for (int i=s2; i<12; i++)
				k += abs(a.temps[i]-OPTIMAL);
			//take it if best so far
			if (k < ans)
				ans = k;
		}
	}
	return ans;
}

//returns min possible total deviation 
//starting from city a and (not necessarily) going to city b
//runtime: O(M)
int best_score2(city &a, city &b) {
	#ifdef USE_HEURISTIC
	if (a.heur+b.heur >= min_badness)
		return min_badness;
	#endif
	int dp[12][3];
	//dp[i][j] is the min badness in months i to 11
	//after using exactly j switches before month i started
	dp[11][2] = abs(a.temps[11]-OPTIMAL); //we've already used both switches, we're back in a for the last one
	dp[11][1] = min(abs(b.temps[11]-OPTIMAL),dp[11][2]);//we have a switch left, we can choose
	dp[11][0] = dp[11][1]; //same, we can choose
	int cost_a,cost_b;
	for (int i=10; i>-1; i--) {
		cost_a = abs(a.temps[i]-OPTIMAL); //cost_a is the cost of staying in city a in the ith month (Jan = 0th)
		cost_b = abs(b.temps[i]-OPTIMAL); //same for b
		dp[i][2] = cost_a+dp[i+1][2];  //if we've used our switches, we must stay in a
		dp[i][1] = min(cost_b+dp[i+1][1],cost_a+dp[i+1][2]); //we can either stay in b, or switch back to a, using a switch
		dp[i][0] = min(cost_a+dp[i+1][0],cost_b+dp[i+1][1]); //we can either stay in a, or switch to b, using a switch
		if (dp[i][0] >= min_badness) //if it already costs as much as the best solution in months i-Dec, using all switches, it's not optimal, so don't waste any more time here
			return min_badness;
	}
	return dp[0][0]; //minimum cost before January with all switches left = answer
}

//finds the best months to switch from a to b (starting at a)
//and stores them in opt_f1 and opt_f2 (-1 = never)
void get_switch_months(city &a, city &b) {
	int ans = 1000000000, k;
	//never switch
	k = 0;
	for (int i=0; i<12; i++)
		k += abs(a.temps[i]-OPTIMAL);
	if (k < ans) {
		ans = k;
		opt_f1 = -1;
		opt_f2 = -1;
	}
	for (int s1=0; s1<12; s1++) {
		//switch before month s1
		k = 0;
		for (int i=0; i<s1; i++) 
			k += abs(a.temps[i]-OPTIMAL);
		for (int i=s1; i<12; i++)
			k += abs(b.temps[i]-OPTIMAL);
		if (k < ans) {
			ans = k;
			opt_f1 = s1;
			opt_f2 = -1;
		}
		for (int s2=s1+1; s2<12; s2++) {
			//switch before month s1 and back before month s2
			k = 0;
			for (int i=0; i<s1; i++)
				k += abs(a.temps[i]-OPTIMAL);
			for (int i=s1; i<s2; i++)
				k += abs(b.temps[i]-OPTIMAL);
			for (int i=s2; i<12; i++)
				k += abs(a.temps[i]-OPTIMAL);
			if (k < ans) {
				ans = k;
				opt_f1 = s1;
				opt_f2 = s2;
			}
		}
	}
}

bool is_ok(city &a, city &b) {
	return abs(latitudes[a.name]-latitudes[b.name]) >= MIN_LAT_DIFF;
}

// checks if the city satisfies the conditions from the input file
bool city_ok(city &c) {
	if (state_required) {
		for (int i=0; i<state_list.size(); i++) {
			if (city_map[c.name].first == state_list[i]) {
				double cLat = (double)latitudes[c.name], cLong = (double)longitudes[c.name];
				return (cLat >= MIN_LAT && cLong >= MIN_LONG && cLat <= MAX_LAT && cLong <= MAX_LONG);
			}
		}
		return false;
	}
	else {
		for (int i=0; i<state_list.size(); i++) {
			if (city_map[c.name].first == state_list[i]) {
				return false;
			}
		}
		double cLat = (double)latitudes[c.name], cLong = (double)longitudes[c.name];
		return (cLat >= MIN_LAT && cLong >= MIN_LONG && cLat <= MAX_LAT && cLong <= MAX_LONG);
	}
}

double ns_dist(city &a, city &b) {
	return EARTH_RADIUS*abs(latitudes[a.name]-latitudes[b.name])*3.141592654/180.0;
}

int calc_heur(city &c) {
	c.heur = 0;
	for (int i=0; i<12; i++)
		c.heur += abs(c.temps[i]-OPTIMAL);
}

int main() {
	//read in input data - we're taking the OPTIMAL constant as a command-line argument
	//format of OPTIMAL is a float to one decimal place (e.g. 71.8)
	ifstream inputs("inputs.txt");
	string inStr;
	
	inputs >> inStr;
	OPTIMAL = (int)(atof(inStr.c_str())*10.0);
	
	inputs >> inStr;
	double NS_DIST_MIN = atof(inStr.c_str());
	MIN_LAT_DIFF = atof(inStr.c_str())/EARTH_RADIUS*180.0/3.141592654;
	
	inputs >> MIN_LAT >> MAX_LAT >> MIN_LONG >> MAX_LONG;
	
	bool OPEN_GOOGLE_MAP;
	inputs >> OPEN_GOOGLE_MAP;
	
	bool MAKE_PLOT;
	inputs >> MAKE_PLOT;
	
	char inChar;
	inputs >> inChar;
	state_required = (inChar == 'r');
	
	while (!inputs.eof()) {
		inputs >> inStr;
		if (inStr.size() != 2)
			break;
		state_list.push_back(inStr);
	}
	
	inputs.close();
	freopen("tempdata.txt","r",stdin);
	N = 0;
	while (!cin.eof()) {
		cin >> cities[N].name;
		if (cities[N].name.size() != 11) //we've missed the last city; we're picking up a blank at the end of the file
			break;
		for (int i=0; i<12; i++) {
			cin >> cities[N].temps[i];
			cin >> cities[N].codes[i];
		}
		N++;
	}
	
	ifstream statin("stationdata.txt");
	string statCode, latVal, longVal, altitude, state, name;
	pair<string,string> coords;
	while (!statin.eof()) {
		statin >> statCode >> latVal >> longVal >> altitude >> state >> name;
		coords.first = state;
		coords.second = name;
		city_map[statCode] = coords;
		latitudes[statCode] = atof(latVal.c_str());
		longitudes[statCode] = atof(longVal.c_str());
	}
	/*
	Test the fast algorithm best_score2 against the 
	slow but correct best_score1 to verify correctness
	- best_score2 and best_score1 give the same answer, 
	evidence for correctness of best_score2
	*/
	
	/*
	for (int i=0; i<N; i++)
		for (int j=0; j<N; j++) {
			if (i == j)	 continue;
			if ((best_score1(cities[i],cities[j]) != best_score2(cities[i],cities[j])) && (best_score1(cities[i],cities[j]) != min_badness)) {
				cout << "FAIL\n";
				cout << cities[i].name << ' ' << cities[j].name << '\n';
				cout << i << ' ' << j << '\n';
				cout << best_score1(cities[i],cities[j]) << '\n';
				cout << best_score2(cities[i],cities[j]) << '\n';
				return 0;
			}
		}
	*/
	
	int now;
	//try each pair of cities: total complexity is O(N^2M), ok since M = 12 and N < 8000
	//the score for a pair of cities is the minimum over all valid flight plans of the 
	//sum over all months of the difference between the city temperature and the optimum multiplied by 10
	//this is larger than any pair of cities to get started 
	#ifdef TIME_IT
	time_t startTime = clock();
	#endif
	for (int i=0; i<N; i++) {
		city_ok_dp[i] = city_ok(cities[i]);
		#ifdef USE_HEURISTIC
		calc_heur(cities[i]);
		#endif
	}
	min_badness = 1000000000;
	for (int i=0; i<N; i++) {
		if (!city_ok_dp[i]) {
			continue;
		}
		for (int j=0; j<N; j++) {
			if (!city_ok_dp[j])
				continue;
			if (i == j) continue; //no need to try going from a city to itself, since best_score2 allows us not to fly
			now = best_score2(cities[i],cities[j]); //O(M)
			//if this is the best one found so far, record it and the pair of cities
			//we have to change latitude by a certain amount though and make sure it's in our box
			if (now < min_badness && is_ok(cities[i],cities[j])) {
				min_badness = now;
				opt_a = i;
				opt_b = j;
			}
		}
	}
	
	//find which months are good to switch in for the optimal pair of cities
	get_switch_months(cities[opt_a],cities[opt_b]);
	
	#ifdef TIME_IT
	cout << clock()-startTime << '\n';
	#endif
	
	//output relevant data
	cout << "Note: all temperatures below are in degrees Fahrenheit.\n";
	cout << "Optimal plan for a desired temperature of " << ((double)OPTIMAL)/10.0 << ":\n";
	cout << "Restrictions:\n";
	cout << "North/south distance between the two cities must be at least " << NS_DIST_MIN << " miles.\n";
	cout << "Minimum latitude = " << MIN_LAT << ", maximum latitude = " << MAX_LAT << '\n';
	cout << "Minimum longitude = " << MIN_LONG << ", maximum longitude = " << MAX_LONG << '\n';
	
	if (state_list.size() > 0) {	
		if (state_required)
			cout << "Must be in one of the following states:\n";
		else 
			cout << "Cannot be in the following states:\n";
		for (int i=0; i<state_list.size()-1; i++)
			cout << state_list[i] << ", ";
		cout << state_list[state_list.size()-1] << '\n';
	}
	
	if (opt_f1 == -1) {
		cout << "Stay in city " << cities[opt_a].name << " all year.\n";
	}
	else {
		cout << "Start in city " << cities[opt_a].name << ".\n";
		cout << "At the beginning of " << MONTHS[opt_f1] << ", fly to city " << cities[opt_b].name << ".\n";
		if (opt_f2 == -1) {
			cout << "Stay in city " << cities[opt_b].name << " for the rest of the year.\n";
		}
		else {
			cout << "At the beginning of " << MONTHS[opt_f2] << ", fly to city " << cities[opt_a].name << ", and stay there for the rest of the year.\n";
		}
	}
	cout << "\n\n";
	cout << "The resulting average deviation in temperature from the optimum of " << ((double)OPTIMAL)/10.0 << " will be " << ((double)min_badness)/120.0 << ".\n";
	cout << "\n";
	
	if (opt_f1 == -1) {
		cout << "Here is the set of monthly average temperatures for the optimal city of " << cities[opt_a].name << ":\n";
		for (int i=0; i<12; i++) {
			cout << MONTHS[i] << ": " << ((double)cities[opt_a].temps[i])/10.0 << '\n';
		}
		cout << '\n';
	}
	else {
		cout << "Here are the sets of monthly average temperatures for the two optimal cities:\n";
		cout << cities[opt_a].name << '\n';
		for (int i=0; i<12; i++) {
			cout << MONTHS[i] << ": " << ((double)cities[opt_a].temps[i])/10.0 << '\n';
		}
		cout << '\n';
		cout << cities[opt_b].name << '\n';
		for (int i=0; i<12; i++) {
			cout << MONTHS[i] << ": " << ((double)cities[opt_b].temps[i])/10.0 << '\n';
		}
		cout << '\n';
	}
	cout << cities[opt_a].name << " -> " << '(' << city_map[cities[opt_a].name].first << ") " << city_map[cities[opt_a].name].second << "(latitude = " << latitudes[cities[opt_a].name] << ", longitude = " << longitudes[cities[opt_a].name] << ')' << '\n';
	cout << cities[opt_b].name << " -> " << '(' << city_map[cities[opt_b].name].first << ") " << city_map[cities[opt_b].name].second << "(latitude = " << latitudes[cities[opt_b].name] << ", longitude = " << longitudes[cities[opt_b].name] << ')' << '\n';
	cout << "North/South distance = " << ns_dist(cities[opt_a],cities[opt_b]) << " miles" << '\n';
	
	if (OPEN_GOOGLE_MAP) {
		ostringstream city_output;
		city_output << "python google_maps_opener.py " << latitudes[cities[opt_a].name] << ' ' << longitudes[cities[opt_a].name] <<
			' ' << latitudes[cities[opt_b].name] << ' ' << longitudes[cities[opt_b].name];
		system(city_output.str().c_str());
	}
	
	if (MAKE_PLOT) {
		ofstream city_temps("citytemps.txt");
		city_temps << ((double)OPTIMAL)/10.0 << '\n';
		for (int i=0; i<12; i++) {
			if (((i<opt_f1)+(i<opt_f2))%2) {
				city_temps << "1 ";
				city_temps << ((double)cities[opt_b].temps[i])/10.0 << '\n';
			}
			else {
				city_temps << "0 ";
				city_temps << ((double)cities[opt_a].temps[i])/10.0 << '\n';
			}
		}
		city_temps << city_map[cities[opt_a].name].first << '-' << city_map[cities[opt_a].name].second << '\n';
		city_temps << city_map[cities[opt_b].name].first << '-' << city_map[cities[opt_b].name].second << '\n';
		city_temps.close();
		system("python histogram_maker.py");
	}
	return 0;
}
	
