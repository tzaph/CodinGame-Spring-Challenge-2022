/*
League: Gold -> Legend
Spells used: All spells
Tactic: True StartEnd S3
	This is the start and this is the end ...season 3!!

- Optimising True StartEnd S2
- Merging the attacking simulation function and the actual attacking query function
- When controlling the monsters to the designated destination, we make sure that the monsters will not reach the destination before we do
	- Calculate distance hero to (dx[3], dy[3]) and monster to (dx[4], dy[4]), divide by speed
	- If max(distHero) + 2 <= distMonster, we can safely say that this monster will be of use for attacking
	- +2 is to compensate cases where heroes stop to control more monsters
- Removed control gimmick from defense as it is ineffective and wastes mana
- Retweaking hardcoded values
*/

#include <bits/stdc++.h>
using namespace std;

#define ll long long

const ll inf = 1e12 + 69;
const int maxX = 17630;
const int maxY = 9000;

struct entity_info{
	int id, type; // unique identifier, type = 0monster 1ally 2opp
	int x, y; 
	int shield_life, is_controlled; // ignore
	int health;
	int vx, vy;
	int near_base, threat_for;
	ll our_base_distance, opp_base_distance;
	int dealt_with;

	void read(){
		cin >> id >> type >> x >> y >> shield_life >> is_controlled >> health >> vx >> vy >> near_base >> threat_for;
		our_base_distance = opp_base_distance = dealt_with = 0;
	}

	void print(){
		cerr << id << " " << type << " " << x << " " << y << " " << shield_life << " " << is_controlled << " " << health << " " << vx << " " << vy << " " << near_base << " " << threat_for << " " << our_base_distance << " " << opp_base_distance << endl;
	}
};

inline ll distsq(entity_info A, entity_info B){
	return (A.x - B.x) * 1ll * (A.x - B.x) + (A.y - B.y) * 1ll * (A.y - B.y);
}

int dx[] = {4200, maxX / 2, maxX / 2, maxX - 5500, maxX - 6540};
int dy[] = {4200, maxY / 4, 3 * maxY / 4, maxY - 200, maxY - 200};
int dd[] = {0, -1000, -1000, -1000, 0, 1000, 1000, 1000};

int heroes_per_player;
entity_info ourBase, oppBase;
int number_of_turns;
bool phase2; // attack phase

int mana, health, entity_count;
vector<entity_info> entity_list;
vector<pair<int, pair<int, int> > > lis;

void defense0_pattern();
void farming12_pattern();
int attack12_pattern(int type);
int simulate_attack12_pattern();

vector<int> heroID;
int mana_cap = 0;

int main(){
	int base_x, base_y; cin >> base_x >> base_y;
	ourBase.x = base_x; ourBase.y = base_y;
	oppBase.x = maxX - base_x; oppBase.y = maxY - base_y;
	ourBase.our_base_distance = 0; ourBase.opp_base_distance = distsq(ourBase, oppBase);
	oppBase.opp_base_distance = 0; oppBase.our_base_distance = distsq(ourBase, oppBase);

	if(base_x == maxX){
		for(int i = 0; i < 5; i ++){
			dx[i] = maxX - dx[i];
			dy[i] = maxY - dy[i];
		}
	}

	cin >> heroes_per_player;
	number_of_turns = 1;
	phase2 = 0;
	for(;; number_of_turns ++){
		entity_list.clear(); lis.clear(); heroID.clear();
        
        if(ourBase.x == 0){
            dx[1] -= dd[number_of_turns % 8];
            dx[2] += dd[number_of_turns % 8];
        }else{
            dx[1] += dd[number_of_turns % 8];
            dx[2] -= dd[number_of_turns % 8];
        }

		health = 0;
		for(int tmpHealth, tmpMana, i = 0; i < 2; i ++){
			cin >> tmpHealth >> tmpMana;
			health += tmpHealth;
			if(i == 0) mana = tmpMana;
		}

		cin >> entity_count;
		for(int i = 0; i < entity_count; i ++){
			entity_info A; A.read();
			A.our_base_distance = distsq(A, ourBase);
			A.opp_base_distance = distsq(A, oppBase);
			A.print();
			if(A.type == 1) heroID.push_back(i);
			entity_list.push_back(A);
		}
		
		if(mana >= 160) phase2 = 1;

		if(phase2) mana_cap = attack12_pattern(1);
		else mana_cap = 0;
		defense0_pattern();
		if(phase2) attack12_pattern(0);
		else farming12_pattern();
	}

	return 0;
}

