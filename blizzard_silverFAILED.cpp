/*
[FAILED]
League: silver
Spells used: wind
Tactic: Project Winter
Early game use barrier method
Next move to 6000 units from opponent base
If sees monster, push to opp base with 3 wind spells (abusing wind stacking)
*/

#include <bits/stdc++.h>

using namespace std;

#define ll long long
const ll inf = 500000000ll;

struct entity_info{
	int id, type; // unique identifier, type = 0monster 1ally 2opp
	int x, y; 
	int shield_life, is_controlled; // ignore
	int health;
	int vx, vy;
	int near_base, threat_for;
	ll to_base_distance;
	int dealt_with, need_help;

	void read(){
		cin >> id >> type >> x >> y >> shield_life >> is_controlled >> health >> vx >> vy >> near_base >> threat_for;
		to_base_distance = dealt_with = need_help = 0;
	}
};

int dx[] = {1000, 4500, 6000};
int dy[] = {6000, 4500, 1000};

int dx2[] = {17630 - 6000, 17630 - 6000};
int dy2[] = {9000 - 1700, 9000 - 1700};

int dx3[] = {4000, 17630 / 2, 14000, 5000};
int dy3[] = {2000, 9000 / 2, 5000, 5000};

int dx4[] = {3300, 4300, 6000};
int dy4[] = {6300, 4300, 1700};

int tx[] = {1000, 1000, 1000, 1000, 1500, 1500, 3000, 3500, 3500, 4000, 4000, 4000, 4000};
int ty[] = {4000, 4000, 4000, 1000, 3500, 3500, 3000, 1500, 1500, 1000, 1000, 1000, 1000};

