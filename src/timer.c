//
// Created by cameron on 10/14/2019.
//
#define DESIRED_TICK_RATE 60
#define TICK_RATE_DELTA (1.0 / DESIRED_TICK_RATE) * 1000.0

#include "chip8.h"
#include "log.h"

#include <sys/time.h>
#include <pthread.h>

uint8_t delay_should_quit = 0;
uint8_t sound_should_quit = 0;

struct timeval last_delay_time;
struct timeval last_sound_time;

void* __delay_timer(void*);
void* __sound_timer(void*);

pthread_t delay_timer_thread;
pthread_t sound_timer_thread;

void start_sound_timer() {
#ifdef DEBUG
    log_debug("tick_rate_delta is %f\n", TICK_RATE_DELTA);
#endif
    gettimeofday(&last_sound_time, NULL);
    pthread_create(&sound_timer_thread, NULL, &__sound_timer, NULL);
}

void start_delay_timer() {
    gettimeofday(&last_delay_time, NULL);
    pthread_create(&delay_timer_thread, NULL, &__delay_timer, NULL);
}

void stop_sound_timer() {
    sound_should_quit = 1;
}

void stop_delay_timer() {
    delay_should_quit = 1;
}

void* __delay_timer(void* __args) {
    while(delay_should_quit != 1) {
        struct timeval now;
        gettimeofday(&now, NULL);
        double now_in_ms = (now.tv_usec / 1000.0) + (now.tv_sec * 1000.0);
        double last_in_ms = (last_delay_time.tv_usec / 1000.0) + (last_delay_time.tv_sec * 1000.0);
        if(now_in_ms - last_in_ms > TICK_RATE_DELTA) {
            tick_delay_timer();
            gettimeofday(&last_delay_time, NULL);
        }
    }
}

void* __sound_timer(void* __args) {
    while(sound_should_quit != 1) {
        struct timeval now;
        gettimeofday(&now, NULL);
        double now_in_ms = (now.tv_usec / 1000.0) + (now.tv_sec * 1000.0);
        double last_in_ms = (last_delay_time.tv_usec / 1000.0) + (last_sound_time.tv_sec * 1000.0);
        if(now_in_ms - last_in_ms > TICK_RATE_DELTA) {
            tick_sound_timer();
            gettimeofday(&last_sound_time, NULL);
        }
    }
}