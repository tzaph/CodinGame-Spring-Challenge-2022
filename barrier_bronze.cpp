/*
League: Bronze [best 47th out of 1638, 3 hours after challenge opened]
Spells used: Wind
Tactic: Barrier
Guard around the base. Move to a monster closest to hero and more heroes target monsters htat are closer to base. 
Use wind spell for one of the heroes if monster is very close to center of base. Set the wind spell away from base.
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

int dx[] = {2000, 3000, 4000};
int dy[] = {4000, 3000, 2000};

int main(){
    int base_x; // The corner of the map representing your base
    int base_y;
    cin >> base_x >> base_y; 
    int heroes_per_player; // Always 3
    cin >> heroes_per_player; 

    // game loop
    int turn = 0;
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
        
        bool wind_done = 0;
        for (int i = 0; i < heroes_per_player; i++){
            int x = (base_x + dx[i] <= 17630 ? base_x + dx[i] : base_x - dx[i]), y = (base_y + dy[i] <= 9000 ? base_y + dy[i] : base_y - dy[i]), threatfor = 0, nearbase = 0;
            ll distance = inf; int target_entity = -1;
            int herox = li[i].second.first, heroy = li[i].second.second;
            for(int j = 0; j < entity_list.size(); j ++) if(entity_list[j].type == 0 && entity_list[j].dealt_with != entity_list[j].need_help){
                if(entity_list[j].threat_for == 1){
                    if(!threatfor) target_entity = -1, distance = inf;
                    threatfor = 1;
                    if(entity_list[j].near_base){
                        if(!nearbase) target_entity = -1, distance = inf;
                        nearbase = 1;
                        ll get_distance = (entity_list[j].x - herox) * 1ll * (entity_list[j].x - herox) + (entity_list[j].y - heroy) * 1ll * (entity_list[j].y - heroy);
                        if(get_distance < distance){
                            distance = get_distance;
                            target_entity = j;
                        }else{
                            if(target_entity == -1){
                                distance = get_distance;
                                target_entity = j;
                            }else{
                                if(entity_list[target_entity].need_help < entity_list[j].need_help){
                                    distance = get_distance;
                                    target_entity = j;
                                }
                            }
                        }
                    }else{
                        if(nearbase == 1) continue;
                        ll get_distance = (entity_list[j].x - herox) * 1ll * (entity_list[j].x - herox) + (entity_list[j].y - heroy) * 1ll * (entity_list[j].y - heroy);
                        if(get_distance < distance){
                            distance = get_distance;
                            target_entity = j;
                        }else{
                            if(target_entity == -1){
                                distance = get_distance;
                                target_entity = j;
                            }else{
                                if(entity_list[target_entity].need_help < entity_list[j].need_help){
                                    distance = get_distance;
                                    target_entity = j;
                                }
                            }
                        }
                    }
                }else{
                    if(threatfor == 1) continue;
                    if(entity_list[j].threat_for == 0){
                        // farm mana
                        ll get_distance = (entity_list[j].x - herox) * 1ll * (entity_list[j].x - herox) + (entity_list[j].y - heroy) * 1ll * (entity_list[j].y - heroy);
                        if(get_distance < distance){
                            distance = get_distance;
                            target_entity = j;
                        }else{
                            if(target_entity == -1){
                                distance = get_distance;
                                target_entity = j;
                            }else{
                                if(entity_list[target_entity].need_help < entity_list[j].need_help){
                                    distance = get_distance;
                                    target_entity = j;
                                }
                            }
                        }
                    }
                }
            }

            bool use_wind = 0;
            if(target_entity != -1){
                entity_list[target_entity].dealt_with ++;
                x = entity_list[target_entity].x;
                y = entity_list[target_entity].y;
                if(wind_done == 0 && our_mana >= 10 && distance <= 800ll * 800ll){
                    use_wind = 1;
                    wind_done = 1;
                }
            }
            if(use_wind){
                cout << "SPELL WIND " << 17630 - base_x << " " << 9000 - base_y << endl;
            }else
                cout << "MOVE " << x << " " << y << endl;
        }
        turn ++;
    }
}