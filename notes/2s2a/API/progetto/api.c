#include <bits/floatn-common.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define INT_INFINITY 0x3F3F3F3F	

#define LEFT(n) 2*n + 1
#define RIGHT(n) 2*n + 2
#define SIZE 1

struct ShortestPath {
    short valid;
    int x_end;
    int y_end;
    int cost;
    unsigned char air;
} typedef sp_t;

struct CacheSP {
    sp_t* routes;
    unsigned char num;
} typedef cache_sp_t ;
struct CoordPair {
    int x;
    int y;
} typedef coord_pair_t;

struct DjikstraCache {
    //int** h_dist;
    coord_pair_t** b_q;
    int** b_q_s;
    //int* p_q_pos;
    short num_find;
} typedef djikstra_cache_t;
struct Route {
    int x_end;
    int y_end;
    unsigned char enabled;
} typedef route_t;

struct Station {
    route_t* routes;
    cache_sp_t cache;
    short  num_routes;
    short cost;
    short edit_times;
    int dist;
} typedef station_t ;

struct Grid {
    int is_init;
    djikstra_cache_t cache;
    //route_t* routes;
    //int num_routes;
    short resync;
    short cost_modified;
    short num_stations;
    int last_x;
    int last_y;
    int n;
    int m;
    int n_internal;
    int m_internal;
    //short count;
    //mod_region_t* mods;
} typedef grid_t;

grid_t grid;
station_t* stations;

__attribute__((always_inline)) static inline  int index_s(int x, int y, int m) {
    return x * grid.m + y;
} 

static inline int parent(int i) {
    return (i - 1) / 2;
}


int dist_cache(int i) {
    if (__builtin_expect(stations[i].edit_times != grid.cache.num_find, 0)) {
        return INT_INFINITY;
    }
    return stations[i].dist;;
    //return (stations[x][y].edit_times == grid.cache.num_find) ? stations[x][y].dist : INT_INFINITY;
}

int chebyshev_hex_dist(int x1, int y1, int x2, int y2) {
    int x1_c = y1;
    int z1_c = x1 - (y1 >> 1) - ((y1 & 1) >> 1);
    int y1_c = - x1_c - z1_c;

    int x2_c = y2;
    int z2_c = x2 - (y2 >> 1) - ((y2 & 1) >> 1);
    int y2_c = - x2_c - z2_c;

    // Compute differences in axial coordinates
    int x_dist = __builtin_abs(x1_c - x2_c);
    int y_dist = __builtin_abs(y1_c - y2_c);
    int z_dist = __builtin_abs(z1_c - z2_c);

    //printf("%d %d %d dbg axial\n",x_dist, y_dist, z_dist);
    // Hex distance formula: (|dq| + |dr| + |dq + dr|) / 2
    return (x_dist + y_dist + z_dist) >> 1;
}

coord_pair_t peek() {

}

void add_element_b_q(coord_pair_t c) {

}

