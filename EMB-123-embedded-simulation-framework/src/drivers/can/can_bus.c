#include "can_bus.h"
#include "logger.h"
#include <pthread.h>
#include <string.h>

typedef struct {
    can_config_t config;
    can_stats_t stats;
    pthread_mutex_t mutex;
    pthread_t rx_thread;
    volatile bool initialized;
    volatile bool running;
    uint8_t rx_buffer[4096];
    uint32_t rx_head;
    uint32_t rx_tail;
} can_controller_t;

static can_controller_t g_can_controllers[2];

static uint32_t can_calculate_crc(const can_frame_t* frame) {
    uint32_t crc = frame->id;
    crc = crc * 31 + frame->dlc;
    for (int i = 0; i < frame->dlc; i++) {
        crc = crc * 31 + frame->data[i];
    }
    return crc;
}

static void* can_rx_task(void* arg) {
    can_controller_t* ctrl = (can_controller_t*)arg;
    
    while (ctrl->running) {
        #ifdef SIMULATION_MODE
        // Simulate CAN traffic
        can_frame_t sim_frame;
        sim_frame.id = rand() % 0x7FF;
        sim_frame.dlc = (rand() % 8) + 1;
        for (int i = 0; i < sim_frame.dlc; i++) {
            sim_frame.data[i] = rand() & 0xFF;
        }
        
        // Store in receive buffer
        pthread_mutex_lock(&ctrl->mutex);
        if ((ctrl->rx_head + sizeof(can_frame_t)) % sizeof(ctrl->rx_buffer) != ctrl->rx_tail) {
            memcpy(&ctrl->rx_buffer[ctrl->rx_head], &sim_frame, sizeof(can_frame_t));
            ctrl->rx_head = (ctrl->rx_head + sizeof(can_frame_t)) % sizeof(ctrl->rx_buffer);
            ctrl->stats.rx_frames++;
        }
        pthread_mutex_unlock(&ctrl->mutex);
        #endif
        
        delay_ms(1);
    }
    
    return NULL;
}

error_code_t can_init(const can_config_t* config) {
    if (!config) return ERR_INVALID_PARAM;
    if (config->controller_id > 1) return ERR_INVALID_PARAM;
    
    can_controller_t* ctrl = &g_can_controllers[config->controller_id];
    
    pthread_mutex_lock(&ctrl->mutex);
    
    memcpy(&ctrl->config, config, sizeof(can_config_t));
    memset(&ctrl->stats, 0, sizeof(can_stats_t));
    ctrl->initialized = true;
    ctrl->running = true;
    
    // Start RX thread
    if (pthread_create(&ctrl->rx_thread, NULL, can_rx_task, ctrl) != 0) {
        ctrl->initialized = false;
        pthread_mutex_unlock(&ctrl->mutex);
        return ERR_GENERAL;
    }
    
    LOG_INFO("CAN%d initialized at %d bps (loopback=%d)", 
             config->controller_id, config->bitrate, config->loopback);
    
    pthread_mutex_unlock(&ctrl->mutex);
    return ERR_OK;
}

error_code_t can_send(uint8_t controller_id, const can_frame_t* frame) {
    if (controller_id > 1) return ERR_INVALID_PARAM;
    if (!frame) return ERR_INVALID_PARAM;
    if (frame->dlc > 8) return ERR_INVALID_PARAM;
    
    can_controller_t* ctrl = &g_can_controllers[controller_id];
    if (!ctrl->initialized) return ERR_NOT_INITIALIZED;
    
    pthread_mutex_lock(&ctrl->mutex);
    
    uint32_t crc = can_calculate_crc(frame);
    ctrl->stats.tx_frames++;
    ctrl->stats.tx_bytes += frame->dlc;
    
    LOG_DEBUG("CAN%d: TX ID=0x%03X DLC=%d CRC=0x%04X", 
              controller_id, frame->id, frame->dlc, crc);
    
    // Loopback if enabled
    if (ctrl->config.loopback) {
        pthread_mutex_unlock(&ctrl->mutex);
        return can_receive_callback(controller_id, frame);
    }
    
    pthread_mutex_unlock(&ctrl->mutex);
    return ERR_OK;
}

error_code_t can_receive(uint8_t controller_id, can_frame_t* frame, uint32_t timeout_ms) {
    if (controller_id > 1) return ERR_INVALID_PARAM;
    if (!frame) return ERR_INVALID_PARAM;
    
    can_controller_t* ctrl = &g_can_controllers[controller_id];
    if (!ctrl->initialized) return ERR_NOT_INITIALIZED;
    
    uint32_t start_time = get_tick_ms();
    
    while (ctrl->rx_head == ctrl->rx_tail) {
        if ((get_tick_ms() - start_time) >= timeout_ms) {
            return ERR_TIMEOUT;
        }
        delay_ms(1);
    }
    
    pthread_mutex_lock(&ctrl->mutex);
    memcpy(frame, &ctrl->rx_buffer[ctrl->rx_tail], sizeof(can_frame_t));
    ctrl->rx_tail = (ctrl->rx_tail + sizeof(can_frame_t)) % sizeof(ctrl->rx_buffer);
    pthread_mutex_unlock(&ctrl->mutex);
    
    return ERR_OK;
}

error_code_t can_receive_callback(uint8_t controller_id, const can_frame_t* frame) {
    if (controller_id > 1) return ERR_INVALID_PARAM;
    if (!frame) return ERR_INVALID_PARAM;
    
    can_controller_t* ctrl = &g_can_controllers[controller_id];
    if (!ctrl->initialized) return ERR_NOT_INITIALIZED;
    
    pthread_mutex_lock(&ctrl->mutex);
    
    if ((ctrl->rx_head + sizeof(can_frame_t)) % sizeof(ctrl->rx_buffer) != ctrl->rx_tail) {
        memcpy(&ctrl->rx_buffer[ctrl->rx_head], frame, sizeof(can_frame_t));
        ctrl->rx_head = (ctrl->rx_head + sizeof(can_frame_t)) % sizeof(ctrl->rx_buffer);
        ctrl->stats.rx_frames++;
        ctrl->stats.rx_bytes += frame->dlc;
    }
    
    pthread_mutex_unlock(&ctrl->mutex);
    return ERR_OK;
}

can_stats_t can_get_stats(uint8_t controller_id) {
    can_stats_t empty_stats = {0};
    if (controller_id > 1) return empty_stats;
    
    can_controller_t* ctrl = &g_can_controllers[controller_id];
    return ctrl->stats;
}
