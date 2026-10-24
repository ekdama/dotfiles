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
    coord_pair_t* p_q;
    int p_q_s;
    //int* p_q_pos;
    short num_find;
} typedef djikstra_cache_t;
struct Route {
    int x_end;
    int y_end;
    short cost;
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
station_t** stations;

int parent(int i) {
    return (i - 1) / 2;
}

int dist_cache(int x, int y) {
    if (__builtin_expect(stations[x][y].edit_times == grid.cache.num_find, 1)) {
        return stations[x][y].dist;
    }
    return INT_INFINITY;
    //return (stations[x][y].edit_times == grid.cache.num_find) ? stations[x][y].dist : INT_INFINITY;
}

int chebyshev_hex_dist(int x1, int y1, int x2, int y2) {
    int x1_c = y1;
    int z1_c = x1 - (((y1 - ((y1) & 1))) >> 1);
    int y1_c = - x1_c - z1_c;

    int x2_c = y2;
    int z2_c = x2 - ((y2 - ((y2) & 1)) >> 1);
    int y2_c = - x2_c - z2_c;

    // Compute differences in axial coordinates
    int x_dist = abs(x1_c - x2_c);
    int y_dist = abs(y1_c - y2_c);
    int z_dist = abs(z1_c - z2_c);

    //printf("%d %d %d dbg axial\n",x_dist, y_dist, z_dist);
    // Hex distance formula: (|dq| + |dr| + |dq + dr|) / 2
    return (x_dist + y_dist + z_dist) >> 1;
}

void min_heapify_p_q(int i) {
    int posmax = i;
    int l, r, dist, dist_pm, dist_l, dist_r;
    while (1) {
        l = LEFT(i);
        r = RIGHT(i);
        dist = dist_cache(grid.cache.p_q[i].x, grid.cache.p_q[i].y);
        dist_l = (l < grid.cache.p_q_s) ? dist_cache(grid.cache.p_q[l].x, grid.cache.p_q[l].y) : INT_INFINITY;
        dist_r = (r < grid.cache.p_q_s) ? dist_cache(grid.cache.p_q[r].x, grid.cache.p_q[r].y) : INT_INFINITY;
                
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
        
        if (posmax == i) {
            break;
        }
        
        coord_pair_t k = grid.cache.p_q[i];
        //grid.cache.p_q_pos[grid.cache.p_q[i].x * grid.m + grid.cache.p_q[i].y] = posmax;
        //grid.cache.p_q_pos[grid.cache.p_q[posmax].x * grid.m + grid.cache.p_q[posmax].y] = i;

        grid.cache.p_q[i] = grid.cache.p_q[posmax];
        grid.cache.p_q[posmax] = k;
        
        i = posmax; 
    }
}

unsigned char natural_sp(int x1, int y1, int x2, int y2) {
    return 0;
}

coord_pair_t remove_max_p_q(int x_d, int y_d) {
    coord_pair_t max;
    max.x = -1;
    max.y = -1;
    if (grid.cache.p_q_s < 1) return max; //return (-1,-1) if the queue is empty
    max = grid.cache.p_q[0];
    grid.cache.p_q[0] = grid.cache.p_q[grid.cache.p_q_s - 1];
    //min heapify
    grid.cache.p_q_s--;
    min_heapify_p_q(0);
    return max;
}

/*void decrease_priority(int i, int alt) {
    int j = grid.cache.p_q_pos[i];
    int dist = dist_cache(grid.cache.p_q[parent(j)].x, grid.cache.p_q[parent(j)].y);
    coord_pair_t k;

    while (j > 0 && (dist) > (alt)) {
        k = grid.cache.p_q[parent(j)];
        grid.cache.p_q_pos[grid.cache.p_q[parent(j)].x  * grid.m + grid.cache.p_q[parent(j)].y] = j;
        grid.cache.p_q_pos[grid.cache.p_q[j].x * grid.m + grid.cache.p_q[j].y] = parent(j);

        grid.cache.p_q[parent(j)] = grid.cache.p_q[j];
        grid.cache.p_q[j] = k;
        //if (i == (85051)) printf("%d dbg priority pos \n",grid.cache.p_q_pos[j]);
        j = parent(j);
        //printf("%d %d dbg dp\n", dist, j);
        dist = dist_cache(grid.cache.p_q[parent(j)].x, grid.cache.p_q[parent(j)].y);
    }
}*/

void add_element_p_q(coord_pair_t i) {
    //int i_l = LEFT(i);
    //int i_r = RIGHT(i);
    grid.cache.p_q_s++;
    int j = grid.cache.p_q_s - 1;
    grid.cache.p_q[grid.cache.p_q_s - 1] = i;
    //grid.cache.p_q_pos[i.x * grid.m + i.y] = grid.cache.p_q_s - 1;
    int dist_p = dist_cache(grid.cache.p_q[parent(j)].x,grid.cache.p_q[parent(j)].y);
    int dist = dist_cache(grid.cache.p_q[(j)].x,grid.cache.p_q[(j)].y);

    coord_pair_t k;
    //printf("%d %d dec\n", j, dist_p);
    while (j > 0 && dist_p > dist) {
        k = grid.cache.p_q[parent(j)];

        grid.cache.p_q[parent(j)] = grid.cache.p_q[j];
        grid.cache.p_q[j] = k;

        j = parent(j);

        dist_p = dist_cache(grid.cache.p_q[parent(j)].x,grid.cache.p_q[parent(j)].y);
        dist = dist_cache(grid.cache.p_q[(j)].x,grid.cache.p_q[(j)].y);
    }
}

unsigned char set_route(int pos, int x1, int y1, int x2, int y2, int src_cost,unsigned char toggle, unsigned char stop_on_max) {
    int tot_cost = 0;    
    for(int i=0; i < stations[x1][y1].num_routes; i++) {
        if (toggle && stations[x1][y1].routes[i].x_end == x2 && stations[x1][y1].routes[i].y_end == y2) {
                //tot_cost += stations[x1][y1].routes[i].cost;
                stations[x1][y1].routes[i].enabled = !stations[x1][y1].routes[i].enabled;
                return 2;
            }
        //if (!toggle) {
            tot_cost += stations[x1][y1].routes[i].cost;
        //}
    }
    if (stop_on_max && stations[x1][y1].num_routes >= 5) {
        return 0;
    }
    //if (pos == stations[x1][y1].num_routes) stations[x1][y1].routes = (route_t*)realloc(stations[x1][y1].routes, sizeof(route_t) * stations[x1][y1].num_routes + 1);
    stations[x1][y1].routes[pos].cost = floor(((float)src_cost + (float)tot_cost) / (float)(stations[x1][y1].num_routes + 1));
    
    //printf("%d %d %d %d dbg tot cost\n", tot_cost, src_cost, stations[x1][y1].num_routes + 1, stations[x1][y1].routes[pos].cost);
    
    stations[x1][y1].routes[pos].enabled = 1;
    stations[x1][y1].routes[pos].x_end = x2;
    stations[x1][y1].routes[pos].y_end = y2;

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
            if (stations[i][j].cost != 1) {
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
                //printf("%d %d %d\n", stations[i][j].cost+(int)((float)v * (((float)radius - (float)d) / (float)radius)),i,j);
                stations[i][j].cost = stations[i][j].cost + floor(v * (((float)radius - (float)d) / (float)radius));
                if (stations[i][j].cost < 0) {
                    stations[i][j].cost = 0;
                }
                for(int p = 0; p < stations[i][j].num_routes; p++) {
                    stations[i][j].routes[p].cost = stations[i][j].routes[p].cost + floor(v * (((float)radius - (float)d) / (float)radius));
                        //set_route(p, i, j,
                          //   stations[i][j].routes[p].x_end,
                            //  stations[i][j].routes[p].y_end,
                             //  stations[i][j].cost, 0, 0);
                    if (stations[i][j].routes[p].cost < 0) {
                        stations[i][j].routes[p].cost = 0;
                    }

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
    int d = dist_cache(x2, y2);
    return d == INT_INFINITY ? -1 : d;
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
    
    stations[x1][y1].dist = 0;
    stations[x1][y1].edit_times = grid.cache.num_find + 1;
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

    while(x_min != -1 && x_min != -1) {    
        //printf("%d %d %d \n", x_min, y_min, stations[x_min][y_min].cost);
            //printf("dbg dist %d %d %d\n", stations[x_min][y_min].dist, x_min, y_min);
            for (int l = 0; l < stations[x_min][y_min].num_routes; l++) {
                    alt = dist_cache(x_min, y_min) 
                        + stations[x_min][y_min].routes[l].cost;
                        //printf("%d %d %d dbg air route\n",stations[x_min][y_min].routes[l].cost, dist_cache(x_min, y_min), chebyshev_hex_dist(x1, y1, x2, y2));
                        if (__builtin_expect(stations[stations[x_min][y_min].routes[l].x_end][stations[x_min][y_min].routes[l].y_end].cost != 0
                         && stations[x_min][y_min].routes[l].enabled && alt != INT_INFINITY
                         && alt < dist_cache(stations[x_min][y_min].routes[l].x_end, stations[x_min][y_min].routes[l].y_end), 1)) {
                        
                        stations[stations[x_min][y_min].routes[l].x_end][stations[x_min][y_min].routes[l].y_end].dist = alt;
                        stations[stations[x_min][y_min].routes[l].x_end][stations[x_min][y_min].routes[l].y_end].edit_times = grid.cache.num_find;
                        
                        to_add.x = stations[x_min][y_min].routes[l].x_end;
                        to_add.y = stations[x_min][y_min].routes[l].y_end;
                        add_element_p_q(to_add);
                        has_air = 1;
                        //decrease_priority(stations[x_min][y_min].routes[l].x_end * grid.m + stations[x_min][y_min].routes[l].y_end, alt);
                    }
            }
            int k_start = (x_min > 0) ? x_min - 1 : 0;
            int k_end = (x_min < grid.n - 1) ? x_min + 1 : grid.n - 1;
            int p_start = (y_min > 0) ? y_min - 1 : 0; 
            int p_end = (y_min < grid.m - 1) ? y_min + 1 : grid.m - 1;
            for (k = k_start; k <= k_end; k++) {
                //printf("dbg adj %d %d %d\n",x_min,y_min,adjacency(x_min, y_min, k, p, grid.m) );
                for(p = p_start; p <= p_end; p++) {//djikstra edges check distance
                    dist = dist_cache(k, p);
                    alt = dist_cache(x_min, y_min);
                    //if(stations[x_min][.disty_min] != INT_INFINITY) {
                    adj = adjacency(x_min, y_min, k, p, grid.m);
                    /*if (adj && alt != INT_INFINITY) {
                        alt += stations[x_min][y_min].cost;
                            to_add.x = k;
                            to_add.y = p;
                    }*/
                    //if(x_min == 324 && y_min == 163) printf("%d %d %d %d dbg alt dest\n", adjacency(x_min, y_min, k, p, grid.m), alt, stations[x_min][.disty_min], x_min * grid.m + y_min);

                    //printf("%d %d %d %d %d\n", stations[k][p], .distk, p, adjacency(x_min, y_min, k, p, grid.m), alt);
                    //if(k == x2 && p == y2) printf("dbg dist %d %d %d %d %d %d\n", k, p, dist, alt, stations[x2][y.edit_times2], grid.cache.num_find);
                    //printf("dbg costs %d\n", stations[k][p].cost);
                    //if(stations[x_min][y_min].cost == 0) printf("%d %d\n", alt, dist_cache(x_min, y_min));

                    if (__builtin_expect(adj && stations[x_min][y_min].cost > 0 && alt != INT_INFINITY && (alt + stations[x_min][y_min].cost) < dist, adj)) {
                        to_add.x = k;
                        to_add.y = p;
                        stations[k][p].dist = alt + stations[x_min][y_min].cost;
                        stations[k][p].edit_times = grid.cache.num_find;
                        //printf("dbg costs %d\n", stations[k][p].cost);
                        add_element_p_q(to_add);
                        //decrease_priority(k * grid.m + p, alt);
                    }
                }
            }
            if (__builtin_expect_with_probability(x_min == x2 && y_min == y2,1,0.005)) goto END;
            coord = remove_max_p_q(x2, y2);
            x_min = coord.x;
            y_min = coord.y;
            //printf("%d %d %d %d dbg rad \n", radius, x_min, y_min, stations[x_min][y_min].cost);
    }
    END:
    grid.cache.p_q_s = 0;
    return has_air;
}

unsigned char add_route(int x1, int y1,int x2,int y2) {
    if(!validate_coord(x1, y1, x2, y2)) {
        return 0;
    }
    if (stations[x1][y1].num_routes == 0) {
        stations[x1][y1].routes = (route_t*)malloc(sizeof(route_t));
    } else {
        stations[x1][y1].routes = (route_t*)realloc(stations[x1][y1].routes, (stations[x1][y1].num_routes+1)*sizeof(route_t));
    }

    int result = set_route(stations[x1][y1].num_routes, x1, y1, x2, y2, stations[x1][y1].cost, 1, 1);

    if (result == 1) stations[x1][y1].num_routes++;
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


    if(grid.is_init) {
        size_t new_flat_size = grid.n_internal * grid.m_internal;
        grid.cache.p_q = realloc(grid.cache.p_q, new_flat_size * sizeof(coord_pair_t));
        //grid.cache.p_q_pos = realloc(grid.cache.p_q_pos, new_flat_size * sizeof(int));
        
        if(n_row > grid.n) {
            stations = realloc(stations, grid.n_internal * sizeof(station_t*));
        }

        for(int i = 0; i < n_row; i++) {
            if(i >= grid.n) {
                stations[i] = malloc(grid.m_internal * sizeof(station_t));
            }
            
            if(n_col > grid.m) {
                stations[i] = realloc(stations[i], grid.m_internal * sizeof(station_t));
            }

            for(int j = 0; j < n_col; j++) {
                stations[i][j].cost = 1;
                stations[i][j].dist = INT_INFINITY;
                stations[i][j].edit_times = 0;
                stations[i][j].num_routes = 0;
                stations[i][j].cache.num = 0;
                //grid.cache.p_q_pos[i * n_col + j] = i * n_col + j;
            }
        }
    } else {
        size_t row_ptr_size = grid.n_internal * sizeof(station_t*);
        size_t col_size = grid.m_internal * sizeof(station_t);
        size_t flat_size = grid.n_internal * grid.m_internal;

        
        grid.cache.p_q = malloc(flat_size * sizeof(coord_pair_t));
        //grid.cache.p_q_pos = malloc(flat_size * sizeof(int));
        stations = malloc(row_ptr_size);
        
        for(int i = 0; i < n_row; i++) {
            stations[i] = malloc(col_size);

            for(int j = 0; j < n_col; j++) {
                stations[i][j].cost = 1;
                stations[i][j].dist = INT_INFINITY;
                stations[i][j].edit_times = 0;
                stations[i][j].num_routes = 0;
                stations[i][j].cache.num = 0;
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
    //printf("%d dbg routes\n", stations[xp][yp].cache.num);
    for(i = 0; i < stations[xp][yp].cache.num; i++) {
        if (
            (stations[xp][yp].cache.routes[i].x_end == xd && stations[xp][yp].cache.routes[i].y_end == yd)) {
            found = 1;
            //hit = stations[xp][yp].cache.routes[i].valid == grid.resync;
            hit = stations[xp][yp].cache.routes[i].valid == grid.resync && grid.resync > 0;//!invalidate_cost_change(xp, yp, stations[xp][yp].cache.routes[i]);
            //printf("%d %d %d duh\n", grid.resync, stations[xp][yp].cache.routes[i].valid, hit);
            if(hit) {
                sprintf(result,"%d\n",stations[xp][yp].cache.routes[i].cost);
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
            if(stations[xp][yp].cache.num == 0) {
                stations[xp][yp].cache.routes = (sp_t*)malloc(sizeof(sp_t));
            } else {
                stations[xp][yp].cache.routes = (sp_t*)realloc(stations[xp][yp].cache.routes,(stations[xp][yp].cache.num + 1)*sizeof(sp_t));
            }
            stations[xp][yp].cache.num++;
        }
        stations[xp][yp].cache.routes[i].cost = extract_travel_cost(xp, yp, xd, yd);
        stations[xp][yp].cache.routes[i].valid = grid.resync;
        stations[xp][yp].cache.routes[i].x_end = xd;
        stations[xp][yp].cache.routes[i].y_end = yd;
        stations[xp][yp].cache.routes[i].air = air;

        /*int j = 0;
        for(j = 0; i < stations[xd][yd].cache.num; i++) {
            if (stations[xd][yd].cache.routes[i].x_end == xp && stations[xd][yd].cache.routes[i].y_end == yp){
                break;
            }
        }
        if(j == stations[xd][yd].cache.num) { // if found in dest
            if(stations[xd][yd].cache.num == 0) {
                stations[xd][yd].cache.routes = (sp_t*)malloc(sizeof(sp_t));
            } else {
                stations[xd][yd].cache.routes = (sp_t*)realloc(stations[xd][yd].cache.routes,(stations[xd][yd].cache.num + 1)*sizeof(sp_t));
            }
            stations[xd][yd].cache.num++;

        }
        stations[xd][yd].cache.routes[j].cost = extract_travel_cost(xp, yp, xd, yd);
        stations[xd][yd].cache.routes[j].valid = grid.resync;
        stations[xd][yd].cache.routes[j].x_end = xp;
        stations[xd][yd].cache.routes[j].y_end = yp;
        stations[xd][yd].cache.routes[j].air = air;*/

        grid.last_x = xp;
        grid.last_y = yp;
    }
    sprintf(result,"%d\n",extract_travel_cost(xp, yp, xd, yd));
    fputs(result, stdout); 
} 


void cleanup() {
        for(int i = 0; i < grid.n_internal; i++) {
            free(stations[i]);
        }
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