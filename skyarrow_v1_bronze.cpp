/*
League: Bronze (created before silver league was opened) [best rank 69/1793 15 hours after start]
Spells used: All spells
Tactic: Sky Arrow
Simply one defender, one farmer, and one attacker.
*/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

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

int dx[] = {3000, 17630 / 2, 17630 - 3500};
int dy[] = {3000, 9000 / 2, 9000 - 3500};

// first and second hero for defending base; use WIND OPPBASE 
// third hero attacks opp base by pushing spiders to opp base; use WIND OPPBASE, SHIELD (threatfor 2), CONTROL OPPBASE (threatfor != 2, close enough to opp base)

int main(){
    int base_x; // The corner of the map representing your base
    int base_y;
    cin >> base_x >> base_y;
    int heroes_per_player; // Always 3
    cin >> heroes_per_player; 

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

        for(int i = 0; i < entity_count; i ++){
            entity_info A;
            A.read();
            A.to_base_distance = (base_x - A.x) * 1ll * (base_x - A.x) + (base_y - A.y) * 1ll * (base_y - A.y);
            if(A.type == 1) li.push_back(make_pair(A.id, make_pair(A.x, A.y)));
            if(A.type == 0 && A.threat_for == 1){
                A.need_help = 1;
                if(A.to_base_distance <= 6000ll * 6000) A.need_help ++;
                if(A.to_base_distance <= 3000ll * 3000) A.need_help ++;
            }
            entity_list.push_back(A);
        }
        sort(li.begin(), li.end());

        // defending
        bool wind_done_defending = 0;
        for (int i = 0; i < heroes_per_player - 2; i++){
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

        // farming
        for(int i = heroes_per_player - 2; i < heroes_per_player - 1; i ++){
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

        // attacking
        for(int i = heroes_per_player - 1; i < heroes_per_player; i ++){
            int x = (base_x + dx[i] <= 17630 ? base_x + dx[i] : base_x - dx[i]), y = (base_y + dy[i] <= 9000 ? base_y + dy[i] : base_y - dy[i]);   
            int herox = li[i].second.first, heroy = li[i].second.second;
            if(herox != x && heroy != y) cout << "MOVE " << x << " " << y << endl;
            else{
                bool what_to_do = 0; // false: control, true: shield
                int target_entity = -1;
                for(int j = 0; j < entity_list.size(); j ++) if(entity_list[j].type == 0){
                    ll get_distance = (entity_list[j].x - herox) * 1ll * (entity_list[j].x - herox) + (entity_list[j].y - heroy) * 1ll * (entity_list[j].y - heroy);
                    ll get_base_distance = (entity_list[j].x - base_x) * 1ll * (entity_list[j].x - base_x) + (entity_list[j].y - base_y) * 1ll * (entity_list[j].y - base_y);
                    if(get_distance <= 2100ll * 2100ll){
                        if(entity_list[j].threat_for == 2 && entity_list[j].shield_life == 0){
                            what_to_do = 1; target_entity = j;
                            break;
                        }else if(entity_list[j].threat_for != 2){
                            what_to_do = 0; target_entity = j;
                            break;
                        }   
                    }
                }

                if(what_to_do && our_mana >= 20){
                    cout << "SPELL SHIELD " << entity_list[target_entity].id << endl;
                }else if(!what_to_do){
                    cout << "SPELL CONTROL " << entity_list[target_entity].id << " " << 17630 - base_x << " " << 9000 - base_y << endl;
                }else{
                    cout << "WAIT" << endl;
                }
            }
        }
    }
}