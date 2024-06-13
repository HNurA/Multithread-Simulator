/*
 * controller.c
 *      Author: adaskin
 */

/*Begüm Karabaş 22120205002
 *Hilal Nur Albayrak 22120205056
 *2024.05.23
 *bu dosyada main fonksiyonu bulunur.
 *main fonksiyonunda model.c ve view.c dosyalarındaki fonksiyonlar çağırılır
 *
*/

#include "SDL2/SDL.h"
#include "simulator.h"

/*do not change any of this*/
extern SDL_bool done;

int main(int argc, char* argv[]) {

    /*thread tanımlamaları*/
    pthread_t thread_s_generator;
    pthread_t thread_controller;

    /*initializes map*/
    init_map(40, 30);

    /*initializes window*/
    printf("initialize window\n");
    init_sdl_window(map);

    printf("draw map\n");
    /*draws initial map*/
    draw_map();

    /*10 tane drone oluşturur*/
    for(int i=0 ; i<10 ; i++){
        drone_generator();
    }

    /*drone controller threadi oluşturulur*/
    pthread_create(&thread_controller, NULL, drone_controller, NULL);

    /* repeat until window is closed */
    while (!done) {

        /*check user events: e.g. click to close x*/
        check_events();

        /* update model:survivors, drones etc. */
        /*survivor generator threadi*/
        pthread_create(&thread_s_generator, NULL, survivor_generator, NULL);
        
        /*draws new updated map*/
        draw_map();

        /*survivor generator threadi bitene kadar bekle*/
        pthread_join(thread_s_generator, NULL);
   
        SDL_Delay(1000); /*sleep(1);*/
  
    }
    
    /*drone controller threadi bitene kadar bekle*/
    pthread_join(thread_controller, NULL);

    printf("quitting...\n");
    freemap();
    /*quit everything*/
    quit_all();
    return 0;
}