/*void min_heapify_p_q(int i) {
    int posmax = i;
    int l, r, dist, dist_pm, dist_l, dist_r;
    while (1) {
        l = LEFT(i);
        r = RIGHT(i);
        int idx = index_s(grid.cache.p_q[i].x, grid.cache.p_q[i].y, grid.m);
        int idx_l = index_s(grid.cache.p_q[l].x, grid.cache.p_q[l].y, grid.m);
        int idx_r = index_s(grid.cache.p_q[r].x, grid.cache.p_q[r].y, grid.m);
        dist = dist_cache(idx);
        dist_l = (l < grid.cache.p_q_s) ? dist_cache(idx_l) : INT_INFINITY;
        dist_r = (r < grid.cache.p_q_s) ? dist_cache(idx_r) : INT_INFINITY;
                
        posmax = i;
        dist_pm = dist;
        
        
        if (l < grid.cache.p_q_s && dist_l < dist_pm) {
            posmax = l;
            dist_pm = dist_l;
        }
        if (r < grid.cache.p_q_s && dist_r < dist_pm) {
            posmax = r;
            dist_pm = dist_r;
        }
        
        if (posmax != i) {
                    coord_pair_t k = grid.cache.p_q[i];
        //grid.cache.p_q_pos[grid.cache.p_q[i].x * grid.m + grid.cache.p_q[i].y] = posmax;
        //grid.cache.p_q_pos[grid.cache.p_q[posmax].x * grid.m + grid.cache.p_q[posmax].y] = i;

            grid.cache.p_q[i] = grid.cache.p_q[posmax];
            grid.cache.p_q[posmax] = k;
            
            i = posmax; 

        } else {
            break;
        }
        
    }
}

unsigned char natural_sp(int x1, int y1, int x2, int y2) {
    return 0;
}

coord_pair_t remove_max_p_q(int x_d, int y_d) {
    coord_pair_t max;
    max.x = -1;
    max.y = -1;
    if (__builtin_expect(grid.cache.p_q_s < 1,0)) return max; //return (-1,-1) if the queue is empty
    max = grid.cache.p_q[0];
    grid.cache.p_q[0] = grid.cache.p_q[grid.cache.p_q_s - 1];
    //min heapify
    grid.cache.p_q_s--;
    min_heapify_p_q(0); 
    return max;
}



void add_element_p_q(coord_pair_t i) {
    //int i_l = LEFT(i);
    //int i_r = RIGHT(i);
    grid.cache.p_q_s++;
    int j = grid.cache.p_q_s - 1;
    grid.cache.p_q[grid.cache.p_q_s - 1] = i;
    //grid.cache.p_q_pos[i.x * grid.m + i.y] = grid.cache.p_q_s - 1;
    int idx_p = index_s(grid.cache.p_q[parent(j)].x,grid.cache.p_q[parent(j)].y,grid.m);
    int idx = index_s(grid.cache.p_q[j].x,grid.cache.p_q[j].y,grid.m);

    int dist_p = dist_cache(idx_p);
    int dist = dist_cache(idx);

    coord_pair_t k;
    //printf("%d %d dec\n", j, dist_p);
    int p;
    while (j > 0 && dist_p > dist) {
        p = parent(j);
        k = grid.cache.p_q[p];

        grid.cache.p_q[p] = grid.cache.p_q[j];
        grid.cache.p_q[j] = k;

        j = p;
        p = parent(j);
        idx_p = index_s(grid.cache.p_q[p].x,grid.cache.p_q[p].y,grid.m);
        idx = index_s(grid.cache.p_q[j].x,grid.cache.p_q[j].y,grid.m);

        dist_p = dist_cache(idx_p);
        dist = dist_cache(idx);
    }
}*/

unsigned char set_route(int pos, int x1, int y1, int x2, int y2, int src_cost,unsigned char toggle, unsigned char stop_on_max) {
    //int tot_cost = 0;    
    int idx = index_s(x1,y1,grid.m);
    for(int i=0; i < stations[idx].num_routes; i++) {
        if (toggle && stations[idx].routes[i].x_end == x2 && stations[idx].routes[i].y_end == y2) {
                //tot_cost += stations[index_s(x1,y1)].routes[i].cost;
                stations[index_s(x1,y1,grid.m)].routes[i].enabled = !stations[idx].routes[i].enabled;
                return 2;
            }
    }
    if (stop_on_max && stations[idx].num_routes >= 5) {
        return 0;
    }
    
    //printf("%d %d %d %d dbg tot cost\n", tot_cost, src_cost, stations[index_s(x1,y1)].num_routes + 1, stations[index_s(x1,y1)].routes[pos].cost);
    
    stations[index_s(x1,y1,grid.m)].routes[pos].enabled = 1;
    stations[index_s(x1,y1,grid.m)].routes[pos].x_end = x2;
    stations[index_s(x1,y1,grid.m)].routes[pos].y_end = y2;

    return 1;
}


