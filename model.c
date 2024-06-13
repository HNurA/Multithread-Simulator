/**
 * @file model.c
 * @author Begüm Karabaş 22120205002
 *         Hilal Nur Albayrak 22120205056
 * @brief
 * @version 0.1
 * @date 2024-05-23
 *
 * @copyright Copyright (c) 2024
 *
 */

/* bu dosyada survivorları ve droneları oluşturma 
*  ve droneları survivorları kurtarmak için gönderme işlemlerinin
*  yapıldığı fonksiyonlar bulunur.
*
*/

/*chatgpt den yardım alınmıştır.*/

#include "simulator.h"
#define MAX_SURVIVOR_PER_CELL 3
#define DRONE_SPEED 1

/*SOME EXAMPLE FUNCTIONS GIVEN BELOW*/
Map map;
int numberofcells = 0;
List *survivors;
List *drones;
List *helpedsurvivors;
pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
int number_helped = 0;

/*haritayı ve listeleri oluşturan fonksiyon */
void init_map(int height, int width) {
    map.height = height;
    map.width = width;
    numberofcells = height * width;
    survivors = create_list(sizeof(Survivor), numberofcells * 10);
    drones = create_list(sizeof(Drone), 10);
    helpedsurvivors = create_list(sizeof(Survivor),1000);

    /*pointer array*/
    map.cells = malloc(sizeof(MapCell *) * map.height);

    for (int j = 0; j < map.height; j++) {
        /*rows for each pointer*/
        map.cells[j] = malloc(sizeof(MapCell) * map.width);
    }

    for (int i = 0; i < map.height; i++) {
        for (int j = 0; j < map.width; j++) {
            map.cells[i][j].coord.x = i;
            map.cells[i][j].coord.y = j; 
            map.cells[i][j].survivors = create_list(sizeof(Survivor), 10);
        }
    }
    printf("height: %d, width:%d\n", map.height, map.width);
}

/*haritayı free eden fonksiyon*/
void freemap() {
    for (int i = 0; i < map.height; i++) {
        for (int j = 0; j < map.width; j++) {
            List *list = map.cells[i][j].survivors;
            destroy(list);
        }
        free(map.cells[i]);
    }
    free(map.cells);
}

/*gelen parametrelere göre survivorun bilgilerini güncelleyen fonksiyon*/
Survivor *create_survivor(Coord *coord, char *info, struct tm *discovery_time) {
    Survivor *s = malloc(sizeof(Survivor));
    memset(s, 0, sizeof(Survivor));
    memcpy(&(s->discovery_time), discovery_time, sizeof(struct tm));
    strncpy(s->info, info, sizeof(s->info));
    memcpy(&(s->coord), coord, sizeof(Coord));
    s->status = 0;

    printf("survivor: %s\n", asctime(&s->discovery_time));
    printf("%s\n\n", s->info);
    return s;
}

/*THREAD FUNCTION: generates random survivor
 */
/*rastgele yerlerde survivor oluşturan fonksiyon*/
void* survivor_generator(void *args) {
    
    if (map.cells != NULL) {
        time_t traw;
        struct tm t; /*used for localtime*/

        /*survivor info*/
        char info[5] = {'A' + (random() % 26),
                        'A' + (random() % 26),
                        '0' + (random() % 9),
                        '0' + (random() % 9)};

        srandom(time(NULL));
        Coord coord = {random() % map.height, random() % map.width};

        time(&traw);
        localtime_r(&traw, &t);

        printf("creating survivor...%s\n", asctime(&t));
        Survivor *s = create_survivor(&coord, info, &t);

        /*add to general list*/
        add(survivors, s);

        /*add to the list in the cell*/
        List *list = map.cells[coord.x][coord.y].survivors;
        add(list, s);

        printf("survivor added, celllist-size:%d\n", list->number_of_elements);
    }
    return NULL;
}

/*gelen parametrelere göre drone oluşturan fonksiyon*/
Drone *create_drone(Coord *coord, char *info, struct tm *stime) {

    Drone *d = malloc(sizeof(Drone));
    memset(d, 0, sizeof(Drone));
    memcpy(&(d->stime), stime, sizeof(struct tm));
    strncpy(d->info, info, sizeof(d->info));
    memcpy(&(d->coord), coord, sizeof(Coord));
    d->numberofhelped = 0;
    d->status = 0;

    printf("Drone %s\n", asctime(&d->stime));
    printf("%s\n\n", d->info);
    return d;
}