void defense0_pattern(){
	entity_info hero = entity_list[heroID[0]];
	int x = dx[0], y = dy[0];

	ll distance = inf, base_distance = inf;
	int target_entity = -1;
	bool use_wind = 0, urgent_wind = 0;
	bool threat = 0, near = 0;

	for(int j = 0; j < entity_count; j ++) if(entity_list[j].type == 0 && entity_list[j].dealt_with == 0){
		ll get_distance = distsq(entity_list[j], hero);
		if(entity_list[j].shield_life == 0 && entity_list[j].our_base_distance <= 3000 * 3000 && get_distance <= 1280 * 1280){
			use_wind = 1;
			if(entity_list[j].our_base_distance <= 1000 * 1000 && entity_list[j].health > 2)
				urgent_wind = 1;
		}

		if(entity_list[j].threat_for == 1){
			if(threat == 0){
				target_entity = -1;
				distance = base_distance = inf;
				threat = 1;
			}

			if(entity_list[j].near_base == 1){
				if(near == 0){
					target_entity = -1;
					distance = base_distance = inf;
					near = 1;
				}

				if(target_entity == -1 || entity_list[j].our_base_distance < base_distance){
					base_distance = entity_list[j].our_base_distance;
					distance = get_distance;
					target_entity = j;
				}
			}else{
				if(near == 1) continue;

				if(target_entity == -1 || entity_list[j].our_base_distance < base_distance){
					base_distance = entity_list[j].our_base_distance;
					distance = get_distance;
					target_entity = j;
				}
			}
		}else{
			if(threat == 1) continue;

			if(target_entity == -1 || entity_list[j].our_base_distance < base_distance){
				base_distance = entity_list[j].our_base_distance;
				distance = get_distance;
				target_entity = j;
			}
		}
	}

	if(phase2 && base_distance >= 6000 * 6000) target_entity = -1;
	if(!phase2 && base_distance >= 9000 * 9000) target_entity = -1;

	if(target_entity != -1){
		entity_list[target_entity].dealt_with = 1;
		x = entity_list[target_entity].x;
		y = entity_list[target_entity].y;
	}

	if((urgent_wind && mana_cap % 10 != 1) || (use_wind && phase2 && mana >= health * 20 && mana >= mana_cap + 10)){
		cout << "SPELL WIND " << hero.x << " " << oppBase.y << endl;
	}else{
		int control_target = -1;
		for(int j = 0; j < entity_count; j ++) if(entity_list[j].type == 2 && !entity_list[j].is_controlled){
			ll get_distance = distsq(entity_list[j], entity_list[heroID[0]]);
			if(get_distance <= 2200 * 2200){
				control_target = j;
				break;
			}
		}

		cout << "MOVE " << x << " " << y << endl;
	}
}