unsigned char validate_coord_single(int xp, int yp) {
    if(xp < 0 || yp < 0 || xp >= grid.n || yp >= grid.m) {
        return 0;
    }
    return 1;

}

unsigned char validate_coord(int xp, int yp, int xd, int yd) {
    if(xp < 0 || yp < 0 || xp >= grid.n || yp >= grid.m) {
        return 0;
    }
    if(xd < 0 || yd < 0 || xd >= grid.n || yd >= grid.m) {
        return 0;
    }
    return 1;

}

unsigned char is_region_modified(int x1, int y1, int x2, int y2) {
    int x_s = x1 < x2 ? x1 : x2;
    int y_s = y1 < y2 ? y1 : y2;

    int x_d = x1 > x2 ? x1 : x2;
    int y_d = y1 > y2 ? y1 : y2;

    int i,j;
    for(i = x_s; i <= x_d; i++) {
        for(j = y_s; j <= y_d; j++) {
            if (stations[index_s(i,j,grid.m)].cost != 1) {
                return 1;
            }
        }
    }
    return 0;
}

unsigned char adjacency(int x1, int y1, int x2, int y2, int m) {
    return __builtin_expect(chebyshev_hex_dist(x1, y1, x2, y2) == 1, 0);
    //return 1;
}

unsigned char propagate_cost_update(int x, int y, int v, int radius) {
    int n = grid.n;
    int m = grid.m;

    int x_start = x > radius ? x - radius : 0;
    int y_start = y > radius ? y - radius : 0;

    int x_end = x + radius < n ? x + radius : n;
    int y_end = y + radius < m ? y + radius : m;

    if(!validate_coord_single(x, y)) {
        return 0;
    }

    grid.resync++;

    for (int i = x_start; i < x_end; i++) {
        for (int j = y_start; j < y_end; j++) {
            int d = chebyshev_hex_dist(x,y,i,j);
            if (d <= radius) {
                //do the nasty stuff
                //printf("%d %d %d\n", stations[index_s(i, j)].cost+(int)((float)v * (((float)radius - (float)d) / (float)radius)),i,j);
                stations[index_s(i, j,grid.m)].cost = stations[index_s(i, j,grid.m)].cost + floor(v * (((float)radius - (float)d) / (float)radius));
                if (stations[index_s(i, j,grid.m)].cost < 0) {
                    stations[index_s(i, j,grid.m)].cost = 0;
                }
            }
        }
    }
    return 1;
}

int extract_travel_cost(int x1, int y1, int x2, int y2) {
    /*int x_temp = x2;
    int y_temp = y2;
    int i,j;
    int cost = 0;
    while (x_temp != x1 || y_temp != y1) {
        if (stations[x_temp].dist[y_temp] == INT_INFINITY) return -1;
        cost += grid.costs[x_temp][y_temp];
        i = grid.cache.prev[x_temp][y_temp][0];
        j = grid.cache.prev[x_temp][y_temp][1];
        x_temp = i;
        y_temp = j;
    }*/
    int d = dist_cache(index_s(x2, y2, grid.m));
    return __builtin_expect(d != INT_INFINITY, 1) ? d : -1;
}

void clean_cache(int x1, int x2) {
    grid.cache.p_q_s = 0;
    coord_pair_t coord;
    for(int i = 0; i < grid.n; i++) {
        for(int j = 0; j < grid.m; j++) {
            coord.x = i;
            coord.y = j;
            add_element_p_q(coord);            
            
        }
    }
}

/*unsigned char in_standard_sp(int x1, int y1, int x2, int y2, int radius) {
    int x_s = x1 < x2 ? x1 : x2;
    int y_s = x1 < x2 ? y1 : x2;

    int x_e = x1 > x2 ? x1 : x2;
    int y_e = x1 > x2 ? x1 : x2;
    //handle also the edge cases where a dimension is higher and the other one is lower
    for(int i = x_s; i < x_e; i++) {
        if (i % 2 == 0) {
            if ((i > x2 - radius && i < x2 + radius)) return 0;
        } else {
            if ((i > x2 - radius && i < x2 + radius) || (i + 1 > x2 - radius && i < x2 + radius)) return 0;
        }
    }
    return 0;
}*/