int main(){
	int base_x; // The corner of the map representing your base
	int base_y;
	cin >> base_x >> base_y;
	cerr << base_x << " " << base_y << endl;
	int heroes_per_player; // Always 3
	cin >> heroes_per_player; 
	int opp_x = 17630 - base_x, opp_y = 9000 - base_y;

	for(int i = 0; i < 3; i ++){
		dx[i] = (base_x + dx[i] <= 17630 ? base_x + dx[i] : base_x - dx[i]);
		dy[i] = (base_y + dy[i] <= 9000 ? base_y + dy[i] : base_y - dy[i]);
		dx4[i] = (base_x + dx4[i] <= 17360 ? base_x + dx4[i] : base_x - dx4[i]);
		dy4[i] = (base_y + dy4[i] <= 17360 ? base_y + dy4[i] : base_y - dy4[i]);
		if(i < 2){
			dx2[i] = (base_x + dx2[i] <= 17630 ? base_x + dx2[i] : base_x - dx2[i]);
			dy2[i] = (base_y + dy2[i] <= 9000 ? base_y + dy2[i] : base_y - dy2[i]);
		}
	}

	int turn_left = 220;
	int stationed_to_position = 0, next_base = 0;
	// game loop
	for(;;){
		int our_mana;
		for(int i = 0; i < 2; i ++){
			int health; // Your base health
			int mana; // Ignore in the first league; Spend ten mana to cast a spell
			cin >> health >> mana;
			if(i == 0) our_mana = mana;
		}

		int entity_count; // Amount of heros and monsters you can see
		cin >> entity_count; cin.ignore();
		vector<entity_info> entity_list;
		vector<pair<int, pair<int, int> > > li;

		int number_of_stationed_heroes = 0;
		for(int i = 0; i < entity_count; i ++){
			entity_info A;
			A.read();
			A.to_base_distance = (base_x - A.x) * 1ll * (base_x - A.x) + (base_y - A.y) * 1ll * (base_y - A.y);
			if(A.type == 1){
				li.push_back(make_pair(A.id, make_pair(A.x, A.y)));
				if(A.x == dx2[next_base] && A.y == dy2[next_base]) number_of_stationed_heroes ++;
			}
			A.need_help = 2;
			cerr << A.type << " " << A.x << " " << A.y << endl;
			entity_list.push_back(A);
		}
		sort(li.begin(), li.end());

		if(number_of_stationed_heroes == 3){
			stationed_to_position = 1;
			next_base ^= 1;
		}

		// defending
		turn_left --;
		if(turn_left >= 160){ // skyarrow early phase
			 // defending
			bool wind_done_defending = 0;
			for (int i = 0; i < heroes_per_player - 2; i++){
				int x = (base_x + (dx3[i + 1] + 1000) <= 17630 ? base_x + (dx3[i + 1] + 1000) : base_x - (dx3[i + 1] + 1000)), y = (base_y + (dy3[i + 1] - 1000) <= 9000 ? base_y + (dy3[i + 1] - 1000) : base_y - (dy3[i + 1] - 1000));
				ll distance = inf; int herox = li[i].second.first, heroy = li[i].second.second;
				int target_entity = -1;
				for(int j = 0; j < entity_list.size(); j ++) if(entity_list[j].type == 0 && entity_list[j].threat_for != 2 && entity_list[j].dealt_with == 0){
					ll get_distance = (entity_list[j].x - herox) * 1ll * (entity_list[j].x - herox) + (entity_list[j].y - heroy) * 1ll * (entity_list[j].y - heroy);
					ll get_base_distance = (entity_list[j].x - base_x) * 1ll * (entity_list[j].x - base_x) + (entity_list[j].y - base_y) * 1ll * (entity_list[j].y - base_y);
					if(get_base_distance > 5500ll * 5500ll && distance > get_distance){
						distance = get_distance;
						target_entity = j;
					}
				}
				if(target_entity != -1){
						entity_list[target_entity].dealt_with = 1;
						x = entity_list[target_entity].x;
						y = entity_list[target_entity].y;
				}

				cout << "MOVE " << x << " " << y << endl;
			}

			// farming/defending
			for(int i = heroes_per_player - 2; i < heroes_per_player - 1; i ++){
				int x = (base_x + (dx3[i] - 1000) <= 17630 ? base_x + (dx3[i] - 1000) : base_x - (dx3[i] - 1000)), y = (base_y + (dy3[i] + 1000) <= 9000 ? base_y + (dy3[i] + 1000) : base_y - (dy3[i] + 1000));
				ll distance = inf; int herox = li[i].second.first, heroy = li[i].second.second;
				int target_entity = -1;
				for(int j = 0; j < entity_list.size(); j ++) if(entity_list[j].type == 0 && entity_list[j].threat_for != 2 && entity_list[j].dealt_with == 0){
					ll get_distance = (entity_list[j].x - herox) * 1ll * (entity_list[j].x - herox) + (entity_list[j].y - heroy) * 1ll * (entity_list[j].y - heroy);
					ll get_base_distance = (entity_list[j].x - base_x) * 1ll * (entity_list[j].x - base_x) + (entity_list[j].y - base_y) * 1ll * (entity_list[j].y - base_y);
					if(get_base_distance > 5500ll * 5500ll && distance > get_distance){
						distance = get_distance;
						target_entity = j;
					}
				}
				if(target_entity != -1){
						entity_list[target_entity].dealt_with = 1;
						x = entity_list[target_entity].x;
						y = entity_list[target_entity].y;
				}

				cout << "MOVE " << x << " " << y << endl;
			}

			// attacking
			for(int i = heroes_per_player - 1; i < heroes_per_player; i ++){
				int x = (base_x + dx3[i + 1] <= 17630 ? base_x + dx3[i + 1] : base_x - dx3[i + 1]), y = (base_y + dy3[i + 1] <= 9000 ? base_y + dy3[i + 1] : base_y - dy3[i + 1]), threatfor = 0, nearbase = 0;
				ll distance = inf; int target_entity = -1;
				ll base_distance = inf;
				int herox = li[i].second.first, heroy = li[i].second.second;
				vector<pair<pair<ll, ll>, int> > lis;
				for(int j = 0; j < entity_list.size(); j ++) if(entity_list[j].type == 0){
					ll get_distance = (entity_list[j].x - herox) * 1ll * (entity_list[j].x - herox) + (entity_list[j].y - heroy) * 1ll * (entity_list[j].y - heroy);
					ll get_base_distance = (entity_list[j].x - base_x) * 1ll * (entity_list[j].x - base_x) + (entity_list[j].y - base_y) * 1ll * (entity_list[j].y - base_y);
					lis.push_back(make_pair(make_pair(get_base_distance, get_distance), entity_list[j].shield_life));
					if(entity_list[j].threat_for == 1){
						if(!threatfor) target_entity = -1, base_distance = inf;
						threatfor = 1;
						if(entity_list[j].near_base){
								if(!nearbase) target_entity = -1, base_distance = inf;
								nearbase = 1;
								
								if(get_base_distance < base_distance){
										base_distance = get_base_distance;
										target_entity = j;
								}else{
										if(target_entity == -1){
												base_distance = get_base_distance;
												target_entity = j;
										}
								}
						}else{
							if(nearbase == 1) continue;
							if(get_base_distance < base_distance){
								base_distance = get_base_distance;
								target_entity = j;
							}else{
								if(target_entity == -1){
									base_distance = get_base_distance;
									target_entity = j;
								}
							}
						}
					}else{
						if(threatfor == 1) continue;
						if(entity_list[j].threat_for == 0){
							// farm mana
							if(get_base_distance < base_distance){
								base_distance = get_base_distance;
								target_entity = j;
							}else{
								if(target_entity == -1){
									base_distance = get_base_distance;
									target_entity = j;
								}
							}
						}
					}
				}
				sort(lis.begin(), lis.end());

				if(base_distance >= 9000ll * 9000ll) target_entity = -1;

				bool use_wind = 0;
				if(target_entity != -1){
						x = entity_list[target_entity].x;
						y = entity_list[target_entity].y;
						bool can_wind = 0;
						for(pair<pair<ll, ll>, int> i : lis) if(i.first.first <= 1100ll * 1100ll && i.second == 0) can_wind = 1;
						if(our_mana >= 10 && can_wind){
								use_wind = 1;
						}
				}
				if(use_wind){
						cout << "SPELL WIND " << opp_x << " " << opp_y << endl;
						wind_done_defending = 1;
				}else
						cout << "MOVE " << x << " " << y << endl;
			}
			continue;
		}

		if(!stationed_to_position){
			set<int> controlled;
			for(int i = 0; i < 3; i ++){
				ll distance = inf, base_distance = inf; int target_entity = -1;
				int herox = li[i].second.first, heroy = li[i].second.second;
				for(int j = 0; j < entity_list.size(); j ++) if(entity_list[j].type == 0 && !controlled.count(j)){
					ll get_distance = (entity_list[j].x - herox) * 1ll * (entity_list[j].x - herox) + (entity_list[j].y - heroy) * 1ll * (entity_list[j].y - heroy);
			  		ll get_base_distance = (entity_list[j].x - base_x) * 1ll * (entity_list[j].x - base_x) + (entity_list[j].y - base_x) * 1ll * (entity_list[j].y - base_x);
			  		if(get_distance <= 2000 * 2000 && entity_list[j].threat_for == 1 && our_mana >= 100){
			  			if(base_distance > get_base_distance){
			  				base_distance = get_base_distance;
			  				distance = get_distance;
			  				target_entity = j;
			  			}
			  		}
				}
				if(target_entity == -1) cout << "MOVE " << dx2[next_base] << " " << dy2[next_base] << endl;
				else{
					if(entity_list[target_entity].threat_for == 1 && entity_list[target_entity].near_base == 1){
						cout << "SPELL WIND " << opp_x << " " << opp_y << endl;
					}else{
						cout << "SPELL CONTROL " << entity_list[target_entity].id << " " << dx2[next_base] << " " << dy2[next_base] << endl;
						controlled.insert(target_entity);
					}
				}
			}
		}else if(stationed_to_position == 1){
			bool only_wind_once = 0;
			for(int i = 0; i < 3; i ++){
				ll distance = inf, base_distance = inf; int target_entity = -1, nearest_entity = -1;
				int herox = li[i].second.first, heroy = li[i].second.second;
			  	for(int j = 0; j < entity_list.size(); j ++) if(entity_list[j].type == 0){
			  		ll get_distance = (entity_list[j].x - herox) * 1ll * (entity_list[j].x - herox) + (entity_list[j].y - heroy) * 1ll * (entity_list[j].y - heroy);
			  		ll get_base_distance = (entity_list[j].x - opp_x) * 1ll * (entity_list[j].x - opp_x) + (entity_list[j].y - opp_y) * 1ll * (entity_list[j].y - opp_y);
			  		if(get_distance <= 1250 * 1250){
			  			if(base_distance > get_base_distance){
			  				base_distance = get_base_distance;
			  				target_entity = j;
			  			}
			  		}
					if(distance > get_distance){
						distance = get_distance;
						nearest_entity = j;
					}
		  	}

		  	if(target_entity == -1){
				ll get_base_distance = (entity_list[nearest_entity].x - opp_x) * 1ll * (entity_list[nearest_entity].x - opp_x) + (entity_list[nearest_entity].y - opp_y) * 1ll * (entity_list[nearest_entity].y - opp_y);
				if(get_base_distance <= 9000 * 9000)
		  			cout << "MOVE " << entity_list[nearest_entity].x << " " << entity_list[nearest_entity].y << endl;
				else
					cout << "MOVE " << dx2[next_base] << " " << dy2[next_base] << endl;
		  	}else{
		  		if(only_wind_once){
		  			cout << "WAIT" << endl;
		  			continue;
		  		}

		  		if(base_distance >= 7200 * 7200){
		  			only_wind_once = 1;
		  		}
		  		cout << "SPELL WIND " << opp_x << " " << opp_y << endl;
		  	}
			}
		}		
		turn_left --;
	}
}