/*rastgele bir yerde dronları oluşturan fonksiyon*/
void* drone_generator() {

    time_t traw;
    struct tm t; /*used for localtime*/

    /*drone info*/
    char info[5] = {'A' + (random() % 26),
                    'A' + (random() % 26),
                    '0' + (random() % 9),
                    '0' + (random() % 9)};

    srandom(time(NULL));
    Coord coord = {random() % map.height, random() % map.width};

    time(&traw);
    localtime_r(&traw, &t);

    printf("creating drone...%s\n", asctime(&t));
    Drone *d = create_drone(&coord, info, &t);

    printf("%d", d->coord.x);

    /*drones listesine droneu ekler*/
    add(drones, d);

    return NULL;
}

/*gelen survivorun listedeki node adresini bulan fonksiyon 
* removenode fonksiyonuna survivor göndermek için help survivor fonksiyonunda çağırılır.
*/
Node* find_node_with_data(List *list, Survivor *data) {
    Node *current = list->head;
    while (current != NULL) {
        Survivor *current_data = (Survivor *)current->data;
        if (current_data == data) {
            return current;
        }
        current = current->next;
    }
    return NULL; 
}

/** a drone delivers aid pack to survivor,
the survivor is marked as helped and removed*/

/*drone survivor a ulaştığında survivorları listeden siler*/
void help_survivor(Drone *d, Survivor *s) {

    /*TODO: remove survivor from survivorlist, mapcell.survivors*/
    /*TODO: edit help_time, add to the helped_survivors*/
    /*TODO: numberofhelped++,
    drone->status is idle, destination: empty*/
    time_t traw;
    struct tm t; /*used for localtime*/
    time(&traw);
    localtime_r(&traw, &t);

    /*fonksiyondan gelen node u removenode fonksiyonuna gönderir*/
    Node *node_to_remove = find_node_with_data(survivors, s);

    if (node_to_remove != NULL) {
        removenode(survivors, node_to_remove);
        removenode(map.cells[s->coord.x][s->coord.y].survivors, node_to_remove);
    }

    s->helped_time = t;
    s->status = 1;

    add(helpedsurvivors, s);

    if (d == NULL) {
        increase_helped();
    }else{
        d->numberofhelped++;
        d->status = 0;
    }
}

/** moves(flies) drone on the map:
based on its speed it jumps cells toward its destination*/

/*droneların konumunu ayarlar*/
void move_drone(Drone *drone) {

    if (drone->coord.x < drone->destination.x) {
        drone->coord.x += DRONE_SPEED;
    } else if (drone->coord.x > drone->destination.x) {
        drone->coord.x -= DRONE_SPEED;
    }

    if (drone->coord.y < drone->destination.y) {
        drone->coord.y += DRONE_SPEED;
    } else if (drone->coord.y > drone->destination.y) {
        drone->coord.y -= DRONE_SPEED;
    }

    /* Move işlemi sonrası bulunduğu konumda survivor kontrolü yap ve varsa kaldır*/
    check_and_remove_survivor_at_coord(drone->coord);
}

/*listede status=1 olan yani yardım almamış survivorları bulur*/
Survivor* find_survivor(List *list) {

    pthread_mutex_lock(&mutex1);/*mutex ekleme*/
    Survivor *survivor = (Survivor *)survivors->head->data;
    Node *current;

    while (list->is_writing) {
        pthread_cond_wait(&(list->cond), &(list->mutex)); /*Yazma işlemi varsa bekle*/
    }

    current = list->head;
    while (current != NULL) {
        Survivor *current_survivor = (Survivor *)current->data;
        if (current_survivor->status == 0) {
            /*bulunan survivorun statusunu 1 yap*/
            current_survivor->status = 1;
            survivor = current_survivor;
            break;
        }
        current = current->next;
    }
    pthread_mutex_unlock(&mutex1);
    return survivor;
}