unsigned char dijkstra_sp(int x1, int y1, int x2, int y2) {
    int idx = index_s(x1,y1,grid.m);
    int idx_kp;
    stations[idx].dist = 0;
    stations[idx].edit_times = grid.cache.num_find + 1;
    coord_pair_t to_add;
    grid.cache.num_find++;
    //clean_cache(x1, y1);
    to_add.x = x1;
    to_add.y = y1;
    add_element_p_q(to_add);

    int has_air;
    int x_min = x1, y_min = y1;
    /*int x_start = x1 < x2 ? x1 : x2;
    int y_start  = y1 < y2 ? y1 : y2;

    int x_end = x1 > x2 ? x1 : x2;
    int y_end  = y1 > y2 ? y1 : y2;*/
    //int x_min_tmp = x_min, y_min_tmp = y_min;
    //int min_dist;
    int k,p;
    int alt;
//    int radius = 0;
//    int x_rad = 0, y_rad = 0;
    coord_pair_t coord = remove_max_p_q(x2, y2);
    x_min = coord.x;
    y_min = coord.y;
    int dist;
    unsigned char adj;

    while(x_min != -1 && y_min != -1) {    
        idx = index_s(x_min,y_min,grid.m);
        //printf("%d %d %d \n", x_min, y_min, stations[idx].cost);
            //printf("dbg dist %d %d %d\n", stations[idx].dist, x_min, y_min);
            for (int l = 0; l < stations[idx].num_routes; l++) {
                    alt = dist_cache(idx) 
                        + stations[idx].cost;
                        //printf("%d %d %d dbg air route\n",stations[idx].routes[l].cost, dist_cache(x_min, y_min), chebyshev_hex_dist(x1, y1, x2, y2));
                        if (__builtin_expect(stations[index_s(stations[idx].routes[l].x_end,stations[idx].routes[l].y_end,grid.m)].cost != 0
                         && stations[idx].routes[l].enabled && alt != INT_INFINITY
                         && alt < dist_cache(index_s(stations[idx].routes[l].x_end, stations[idx].routes[l].y_end, grid.m)), 1)) {
                        int idx_r = index_s(stations[idx].routes[l].x_end,stations[idx].routes[l].y_end,grid.m);
                        stations[idx_r].dist = alt;
                        stations[idx_r].edit_times = grid.cache.num_find;
                        
                        to_add.x = stations[idx].routes[l].x_end;
                        to_add.y = stations[idx].routes[l].y_end;
                        add_element_p_q(to_add);
                        has_air = 1;
                        //decrease_priority(stations[idx].routes[l].x_end * grid.m + stations[idx].routes[l].y_end, alt);
                    }
            }
            int k_start = (x_min > 0) ? x_min - 1 : 0;
            int k_end = (x_min < grid.n - 1) ? x_min + 1 : grid.n - 1;
            int p_start = (y_min > 0) ? y_min - 1 : 0; 
            int p_end = (y_min < grid.m - 1) ? y_min + 1 : grid.m - 1;
            for (k = k_start; k <= k_end; k++) {
                //printf("dbg adj %d %d %d\n",x_min,y_min,adjacency(x_min, y_min, k, p, grid.m) );
                for(p = p_start; p <= p_end; p++) {//djikstra edges check distance
                    idx_kp = index_s(k, p,grid.m);
                    dist = dist_cache(idx_kp);
                    alt = dist_cache(idx);
                    //if(stations[x_min][.disty_min] != INT_INFINITY) {
                    adj = adjacency(x_min, y_min, k, p, grid.m);
                    /*if (adj && alt != INT_INFINITY) {
                        alt += stations[idx].cost;
                            to_add.x = k;
                            to_add.y = p;
                    }*/
                    //if(x_min == 324 && y_min == 163) printf("%d %d %d %d dbg alt dest\n", adjacency(x_min, y_min, k, p, grid.m), alt, stations[x_min][.disty_min], x_min * grid.m + y_min);

                    //printf("%d %d %d %d %d\n", stations[index_s(k, p)], .distk, p, adjacency(x_min, y_min, k, p, grid.m), alt);
                    //if(k == x2 && p == y2) printf("dbg dist %d %d %d %d %d %d\n", k, p, dist, alt, stations[x2][y.edit_times2], grid.cache.num_find);
                    //printf("dbg costs %d\n", stations[index_s(k, p)].cost);
                    //if(stations[idx].cost == 0) printf("%d %d\n", alt, dist_cache(x_min, y_min));

                    if (__builtin_expect(adj && stations[idx].cost > 0 && alt != INT_INFINITY && (alt + stations[idx].cost) < dist, adj)) {
                        to_add.x = k;
                        to_add.y = p;
                        stations[idx_kp].dist = alt + stations[idx].cost;
                        stations[idx_kp].edit_times = grid.cache.num_find;
                        //printf("dbg costs %d\n", stations[index_s(k, p)].cost);
                        add_element_p_q(to_add);
                        //decrease_priority(k * grid.m + p, alt);
                    }
                }
            }
            if (__builtin_expect_with_probability(x_min == x2 && y_min == y2,1,0.005)) goto END;
            coord = remove_max_p_q(x2, y2);
            x_min = coord.x;
            y_min = coord.y;
            //printf("%d %d %d %d dbg rad \n", radius, x_min, y_min, stations[idx].cost);
    }
    END:
    grid.cache.p_q_s = 0;
    return has_air;
}

