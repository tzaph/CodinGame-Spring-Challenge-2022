/*
League: Gold (rank 8th in Gold, rank 76th global)
Spells used: All spells
Tactic: True StartEnd S2
	This is the start and this is the end ...season 2!

- Optimising True StartEnd S1
- Calculate the mana usage for attacking that turn before doing defense, as attacking is the main point of this strategy and takes priority
	- Simulate the attacking strategy beforehand
	- Defense now can only use spells if mana >= attack_mana_usage + 10
- Now allowed to control up to 5 monsters for attack (it was 1 before)
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
int dd[] = {0, -1000, -1000, 0, 1000, 1000};

int heroes_per_player;
entity_info ourBase, oppBase;
int number_of_turns;
bool phase2; // attack phase

int mana, health, entity_count;
vector<entity_info> entity_list;
vector<pair<int, pair<int, int> > > lis;

void defense0_pattern();
void farming12_pattern();
void attack12_pattern();
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
            dx[1] -= dd[number_of_turns % 6];
            dx[2] += dd[number_of_turns % 6];
        }else{
            dx[1] += dd[number_of_turns % 6];
            dx[2] -= dd[number_of_turns % 6];
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

		if(phase2) mana_cap = simulate_attack12_pattern();
        else mana_cap = 0;
        cerr << mana_cap << endl;
		defense0_pattern();
		if(phase2) attack12_pattern();
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

	if(phase2 && base_distance >= 6500 * 6500) target_entity = -1;
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

		if(control_target != -1 && phase2 && mana >= health * 20 && mana >= mana_cap + 10)
			cout << "SPELL CONTROL " << entity_list[control_target].id << " " << oppBase.x << " " << hero.y << endl;
		else
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

void apply_shield_when_necessary(int id){
	bool alerting = 0;
	for(int j = 0; j < entity_count; j ++) if(entity_list[j].type == 2){
		ll distance = distsq(entity_list[j], entity_list[id]);
		if(distance <= 1300 * 1300) alerting = 1;
	}
	if(alerting && mana >= 70 && entity_list[id].shield_life == 0)
		cout << "SPELL SHIELD " << entity_list[id].id << endl;
	else
		cout << "MOVE " << dx[3] << " " << dy[3] << endl;
}

void attack12_pattern(){
	bool in_position = 1;
	for(int i = 1; i <= 2; i ++)
		if(entity_list[heroID[i]].x != dx[3] || entity_list[heroID[i]].y != dy[3])
			in_position = 0;

	if(!in_position){
		for(int i = 1; i <= 2; i ++){
			entity_info hero = entity_list[heroID[i]];
			int try_to_control = -1;
			for(int j = 0; j < entity_count; j ++) if(entity_list[j].type == 0){
				ll get_distance = distsq(entity_list[j], hero);
				if(get_distance <= 2200 * 2200 && !controlled_monsters.count(entity_list[j].id) && controlled_monsters.size() < 5){
					try_to_control = j;
					break;
				}	
			}
			if(try_to_control != -1 && mana >= 40){
				cout << "SPELL CONTROL " << entity_list[try_to_control].id << " " << dx[4] << " " << dy[4] << endl;
				controlled_monsters.insert(entity_list[try_to_control].id);
			}else
				cout << "MOVE " << dx[3] << " " << dy[3] << endl;
		}
		return;
	}

	set<int> update_controlled_monsters;
	for(int j = 0; j < entity_count; j ++) if(controlled_monsters.count(entity_list[j].id) && entity_list[j].opp_base_distance > 5000 * 5000)
		update_controlled_monsters.insert(entity_list[j].id);
	controlled_monsters = update_controlled_monsters;

    for(int i : controlled_monsters) cerr << i << " "; cerr << endl;

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
		cout << "SPELL WIND " << oppBase.x << " " << entity_list[heroID[1]].y << endl;
		cout << "SPELL WIND " << oppBase.x << " " << entity_list[heroID[1]].y << endl;
	}else if(back){
		cout << "SPELL WIND " << oppBase.x << " " << entity_list[heroID[1]].y << endl;
		apply_shield_when_necessary(2);
	}else{
		ll distance = inf; int target_entity = -1;
		for(int j = 0; j < entity_count; j ++) if(entity_list[j].type == 0){
			ll get_distance = distsq(entity_list[j], entity_list[heroID[1]]);
            cerr << get_distance << " " << controlled_monsters.size() << " " << entity_list[j].opp_base_distance << endl;
			if(get_distance <= 2200 * 2200 && !controlled_monsters.count(entity_list[j].id) && controlled_monsters.size() < 5 && entity_list[j].opp_base_distance > 5000 * 5000){
				if(distance > get_distance){
                    cerr << "new target " << entity_list[j].id << endl;
					distance = get_distance;
					target_entity = j; 
				}
			}	
		}
        cerr << target_entity << " " << mana << " " << health << endl;
		if(target_entity != -1 && mana >= 40){
            cerr << "vibe check" << endl;
			cout << "SPELL CONTROL " << entity_list[target_entity].id << " " << dx[4] << " " << dy[4] << endl;
			controlled_monsters.insert(entity_list[target_entity].id);
		}else{
			apply_shield_when_necessary(1);
		}
		apply_shield_when_necessary(2);
	}
	return;
}

int simulate_apply_shield_when_necessary(int id){
	bool alerting = 0;
	for(int j = 0; j < entity_count; j ++) if(entity_list[j].type == 2){
		ll distance = distsq(entity_list[j], entity_list[id]);
		if(distance <= 1300 * 1300) alerting = 1;
	}
	if(alerting && mana >= 60 && entity_list[id].shield_life == 0)
		return 10;
	return 0;
}

int simulate_attack12_pattern(){
	int mana_used = 0;
	bool in_position = 1;
	for(int i = 1; i <= 2; i ++)
		if(entity_list[heroID[i]].x != dx[3] || entity_list[heroID[i]].y != dy[3])
			in_position = 0;

	if(!in_position){
		for(int i = 1; i <= 2; i ++){
			entity_info hero = entity_list[heroID[i]];
			int try_to_control = -1;
			for(int j = 0; j < entity_count; j ++) if(entity_list[j].type == 0){
				ll get_distance = distsq(entity_list[j], hero);
				if(get_distance <= 2200 * 2200 && !controlled_monsters.count(entity_list[j].id) && controlled_monsters.size() < 5){
					try_to_control = j;
					break;
				}	
			}
			if(try_to_control != -1 && mana >= 40) mana_used += 10;
		}
		return mana_used;
	}

	set<int> update_controlled_monsters;
	for(int j = 0; j < entity_count; j ++) if(controlled_monsters.count(entity_list[j].id) && entity_list[j].opp_base_distance > 5000 * 5000)
		update_controlled_monsters.insert(entity_list[j].id);

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
		return 21;
	}else if(back){
		return 10 + simulate_apply_shield_when_necessary(2) + 21;
	}else{
		ll distance = inf; int target_entity = -1;
		for(int j = 0; j < entity_count; j ++) if(entity_list[j].type == 0){
			ll get_distance = distsq(entity_list[j], entity_list[heroID[1]]);
			if(get_distance <= 2200 * 2200 && !update_controlled_monsters.count(entity_list[j].id) && update_controlled_monsters.size() < 5 && entity_list[j].opp_base_distance > 5000 * 5000){
				if(distance > get_distance){
					distance = get_distance;
					target_entity = j; 
				}
			}	
		}

		if(target_entity != -1 && mana >= 40){
           	mana_used += 10;
		}else{
			mana_used += simulate_apply_shield_when_necessary(1);
		}
		mana_used += simulate_apply_shield_when_necessary(2);
		return mana_used;
	}

	return 0;
}