/*boşta olan droneları bulur*/
Drone* find_drone(List *list) {
    Drone *drone = NULL;
    Node *current;

    pthread_mutex_lock(&(list->mutex)); /*Listeyi kilitle*/

    while (list->is_writing) {
        pthread_cond_wait(&(list->cond), &(list->mutex)); /*Yazma işlemi varsa bekle*/
    }

    current = list->head;
    while (current != NULL) {
        Drone *current_drone = (Drone *)current->data;
        if (current_drone->status == 0) {
            drone = current_drone;
            drone->status=1;
            break;
        }
        current = current->next;
    }

    pthread_mutex_unlock(&(list->mutex)); 
    return drone;
}

/*yardım edilenlerin sayısını arttıran fonksiyon*/
void increase_helped(){
    pthread_mutex_lock(&mutex2);
    number_helped += 1;
    pthread_mutex_unlock(&mutex2);
}

/*drone nun bulunduğu kordinatta survivor varsa ona yardım eder*/
void check_and_remove_survivor_at_coord(Coord coord) {
    List *list = map.cells[coord.x][coord.y].survivors;
    Node *current = list->head;
    int survivor_count = list->number_of_elements;
    
    while (current != NULL) {
        Survivor *current_survivor = (Survivor *)current->data;
        if (current_survivor->status == 0) {
            /* Survivor bulundu, sil ve yardımcı ol*/
            help_survivor(NULL, current_survivor);
            return; /* Her seferinde sadece bir survivor kaldırıyoruz*/
        }
        current = current->next;
    }
}

/*THREAD FUNCTION: simulates a drone: */
/*yardıma ihtiyacı olan survivor u bulur 
* parametre olarak aldığı drone bu survivora yardım için gönderir
* drone destination u survivor coordinatlarına eşitler.
*
*/
void *drone_runner(void *drone) {
    Drone *d = (Drone *)drone;
    while(number_helped < 1000){
        Survivor* finded_survivor = find_survivor(survivors);
        if (finded_survivor == NULL) {
            printf("No unhelped survivor found.\n");
        }
        d->destination.x = finded_survivor->coord.x;
        d->destination.y = finded_survivor->coord.y;
        
        while (1) {
            // Hedefe olan farkı hesapla
            int x_farki = d->destination.x - d->coord.x;
            int y_farki = d->destination.y - d->coord.y;

            if (x_farki == 0 && y_farki == 0) {
                break;
            }else{
                move_drone(d);
            }

            // Belirli bir süre bekle (örneğin x saniye)
            sleep(1);
        }
        /*survivor ve drone bilgilerini ayarlar*/
        help_survivor(d, finded_survivor);
        /*yardım edilen sayısını arttırır*/
        increase_helped();
    }
    return NULL;
}

/*THREAD FUNCTION: an AI that controls drones based on survivors*/

/*drones listesinden boşta olan drone u bulup drone_runner fonksiyonuna paremetre olarak gönderir
* bu işlemi 10*100(10 drone*100=1000) defa çalışacak şekilde yapar
*drone_runner threadleri burada oluşturulur aynı anda 10 tane thread çalışır
*10 thread işlemi bittiğinde threadler durdulur ve yeniden oluşturulur
*bu işlem 100 defa tekrar eder
*/
void* drone_controller(void *args) {
    pthread_t drone_thread[10];
    Drone *finded_drone[10];
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);
    /*yeteri kadar survivor oluşmasını bekler*/
    sleep(15);
    /*aynı anda 10 tane drone threadi çalışır bu threadlerin sonlanması beklenir
    *sonrasında tekrardan 10 thread oluşturulur bu işlem 100 defa yapılır ki 1000 survivora yardım edilsin
    */
    for(int i=0; i<100; i++){
        for(int j = 0; j < 10; j++){
            finded_drone[j] = NULL;
            finded_drone[j] = find_drone(drones);
        }
        for(int k = 0; k < 10; k++){
            pthread_create(&drone_thread[k], NULL, drone_runner, (void *)finded_drone[k]);
        }
        for(int k = 0; k < 10; k++){
            pthread_join(drone_thread[k], NULL);
        }
    }
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    return NULL;
}