void farming12_pattern(){
	for(int i = 1; i <= 2; i ++){
		entity_info hero = entity_list[heroID[i]];
		int x = dx[i], y = dy[i];
		ll distance = inf; int target_entity = -1;

		for(int j = 0; j < entity_count; j ++) if(entity_list[j].type == 0 && entity_list[j].dealt_with == 0){
			ll get_distance = distsq(entity_list[j], hero);
			if(entity_list[j].our_base_distance > 5500 * 5500 && distance > get_distance){
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
}

set<int> controlled_monsters;

int apply_shield_when_necessary(int id, int type){
	bool alerting = 0;
	for(int j = 0; j < entity_count; j ++) if(entity_list[j].type == 2){
		ll distance = distsq(entity_list[j], entity_list[id]);
		if(distance <= 1300 * 1300) alerting = 1;
	}
	if(alerting && mana >= 70 && entity_list[id].shield_life == 0){
		if(type) return 10;
		cout << "SPELL SHIELD " << entity_list[id].id << endl;
	}
	else{
		if(type) return 0;
		cout << "MOVE " << dx[3] << " " << dy[3] << endl;
	}
	return 0;
}

int attack12_pattern(int type = 0){ //type 0 = print, type 1 = simulate
	bool in_position = 1;
	int mana_use = 0;
	for(int i = 1; i <= 2; i ++)
		if(entity_list[heroID[i]].x != dx[3] || entity_list[heroID[i]].y != dy[3])
			in_position = 0;

	if(!in_position){
		int estimated_time_reach = 0;
		for(int i = 1; i <= 2; i ++){
			entity_info hero = entity_list[heroID[i]];
			ll get_distance = (entity_list[heroID[i]].x - dx[3]) * (entity_list[heroID[i]].x - dx[3]) + (entity_list[heroID[i]].y - dy[3]) * (entity_list[heroID[i]].y - dy[3]);
			get_distance = (ll)sqrtl(get_distance);
			get_distance = (get_distance + 799) / 800;
			estimated_time_reach = max(estimated_time_reach, (int)get_distance);
		}
		estimated_time_reach += 2;

		for(int i = 1; i <= 2; i ++){
			entity_info hero = entity_list[heroID[i]];
			int try_to_control = -1;
			for(int j = 0; j < entity_count; j ++) if(entity_list[j].type == 0){
				ll estimated_time = (entity_list[j].x - dx[3]) * (entity_list[j].x - dx[3]) + (entity_list[j].y - dy[3]) * (entity_list[j].y - dy[3]);
				estimated_time = (ll)sqrtl(estimated_time);
				estimated_time = (estimated_time + 399) / 400;
				if(estimated_time < estimated_time_reach) continue;

				ll get_distance = distsq(entity_list[j], hero);
				if(get_distance <= 2200 * 2200 && !controlled_monsters.count(entity_list[j].id) && controlled_monsters.size() < 5){
					try_to_control = j;
					break;
				}	
			}
			if(try_to_control != -1 && mana >= 40){
				if(type) mana_use += 10;
				else{
					cout << "SPELL CONTROL " << entity_list[try_to_control].id << " " << dx[4] << " " << dy[4] << endl;
					controlled_monsters.insert(entity_list[try_to_control].id);
				}
			}else{
				if(type) mana_use += 0;
				else cout << "MOVE " << dx[3] << " " << dy[3] << endl;
			}
		}
		return mana_use;
	}

	set<int> update_controlled_monsters;
	for(int j = 0; j < entity_count; j ++) if(controlled_monsters.count(entity_list[j].id) && entity_list[j].opp_base_distance > 5000 * 5000)
		update_controlled_monsters.insert(entity_list[j].id);
	if(!type) controlled_monsters = update_controlled_monsters;

	bool front = 0, back = 0;
	for(int j = 0; j < entity_count; j ++) if(entity_list[j].type == 0){
		ll get_distance = distsq(entity_list[heroID[1]], entity_list[j]);
		if(get_distance <= 1280 * 1280 && abs(entity_list[j].y - entity_list[heroID[1]].y) <= 200 && abs(entity_list[j].x - entity_list[heroID[1]].x) >= 920){
			if(entity_list[heroID[1]].opp_base_distance < entity_list[j].opp_base_distance){
				back = 1;
			}else{
				front = 1;
			}
		}
	}

	if(front){
		if(type){
			return 21;
		}
		cout << "SPELL WIND " << oppBase.x << " " << entity_list[heroID[1]].y << endl;
		cout << "SPELL WIND " << oppBase.x << " " << entity_list[heroID[1]].y << endl;
	}else if(back){
		if(type){
			return 31 + apply_shield_when_necessary(2, type);
		}
		cout << "SPELL WIND " << oppBase.x << " " << entity_list[heroID[1]].y << endl;
		apply_shield_when_necessary(2, type);
	}else{
		ll distance = inf; int target_entity = -1;
		for(int j = 0; j < entity_count; j ++) if(entity_list[j].type == 0){
			ll get_distance = distsq(entity_list[j], entity_list[heroID[1]]);
			if(type){
				if(get_distance <= 2200 * 2200 && !update_controlled_monsters.count(entity_list[j].id) && update_controlled_monsters.size() < 5 && entity_list[j].opp_base_distance > 5000 * 5000){
					if(distance > get_distance){
						distance = get_distance;
						target_entity = j; 
					}
				}	
			}else{
				if(get_distance <= 2200 * 2200 && !controlled_monsters.count(entity_list[j].id) && controlled_monsters.size() < 5 && entity_list[j].opp_base_distance > 5000 * 5000){
					if(distance > get_distance){
						distance = get_distance;
						target_entity = j; 
					}
				}	
			}
		}

		if(target_entity != -1 && mana >= 40){
			if(type) mana_use += 10;
			else{
				cout << "SPELL CONTROL " << entity_list[target_entity].id << " " << dx[4] << " " << dy[4] << endl;
				controlled_monsters.insert(entity_list[target_entity].id);
			}
		}else{
			mana_use += apply_shield_when_necessary(1, type);
		}
		mana_use += apply_shield_when_necessary(2, type);
	}
	return mana_use;
}