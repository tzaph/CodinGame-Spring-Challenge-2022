/*
League: Silver -> Gold
Spells used: All spells
Tactic: Sky Arrow V4
Simply one defender, one farmer, and one attacker.
- Instead of farmer helping to defend, farmer attacks with attacker
- Attacks now use wind instead of shield, abuse wind stacking
*/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>
#include <set>

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

int dx[] = {4000, 17630 / 2, 14000, 5000};
int dy[] = {2000, 9000 / 2, 5000, 5000};

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
    if(opp_x > 500){
        for(int i = 0; i < 13; i ++){
            tx[i] = opp_x - tx[i];
            ty[i] = opp_y - ty[i];
        }
    }

    // game loop
    int turn_left = 220;
    for(;;){
        if(turn_left % 2 == 0);
        else if(2 <= turn_left % 8 && turn_left % 8 <= 6) dx[2] += 2000, dy[2] -= 2000;
        else dx[2] -= 2000, dy[2] += 2000;

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
            if(turn_left <= 170){
                int x = (base_x + dx[i] <= 17630 ? base_x + dx[i] : base_x - dx[i]), y = (base_y + dy[i] <= 9000 ? base_y + dy[i] : base_y - dy[i]), threatfor = 0, nearbase = 0;
                ll distance = inf; int target_entity = -1;
                ll base_distance = inf;
                int herox = li[i].second.first, heroy = li[i].second.second;
                int idhero = li[i].first;

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
                        cout << "SPELL WIND " << opp_x << " " << opp_y << endl;
                        wind_done_defending = 1;
                }else
                        cout << "MOVE " << x << " " << y << endl;
                continue;
            }

            int x = (base_x + (dx[i + 1] + 1000) <= 17630 ? base_x + (dx[i + 1] + 1000) : base_x - (dx[i + 1] + 1000)), y = (base_y + (dy[i + 1] - 1000) <= 9000 ? base_y + (dy[i + 1] - 1000) : base_y - (dy[i + 1] - 1000));
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

        set<int> controlled;
        // farming/defending
        for(int i = heroes_per_player - 2; i < heroes_per_player - 1; i ++){
            if(turn_left <= 150){
                int x = (base_x + dx[i + 1] <= 17630 ? base_x + dx[i + 1] : base_x - dx[i + 1]), y = (base_y + dy[i + 1] <= 9000 ? base_y + dy[i + 1] : base_y - dy[i + 1]);   
                int herox = li[i].second.first, heroy = li[i].second.second;

                int idhero = li[i].first;

                int what_to_do = -1; 
                int target_entity = -1, nearest_entity = -1; ll dist = inf;
                for(int j = 0; j < entity_list.size(); j ++) if(entity_list[j].type == 0){
                    ll get_distance = (entity_list[j].x - herox) * 1ll * (entity_list[j].x - herox) + (entity_list[j].y - heroy) * 1ll * (entity_list[j].y - heroy);
                    ll get_base_distance = (entity_list[j].x - (opp_x)) * 1ll * (entity_list[j].x - (opp_x)) + (entity_list[j].y - (opp_y)) * 1ll * (entity_list[j].y - (opp_y));
                    if(get_distance <= 2100ll * 2100ll && our_mana >= 10){
                        if(entity_list[j].threat_for == 2 && entity_list[j].shield_life == 0 && get_distance <= 1190 * 1190 && get_base_distance <= 5500ll * 5500ll){
                            what_to_do = 1; target_entity = j;
                        }else if(our_mana >= 60 && entity_list[j].threat_for != 2 && what_to_do != 1 && !controlled.count(entity_list[j].id)){
                            what_to_do = 0; 
                            if(dist > get_distance) {target_entity = j; dist = get_distance;}
                        }   
                    }
                    if(dist > get_distance && entity_list[j].threat_for != 2){
                        nearest_entity = j;
                        dist = get_distance;
                    }
                }


                if(what_to_do == 1){
                    cout << "SPELL WIND " << opp_x << " " << opp_y << endl;
                }else if(what_to_do == 0){
                    if(entity_list[target_entity].near_base == 1 && dist <= 1100ll * 1100ll){
                        cout << "SPELL WIND " << opp_x << " " << opp_y << endl;
                    }else{
                        int gt = rand() % 13;
                        controlled.insert(entity_list[target_entity].id);
                        cout << "SPELL CONTROL " << entity_list[target_entity].id << " " << tx[gt] << " " << ty[gt] << endl;
                    }
                }else if(dist <= 2000ll * 2000ll){
                    if((herox - x) * 1ll * (herox - x) + (heroy - y) * 1ll * (heroy - y) >= 2000ll * 2000ll)
                        cout << "MOVE " << x << " " << y << endl;
                    else{
                        ll get_opponent_distance = (entity_list[nearest_entity].x - (opp_x)) * 1ll * (entity_list[nearest_entity].x - (opp_x)) + (entity_list[nearest_entity].y - (opp_y)) * (entity_list[nearest_entity].y - (opp_y));
                        if(get_opponent_distance <= 7500ll * 7500ll){
                            cout << "MOVE " << entity_list[nearest_entity].x << " " << entity_list[nearest_entity].y << endl;
                        }else{
                            cout << "MOVE " << x << " " << y << endl;
                        }
                    }
                }else{
                    target_entity = -1;
                    for(int j = 0; j < entity_list.size(); j ++) if(entity_list[j].type == 2){
                        ll get_distance = (entity_list[j].x - herox) * 1ll * (entity_list[j].x - herox) + (entity_list[j].y - heroy) * 1ll * (entity_list[j].y - heroy);
                        if(get_distance <= 2150 * 2150 && !controlled.count(entity_list[j].id)){
                            target_entity = j;
                        }
                    }

                    if(target_entity == -1){
                        cout << "MOVE " << x << " " << y << endl;
                    }else if(our_mana >= 60){
                        controlled.insert(entity_list[target_entity].id);
                        cout << "SPELL CONTROL " << entity_list[target_entity].id << " " << base_x << " " << base_y << endl;
                    }else{
                        cout << "MOVE " << x << " " << y << endl;
                    }
                }continue;
            }
            int x = (base_x + (dx[i] - 1000) <= 17630 ? base_x + (dx[i] - 1000) : base_x - (dx[i] - 1000)), y = (base_y + (dy[i] + 1000) <= 9000 ? base_y + (dy[i] + 1000) : base_y - (dy[i] + 1000));
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
            if(turn_left <= 150){
                int x = (base_x + dx[i] <= 17630 ? base_x + dx[i] : base_x - dx[i]), y = (base_y + dy[i] <= 9000 ? base_y + dy[i] : base_y - dy[i]);   
                int herox = li[i].second.first, heroy = li[i].second.second;

                int idhero = li[i].first;

                int what_to_do = -1; 
                int target_entity = -1, nearest_entity = -1; ll dist = inf;
                for(int j = 0; j < entity_list.size(); j ++) if(entity_list[j].type == 0){
                    ll get_distance = (entity_list[j].x - herox) * 1ll * (entity_list[j].x - herox) + (entity_list[j].y - heroy) * 1ll * (entity_list[j].y - heroy);
                    ll get_base_distance = (entity_list[j].x - (opp_x)) * 1ll * (entity_list[j].x - (opp_x)) + (entity_list[j].y - (opp_y)) * 1ll * (entity_list[j].y - (opp_y));
                    if(get_distance <= 2100ll * 2100ll && our_mana >= 10){
                        if(entity_list[j].threat_for == 2 && entity_list[j].shield_life == 0 && get_distance <= 1190 * 1190 && get_base_distance <= 5500ll * 5500ll){
                            what_to_do = 1; target_entity = j;
                        }else if(our_mana >= 60 && entity_list[j].threat_for != 2 && what_to_do != 1 && !controlled.count(entity_list[j].id)){
                            what_to_do = 0; 
                            if(dist > get_distance) {target_entity = j; dist = get_distance;}
                        }   
                    }
                    if(dist > get_distance && entity_list[j].threat_for != 2){
                        nearest_entity = j;
                        dist = get_distance;
                    }
                }


                if(what_to_do == 1){
                    cout << "SPELL WIND " << opp_x << " " << opp_y << endl;
                }else if(what_to_do == 0){
                    if(entity_list[target_entity].near_base == 1 && dist <= 1100ll * 1100ll){
                        cout << "SPELL WIND " << opp_x << " " << opp_y << endl;
                    }else{
                        int gt = rand() % 13;
                        controlled.insert(entity_list[target_entity].id);
                        cout << "SPELL CONTROL " << entity_list[target_entity].id << " " << tx[gt] << " " << ty[gt] << endl;
                    }
                }else if(dist <= 2000ll * 2000ll){
                    if((herox - x) * 1ll * (herox - x) + (heroy - y) * 1ll * (heroy - y) >= 2000ll * 2000ll)
                        cout << "MOVE " << x << " " << y << endl;
                    else{
                        ll get_opponent_distance = (entity_list[nearest_entity].x - (opp_x)) * 1ll * (entity_list[nearest_entity].x - (opp_x)) + (entity_list[nearest_entity].y - (opp_y)) * (entity_list[nearest_entity].y - (opp_y));
                        if(get_opponent_distance <= 7500ll * 7500ll){
                            cout << "MOVE " << entity_list[nearest_entity].x << " " << entity_list[nearest_entity].y << endl;
                        }else{
                            cout << "MOVE " << x << " " << y << endl;
                        }
                    }
                }else{
                    target_entity = -1;
                    for(int j = 0; j < entity_list.size(); j ++) if(entity_list[j].type == 2){
                        ll get_distance = (entity_list[j].x - herox) * 1ll * (entity_list[j].x - herox) + (entity_list[j].y - heroy) * 1ll * (entity_list[j].y - heroy);
                        if(get_distance <= 2150 * 2150 && !controlled.count(entity_list[j].id)){
                            target_entity = j;
                        }
                    }

                    if(target_entity == -1){
                        cout << "MOVE " << x << " " << y << endl;
                    }else if(our_mana >= 60){
                        controlled.insert(entity_list[target_entity].id);
                        cout << "SPELL CONTROL " << entity_list[target_entity].id << " " << base_x << " " << base_y << endl;
                    }else{
                        cout << "MOVE " << x << " " << y << endl;
                    }
                }continue;
            }

            int x = (base_x + dx[i + 1] <= 17630 ? base_x + dx[i + 1] : base_x - dx[i + 1]), y = (base_y + dy[i + 1] <= 9000 ? base_y + dy[i + 1] : base_y - dy[i + 1]), threatfor = 0, nearbase = 0;
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
        turn_left --;
    }
}