unsigned char add_route(int x1, int y1,int x2,int y2) {
    if(!validate_coord(x1, y1, x2, y2)) {
        return 0;
    }
    if (stations[index_s(x1,y1,grid.m)].num_routes == 0) {
        stations[index_s(x1,y1,grid.m)].routes = (route_t*)malloc(sizeof(route_t));
    } else {
        stations[index_s(x1,y1,grid.m)].routes = (route_t*)realloc(stations[index_s(x1,y1,grid.m)].routes, (stations[index_s(x1,y1,grid.m)].num_routes+1)*sizeof(route_t));
    }

    int result = set_route(stations[index_s(x1,y1,grid.m)].num_routes, x1, y1, x2, y2, stations[index_s(x1,y1,grid.m)].cost, 1, 1);

    if (result == 1) stations[index_s(x1,y1,grid.m)].num_routes++;
    grid.resync++;
    grid.last_x = -1;
    grid.last_y = -1;
    return result > 0;
}

void init(int n_row, int n_col) {
    grid.resync = 0;
    grid.last_x = -1;
    grid.last_y = -1;
    grid.cost_modified = 0;
    grid.num_stations = 0;
    grid.cache.num_find = 0;

    grid.n_internal = ((n_row + SIZE - 1) / SIZE) * SIZE;
    grid.m_internal = ((n_col + SIZE - 1) / SIZE) * SIZE;

    grid.n = ((n_row + SIZE - 1) / SIZE) * SIZE;
    grid.m = ((n_col + SIZE - 1) / SIZE) * SIZE;
    if(grid.is_init) {
        size_t new_flat_size = grid.n_internal * grid.m_internal;
        grid.cache.p_q = realloc(grid.cache.p_q, new_flat_size * sizeof(coord_pair_t));
        //grid.cache.p_q_pos = realloc(grid.cache.p_q_pos, new_flat_size * sizeof(int));
        
        stations = realloc(stations, new_flat_size * sizeof(station_t));

        for(int i = 0; i < n_row; i++) {
            

            for(int j = 0; j < n_col; j++) {
                int idx = index_s(i, j,grid.m);
                stations[idx].cost = 1;
                stations[idx].dist = INT_INFINITY;
                stations[idx].edit_times = 0;
                stations[idx].num_routes = 0;
                stations[idx].cache.num = 0;
                stations[idx].cache.routes = NULL;
                stations[idx].routes = NULL;
                //grid.cache.p_q_pos[i * n_col + j] = i * n_col + j;
            }
        }
    } else {
        size_t flat_size = n_col * n_row;

        
        grid.cache.p_q = malloc(flat_size * sizeof(coord_pair_t));
        //grid.cache.p_q_pos = malloc(flat_size * sizeof(int));
        stations = malloc(flat_size * sizeof(station_t));
        
        for(int i = 0; i < n_row; i++) {
            for(int j = 0; j < n_col; j++) {
                int idx = index_s(i, j,grid.m);
                stations[idx].cost = 1;
                stations[idx].dist = INT_INFINITY;
                stations[idx].edit_times = 0;
                stations[idx].num_routes = 0;
                stations[idx].cache.num = 0;
                stations[idx].cache.routes = NULL;
                stations[idx].routes = NULL;

                //grid.cache.p_q_pos[i * n_col + j] = i * n_col + j;
            }
        }
    }

    grid.n = n_row;
    grid.m = n_col;
    grid.is_init = 1;
    fputs("OK\n", stdout);
}

