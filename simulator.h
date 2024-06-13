/**
 * @file simulator.h
 * @author Begüm Karabaş 22120205002
 *         Hilal Nur Albayrak 22120205056
 * @brief 
 * @version 0.1
 * @date 2024-05-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <semaphore.h>
#include "list.h"

/*type for a coordinate*/
typedef struct coord {
    int x;
    int y;
} Coord;

/*type for a survivor*/
typedef struct survivor {
    int status;/*yardım bekliyorsa 0 aldysa 1 ben açıklamasını böyle yaptım */
    Coord coord;
    struct tm discovery_time; /*you can use struct tm, too*/
    struct tm helped_time;
    char info[25];
} Survivor;

/*type for a cell in the map*/
typedef struct mapcell {
    Coord coord;       /*in our program simply i,j*/
    List *survivors; /*the survivors waiting help in this cell*/
} MapCell;

/*2d map type*/
typedef struct map {
    int height; /*height of the map*/
    int width;  /*width of the map*/
    MapCell **cells; /*pointer to 2d MapCell*/
} Map;


/*type for a drone*/
typedef struct {
    Coord coord;
    Coord destination;
    struct tm stime;
    char info[20];
    int numberofhelped;
    int status; /* 0: idle, 1: active*/
    Survivor *assigned_survivor; // Add this field
} Drone;

extern Map map;

/*lists*/
extern List *survivors;/* survivor list, waiting for help*/         
extern List *helpedsurvivors; /* helped survivor list*/
extern List *drones;  /*drone list*/

/*model.c*/
void init_map(int height, int width);
void freemap();
Survivor *create_survivor(Coord *coord, char *info, struct tm *discovery_time);
void* survivor_generator(void *args);
void* drone_controller(void *args); 
void* drone_generator();
void *drone_runner(void *drone);
Node* find_node_with_data(List *list, Survivor *data);
void increase_helped();
void check_and_remove_survivor_at_coord(Coord coord);

/*view.c*/
int init_sdl_window();
int draw_map();
int check_events();
int quit_all();
