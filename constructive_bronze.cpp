/*
League: Bronze (created before silver league was opened) [120/1,741 12 hours after challenge opened]
Spells used: Wind (control usage to be explored)
Tactic: Constructive
Two heroes roam in the middle of the map to farm mana from killing spiders:
- Moving to the closest creep
- Control a spider to come closer to the heroes (to be explored in detail)
One hero guards center of base by only using wind spell, repelling every creep away
*/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

#define ll long long
const ll inf = 500000000ll;
const int maxX = 17630;
const int maxY = 9000;

struct entity_info{
		int id, type; // unique identifier, type = 0monster 1ally 2opp
		int x, y; 
		int shield_life, is_controlled; // ignore
		int health;
		int vx, vy;
		int near_base, threat_for;
		ll to_base_distance;
		bool dealt_with;

		void read(){
				cin >> id >> type >> x >> y >> shield_life >> is_controlled >> health >> vx >> vy >> near_base >> threat_for;
				to_base_distance = 0;
				dealt_with = 0;
		}
};

int dx[] = {maxX / 2 + 600, maxX / 2 - 600, 3000};
int dy[] = {maxY / 2 - 600, maxY / 2 + 600, 3000};

int main(){
		int base_x; // The corner of the map representing your base
		int base_y;
		cin >> base_x >> base_y;
		int heroes_per_player; // Always 3
		cin >> heroes_per_player;

		// game loop
		int turns_left = 220;
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
				for(int i = 0; i < entity_count; i ++){
						entity_info A;
						A.read();
						A.to_base_distance = (base_x - A.x) * 1ll * (base_x - A.x) + (base_y - A.y) * 1ll * (base_y - A.y);
						if(A.type == 1) li.push_back(make_pair(A.id, make_pair(A.x, A.y)));
						entity_list.push_back(A);
				}
				sort(li.begin(), li.end());

				for(int i = 0; i < heroes_per_player - 1; i ++){
					int x = (base_x + dx[i] <= 17630 ? base_x + dx[i] : base_x - dx[i]), y = (base_y + dy[i] <= 9000 ? base_y + dy[i] : base_y - dy[i]);
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

				for(int i = heroes_per_player - 1; i < heroes_per_player; i ++){
					int x = (base_x + dx[i] <= 17630 ? base_x + dx[i] : base_x - dx[i]), y = (base_y + dy[i] <= 9000 ? base_y + dy[i] : base_y - dy[i]), threatfor = 0, nearbase = 0;
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

					if(base_distance >= 5500ll * 5500ll) target_entity = -1;

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
							cout << "SPELL WIND " << 17630 - base_x << " " << 9000 - base_y << endl;
					}else
							cout << "MOVE " << x << " " << y << endl;
				}
				turns_left --;
		}
}