void change_cost(int x, int y, int v, int radius) {
    if(!grid.is_init) {
        fputs("KO\n",stdout);
        return;
    }
    if (!validate_coord_single(x, y)) {
        fputs("KO\n",stdout);
        return;
    }
    if (radius <= 0) {
        fputs("KO\n",stdout);
        return;
    }
    if (v < -10 || v > 10) {
        fputs("KO\n",stdout);
        return;

    }
    if(!propagate_cost_update(x, y, v, radius)) {
        fputs("KO\n",stdout);
        return;
    }
    fputs("OK\n",stdout);
    grid.cost_modified = 1;
    grid.last_x = -1;
    grid.last_y = -1;
}

void toggle_air_route(int x1, int y1, int x2, int y2) {
    if(!grid.is_init) {
        fputs("KO\n",stdout);
        return;
    }
    grid.num_stations = 1;
    if(add_route(x1, y1, x2, y2)) {
        fputs("OK\n",stdout);
        return;
    } 
    fputs("KO\n",stdout);
}

void travel_cost(int xp, int yp, int xd, int yd) {
    char result[15];
    unsigned char hit = 0, found = 0;
    int i;
    int idx = index_s(xp, yp,grid.m);
    //int idx_d = index_s(xd, yd, grid.m);
    if(!grid.is_init) {
        fputs("-1\n",stdout);
        return;
    }

    if (xp == xd && yp == yd) {
        fputs("0\n", stdout);
        return;
    }
    if (!validate_coord(xp, yp, xd, yd)) {
        fputs("-1\n", stdout);
        return;
    }
    if(!grid.cost_modified && grid.num_stations == 0){
        sprintf(result,"%d\n",chebyshev_hex_dist(xp, yp, xd, yd));
        fputs(result, stdout);
        return;
    }
    //printf("%d dbg routes\n", stations[idx].cache.num);
    for(i = 0; i < stations[idx].cache.num; i++) {
        if (
            (stations[idx].cache.routes[i].x_end == xd && stations[idx].cache.routes[i].y_end == yd)) {
            found = 1;
            //hit = stations[idx].cache.routes[i].valid == grid.resync;
            hit = stations[idx].cache.routes[i].valid == grid.resync && grid.resync > 0 && stations[idx].cache.routes[i].air != 3;//!invalidate_cost_change(xp, yp, stations[idx].cache.routes[i]);
            //printf("%d %d %d duh\n", grid.resync, stations[idx].cache.routes[i].valid, hit);
            if(hit) {
                sprintf(result,"%d\n",stations[idx].cache.routes[i].cost);
                fputs(result, stdout);
                return;
            } else {
                break;
            }
        }
    }
    

    /*if(!is_region_modified(xp, yp, xd, yd)){
        sprintf(result,"%d\n",chebyshev_hex_dist(xp, yp, xd, yd));
        fputs(result, stdout);
        return;
    } */
    if (1){
        //for (int i = 0; i < grid.num_mods; i++) {
            // controllare se una regione di costo modificato appare nella griglia fra
            //i due esagoni. se si, si fa djikstra :(
        //}

        unsigned char air = dijkstra_sp(xp, yp, xd, yd);
        if(!found) {
            if(stations[idx].cache.num == 0) {
                stations[idx].cache.routes = (sp_t*)malloc(sizeof(sp_t));
            } else {
                stations[idx].cache.routes = (sp_t*)realloc(stations[idx].cache.routes,(stations[idx].cache.num + 1)*sizeof(sp_t));
            }
            stations[idx].cache.num++;
        }
        stations[idx].cache.routes[i].cost = extract_travel_cost(xp, yp, xd, yd);
        stations[idx].cache.routes[i].valid = grid.resync;
        stations[idx].cache.routes[i].x_end = xd;
        stations[idx].cache.routes[i].y_end = yd;
        stations[idx].cache.routes[i].air = air;

        /*int j = 0;
        for(j = 0; i < stations[idx_d].cache.num; i++) {
            if (stations[idx_d].cache.routes[i].x_end == xp && stations[idx_d].cache.routes[i].y_end == yp){
                break;
            }
        }
        if(j == stations[idx_d].cache.num) { // if found in dest
            if(stations[idx_d].cache.num == 0) {
                stations[idx_d].cache.routes = (sp_t*)malloc(sizeof(sp_t));
            } else {
                stations[idx_d].cache.routes = (sp_t*)realloc(stations[idx_d].cache.routes,(stations[idx_d].cache.num + 1)*sizeof(sp_t));
            }
            stations[idx_d].cache.num++;

        }
        stations[idx_d].cache.routes[j].cost = extract_travel_cost(xp, yp, xd, yd);
        stations[idx_d].cache.routes[j].valid = grid.resync;
        stations[idx_d].cache.routes[j].x_end = xp;
        stations[idx_d].cache.routes[j].y_end = yp;
        stations[idx_d].cache.routes[j].air = air + 2;*/

        grid.last_x = xp;
        grid.last_y = yp;
    }
    sprintf(result,"%d\n",extract_travel_cost(xp, yp, xd, yd));
    fputs(result, stdout); 
} 


