// fan_control_state_machine.c - 基於回調的 BMC 風扇控制狀態機
// 這是一個完整的狀態機實作，模擬 OpenBMC 中的風扇控制邏輯

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// === 常數定義 (遵循 MISRA-C) ===
#define MAX_TEMPERATURE_NORMAL_C    50U
#define MAX_TEMPERATURE_WARNING_C   70U
#define MAX_TEMPERATURE_CRITICAL_C  85U
#define MAX_TEMPERATURE_SHUTDOWN_C  95U

#define FAN_SPEED_OFF_PERCENT       0U
#define FAN_SPEED_LOW_PERCENT       30U
#define FAN_SPEED_MEDIUM_PERCENT    60U
#define FAN_SPEED_HIGH_PERCENT      85U
#define FAN_SPEED_MAX_PERCENT       100U

#define MAX_EVENT_NAME_LENGTH       50
#define TEMPERATURE_CHECK_INTERVAL  1000  // 毫秒

// === 狀態定義 ===
typedef enum {
    STATE_IDLE,
    STATE_NORMAL,
    STATE_WARNING,
    STATE_CRITICAL,
    STATE_EMERGENCY_COOLING,
    STATE_SHUTDOWN,
    STATE_COUNT  // 狀態總數
} SystemState;

// === 事件定義 ===
typedef enum {
    EVENT_TEMP_NORMAL,
    EVENT_TEMP_WARNING,
    EVENT_TEMP_CRITICAL,
    EVENT_TEMP_EXTREME,
    EVENT_COOLING_SUCCESS,
    EVENT_COOLING_FAILURE,
    EVENT_SYSTEM_INIT,
    EVENT_COUNT  // 事件總數
} SystemEvent;

// === 前向宣告 ===
typedef struct StateMachine StateMachine;

// === 回調函數類型定義 ===
// 狀態進入/退出回調
typedef void (*StateCallback)(StateMachine *sm);

// 事件處理回調
typedef SystemState (*EventHandler)(StateMachine *sm, SystemEvent event);

// 動作回調
typedef void (*ActionCallback)(StateMachine *sm, uint8_t parameter);

// === 狀態配置結構 ===
typedef struct {
    const char *name;
    StateCallback on_enter;
    StateCallback on_exit;
    EventHandler handle_event;
} StateConfig;

// === 狀態機結構 ===
struct StateMachine {
    SystemState current_state;
    SystemState previous_state;
    
    // 系統數據
    uint16_t current_temperature;
    uint8_t current_fan_speed;
    uint32_t state_entry_time;
    bool emergency_cooling_active;
    
    // 回調函數
    ActionCallback set_fan_speed;
    ActionCallback log_message;
    
    // 統計資訊
    uint32_t state_transitions;
    uint32_t events_processed;
};

// === 動作回調實作 ===
void action_set_fan_speed(StateMachine *sm, uint8_t speed_percent) {
    sm->current_fan_speed = speed_percent;
    printf("[動作] 設定風扇速度: %u%%\n", speed_percent);
}

void action_log_message(StateMachine *sm, uint8_t severity) {
    const char *severity_str[] = {"INFO", "WARNING", "ERROR", "CRITICAL"};
    printf("[日誌][%s] 狀態: %d, 溫度: %u°C, 風扇: %u%%\n",
           severity_str[severity % 4],
           sm->current_state,
           sm->current_temperature,
           sm->current_fan_speed);
}

// === IDLE 狀態處理 ===
void state_idle_enter(StateMachine *sm) {
    printf("\n[狀態] 進入 IDLE (閒置)\n");
    sm->set_fan_speed(sm, FAN_SPEED_OFF_PERCENT);
    sm->log_message(sm, 0);  // INFO
}

void state_idle_exit(StateMachine *sm) {
    printf("[狀態] 離開 IDLE\n");
}

SystemState state_idle_event(StateMachine *sm, SystemEvent event) {
    switch (event) {
        case EVENT_SYSTEM_INIT:
            return STATE_NORMAL;
        case EVENT_TEMP_NORMAL:
            return STATE_NORMAL;
        default:
            return STATE_IDLE;  // 保持當前狀態
    }
}

// === NORMAL 狀態處理 ===
void state_normal_enter(StateMachine *sm) {
    printf("\n[狀態] 進入 NORMAL (正常運行)\n");
    sm->set_fan_speed(sm, FAN_SPEED_LOW_PERCENT);
    sm->log_message(sm, 0);  // INFO
}

void state_normal_exit(StateMachine *sm) {
    printf("[狀態] 離開 NORMAL\n");
}

SystemState state_normal_event(StateMachine *sm, SystemEvent event) {
    switch (event) {
        case EVENT_TEMP_WARNING:
            return STATE_WARNING;
        case EVENT_TEMP_CRITICAL:
            return STATE_CRITICAL;
        case EVENT_TEMP_EXTREME:
            return STATE_EMERGENCY_COOLING;
        default:
            return STATE_NORMAL;
    }
}