void cleanup() {
        free(stations);
        
}

int main() {
    grid.is_init = 0;
    char command[20]; 
    char input[100]; 
    int args[4];   
    //grid.count = 0;
    while (1) {
        //grid.count++;
            if(fgets(input,100,stdin) == NULL) {
                return 0;
            }

            sscanf(input, "%19s", command);

            if (strcmp(command, "init") == 0) {
                if (sscanf(input, "%*s %d %d", &args[0], &args[1]) == 2) {
                    init(args[0], args[1]);
                }
            }
            else if (strcmp(command, "change_cost") == 0) {
                if (sscanf(input, "%*s %d %d %d %d", &args[0], &args[1], &args[2], &args[3]) == 4) {
                    change_cost(args[0], args[1], args[2], args[3]);
                }
            }
            else if (strcmp(command, "toggle_air_route") == 0) {
                if (sscanf(input, "%*s %d %d %d %d", &args[0], &args[1], &args[2], &args[3]) == 4) {
                    toggle_air_route(args[0], args[1], args[2], args[3]);
                }        
            }
            else if (strcmp(command, "travel_cost") == 0) {
                if (sscanf(input, "%*s %d %d %d %d", &args[0], &args[1], &args[2], &args[3]) == 4) {
                    travel_cost(args[0], args[1], args[2], args[3]);
                }
            }
    }
    cleanup();
}