// === WARNING 狀態處理 ===
void state_warning_enter(StateMachine *sm) {
    printf("\n[狀態] 進入 WARNING (溫度警告)\n");
    sm->set_fan_speed(sm, FAN_SPEED_MEDIUM_PERCENT);
    sm->log_message(sm, 1);  // WARNING
}

void state_warning_exit(StateMachine *sm) {
    printf("[狀態] 離開 WARNING\n");
}

SystemState state_warning_event(StateMachine *sm, SystemEvent event) {
    switch (event) {
        case EVENT_TEMP_NORMAL:
            return STATE_NORMAL;
        case EVENT_TEMP_CRITICAL:
            return STATE_CRITICAL;
        case EVENT_TEMP_EXTREME:
            return STATE_EMERGENCY_COOLING;
        case EVENT_COOLING_SUCCESS:
            return STATE_NORMAL;
        default:
            return STATE_WARNING;
    }
}

// === CRITICAL 狀態處理 ===
void state_critical_enter(StateMachine *sm) {
    printf("\n[狀態] 進入 CRITICAL (溫度危急)\n");
    sm->set_fan_speed(sm, FAN_SPEED_HIGH_PERCENT);
    sm->log_message(sm, 2);  // ERROR
}

void state_critical_exit(StateMachine *sm) {
    printf("[狀態] 離開 CRITICAL\n");
}

SystemState state_critical_event(StateMachine *sm, SystemEvent event) {
    switch (event) {
        case EVENT_TEMP_NORMAL:
            return STATE_NORMAL;
        case EVENT_TEMP_WARNING:
            return STATE_WARNING;
        case EVENT_TEMP_EXTREME:
            return STATE_EMERGENCY_COOLING;
        case EVENT_COOLING_SUCCESS:
            return STATE_WARNING;
        default:
            return STATE_CRITICAL;
    }
}

// === EMERGENCY_COOLING 狀態處理 ===
void state_emergency_enter(StateMachine *sm) {
    printf("\n[狀態] 進入 EMERGENCY_COOLING (緊急冷卻)\n");
    sm->set_fan_speed(sm, FAN_SPEED_MAX_PERCENT);
    sm->emergency_cooling_active = true;
    sm->log_message(sm, 3);  // CRITICAL
}

void state_emergency_exit(StateMachine *sm) {
    printf("[狀態] 離開 EMERGENCY_COOLING\n");
    sm->emergency_cooling_active = false;
}

SystemState state_emergency_event(StateMachine *sm, SystemEvent event) {
    switch (event) {
        case EVENT_COOLING_SUCCESS:
            return STATE_WARNING;
        case EVENT_COOLING_FAILURE:
            return STATE_SHUTDOWN;
        case EVENT_TEMP_NORMAL:
            return STATE_NORMAL;
        case EVENT_TEMP_WARNING:
            return STATE_WARNING;
        default:
            return STATE_EMERGENCY_COOLING;
    }
}

// === SHUTDOWN 狀態處理 ===
void state_shutdown_enter(StateMachine *sm) {
    printf("\n[狀態] 進入 SHUTDOWN (系統關機)\n");
    printf("!!! 系統因過熱而關機 !!!\n");
    sm->set_fan_speed(sm, FAN_SPEED_MAX_PERCENT);  // 保持最大風扇
    sm->log_message(sm, 3);  // CRITICAL
}

void state_shutdown_exit(StateMachine *sm) {
    // 通常不會離開關機狀態
    printf("[狀態] 離開 SHUTDOWN\n");
}

SystemState state_shutdown_event(StateMachine *sm, SystemEvent event) {
    // 關機狀態下只響應系統初始化
    if (event == EVENT_SYSTEM_INIT) {
        return STATE_IDLE;
    }
    return STATE_SHUTDOWN;
}

// === 狀態配置表 ===
static const StateConfig state_configs[STATE_COUNT] = {
    [STATE_IDLE] = {
        .name = "IDLE",
        .on_enter = state_idle_enter,
        .on_exit = state_idle_exit,
        .handle_event = state_idle_event
    },
    [STATE_NORMAL] = {
        .name = "NORMAL",
        .on_enter = state_normal_enter,
        .on_exit = state_normal_exit,
        .handle_event = state_normal_event
    },
    [STATE_WARNING] = {
        .name = "WARNING",
        .on_enter = state_warning_enter,
        .on_exit = state_warning_exit,
        .handle_event = state_warning_event
    },
    [STATE_CRITICAL] = {
        .name = "CRITICAL",
        .on_enter = state_critical_enter,
        .on_exit = state_critical_exit,
        .handle_event = state_critical_event
    },
    [STATE_EMERGENCY_COOLING] = {
        .name = "EMERGENCY_COOLING",
        .on_enter = state_emergency_enter,
        .on_exit = state_emergency_exit,
        .handle_event = state_emergency_event
    },
    [STATE_SHUTDOWN] = {
        .name = "SHUTDOWN",
        .on_enter = state_shutdown_enter,
        .on_exit = state_shutdown_exit,
        .handle_event = state_shutdown_event
    }
};

// === 狀態機核心函數 ===
void sm_init(StateMachine *sm) {
    memset(sm, 0, sizeof(StateMachine));
    
    sm->current_state = STATE_IDLE;
    sm->previous_state = STATE_IDLE;
    sm->current_temperature = 25U;  // 室溫
    sm->current_fan_speed = 0U;
    
    // 設定動作回調
    sm->set_fan_speed = action_set_fan_speed;
    sm->log_message = action_log_message;
    
    printf("=== BMC 風扇控制狀態機初始化 ===\n");
}

void sm_transition(StateMachine *sm, SystemState new_state) {
    if (new_state >= STATE_COUNT) {
        printf("[錯誤] 無效的狀態: %d\n", new_state);
        return;
    }
    
    // 執行當前狀態的退出回調
    if (state_configs[sm->current_state].on_exit) {
        state_configs[sm->current_state].on_exit(sm);
    }
    
    // 更新狀態
    sm->previous_state = sm->current_state;
    sm->current_state = new_state;
    sm->state_entry_time = (uint32_t)time(NULL);
    sm->state_transitions++;
    
    printf("[轉換] %s -> %s\n", 
           state_configs[sm->previous_state].name,
           state_configs[sm->current_state].name);
    
    // 執行新狀態的進入回調
    if (state_configs[sm->current_state].on_enter) {
        state_configs[sm->current_state].on_enter(sm);
    }
}

void sm_process_event(StateMachine *sm, SystemEvent event) {
    if (event >= EVENT_COUNT) {
        printf("[錯誤] 無效的事件: %d\n", event);
        return;
    }
    
    sm->events_processed++;
    
    // 使用當前狀態的事件處理器
    EventHandler handler = state_configs[sm->current_state].handle_event;
    if (handler) {
        SystemState new_state = handler(sm, event);
        
        // 如果需要轉換狀態
        if (new_state != sm->current_state) {
            sm_transition(sm, new_state);
        }
    }
}

// === 溫度監控函數 ===
SystemEvent get_temperature_event(uint16_t temperature) {
    if (temperature >= MAX_TEMPERATURE_SHUTDOWN_C) {
        return EVENT_TEMP_EXTREME;
    } else if (temperature >= MAX_TEMPERATURE_CRITICAL_C) {
        return EVENT_TEMP_CRITICAL;
    } else if (temperature >= MAX_TEMPERATURE_WARNING_C) {
        return EVENT_TEMP_WARNING;
    } else {
        return EVENT_TEMP_NORMAL;
    }
}

// === 模擬溫度變化 ===
void simulate_temperature_change(StateMachine *sm, int change) {
    int new_temp = (int)sm->current_temperature + change;
    
    // 限制溫度範圍
    if (new_temp < 20) new_temp = 20;
    if (new_temp > 100) new_temp = 100;
    
    sm->current_temperature = (uint16_t)new_temp;
    
    printf("\n[感測器] 溫度變化: %d°C %s %u°C\n", 
           sm->current_temperature - change,
           (change > 0) ? "->" : "<-",
           sm->current_temperature);
}

// === 主程式：狀態機演示 ===
int main(void) {
    StateMachine sm;
    sm_init(&sm);
    
    printf("\n=== 開始狀態機模擬 ===\n");
    
    // 初始化系統
    sm_process_event(&sm, EVENT_SYSTEM_INIT);
    
    // 模擬場景
    printf("\n--- 場景 1: 正常運行 ---\n");
    simulate_temperature_change(&sm, 20);  // 45°C
    sm_process_event(&sm, get_temperature_event(sm.current_temperature));
    
    printf("\n--- 場景 2: 溫度上升至警告 ---\n");
    simulate_temperature_change(&sm, 30);  // 75°C
    sm_process_event(&sm, get_temperature_event(sm.current_temperature));
    
    printf("\n--- 場景 3: 溫度繼續上升至危急 ---\n");
    simulate_temperature_change(&sm, 15);  // 90°C
    sm_process_event(&sm, get_temperature_event(sm.current_temperature));
    
    printf("\n--- 場景 4: 極端溫度，觸發緊急冷卻 ---\n");
    simulate_temperature_change(&sm, 8);   // 98°C
    sm_process_event(&sm, get_temperature_event(sm.current_temperature));
    
    printf("\n--- 場景 5: 冷卻成功 ---\n");
    simulate_temperature_change(&sm, -30); // 68°C
    sm_process_event(&sm, EVENT_COOLING_SUCCESS);
    
    printf("\n--- 場景 6: 溫度回到正常 ---\n");
    simulate_temperature_change(&sm, -25); // 43°C
    sm_process_event(&sm, get_temperature_event(sm.current_temperature));
    
    // 顯示統計
    printf("\n=== 狀態機統計 ===\n");
    printf("狀態轉換次數: %u\n", sm.state_transitions);
    printf("處理事件次數: %u\n", sm.events_processed);
    printf("最終狀態: %s\n", state_configs[sm.current_state].name);
    printf("最終溫度: %u°C\n", sm.current_temperature);
    printf("最終風扇速度: %u%%\n", sm.current_fan_speed);
    
    return 0;
}
