// misra_c_basics.c - MISRA-C 編碼標準基礎教學
// MISRA-C 是汽車工業軟體可靠性協會制定的 C 語言編碼標準
// 在 BMC 等關鍵系統開發中，遵循類似標準很重要

#include <stdio.h>
#include <stdint.h>  // MISRA 建議使用固定大小的整數類型
#include <stdbool.h> // MISRA 建議使用 bool 而非 int 表示布林值
#include <string.h>

// === MISRA-C 核心原則 ===
// 1. 避免未定義行為
// 2. 避免實作定義行為
// 3. 促進可移植性
// 4. 促進可讀性和可維護性

// === 規則示範 1: 使用固定大小的整數類型 ===
// 錯誤：使用平台相關的類型
// int sensor_value;  // 在不同平台上大小可能不同

// 正確：使用固定大小類型
typedef struct {
    uint16_t temperature;  // 明確的 16 位元無符號整數
    int32_t  pressure;     // 明確的 32 位元有符號整數
    uint8_t  status;       // 明確的 8 位元無符號整數
} SensorData;

// === 規則示範 2: 避免隱式類型轉換 ===
void type_conversion_demo(void) {
    printf("\n=== MISRA 規則: 避免隱式類型轉換 ===\n");
    
    uint16_t u16_value = 100U;
    uint32_t u32_value;
    
    // 錯誤：隱式類型轉換
    // u32_value = u16_value * 1000;  // 可能溢位
    
    // 正確：明確類型轉換
    u32_value = (uint32_t)u16_value * 1000U;
    printf("正確的類型轉換: %u * 1000 = %u\n", u16_value, u32_value);
}

// === 規則示範 3: 函數應有單一退出點 ===
// 錯誤示範
/*
int bad_function(int value) {
    if (value < 0) {
        return -1;  // 多個退出點
    }
    if (value > 100) {
        return -2;  // 多個退出點
    }
    return value * 2;
}
*/

// 正確示範：單一退出點
int32_t process_sensor_value(int32_t value) {
    int32_t result;
    
    if (value < 0) {
        result = -1;
    } else if (value > 100) {
        result = -2;
    } else {
        result = value * 2;
    }
    
    return result;  // 單一退出點
}

// === 規則示範 4: 初始化所有變數 ===
void initialization_demo(void) {
    printf("\n=== MISRA 規則: 初始化所有變數 ===\n");
    
    // 錯誤：未初始化的變數
    // int uninitialized;
    // printf("%d\n", uninitialized);  // 未定義行為！
    
    // 正確：總是初始化
    uint32_t sensor_count = 0U;
    float temperature = 0.0f;
    char buffer[100] = {0};  // 初始化整個陣列
    
    printf("已初始化的變數: count=%u, temp=%.1f\n", 
           sensor_count, temperature);
}

// === 規則示範 5: 避免使用動態記憶體分配 ===
// MISRA 通常禁止 malloc/free，但 OpenBMC 中可能需要
// 這裡示範安全使用的方法

#define MAX_SENSORS 10

typedef struct {
    uint32_t sensor_ids[MAX_SENSORS];
    uint32_t sensor_count;
} StaticSensorArray;

void static_allocation_demo(void) {
    printf("\n=== MISRA 規則: 優先使用靜態分配 ===\n");
    
    // 使用靜態分配而非動態分配
    StaticSensorArray sensors = {
        .sensor_ids = {0},
        .sensor_count = 0U
    };
    
    // 添加感測器
    for (uint32_t i = 0U; i < 5U; i++) {
        if (sensors.sensor_count < MAX_SENSORS) {
            sensors.sensor_ids[sensors.sensor_count] = 1000U + i;
            sensors.sensor_count++;
        }
    }
    
    printf("靜態分配的感測器陣列:\n");
    for (uint32_t i = 0U; i < sensors.sensor_count; i++) {
        printf("  感測器 ID: %u\n", sensors.sensor_ids[i]);
    }
}

// === 規則示範 6: 使用括號明確運算順序 ===
void operator_precedence_demo(void) {
    printf("\n=== MISRA 規則: 明確運算順序 ===\n");
    
    uint32_t a = 10U;
    uint32_t b = 20U;
    uint32_t c = 30U;
    uint32_t result;
    
    // 錯誤：依賴運算子優先順序
    // result = a + b * c;  // 容易誤解
    
    // 正確：使用括號明確順序
    result = a + (b * c);
    printf("a + (b * c) = %u + (%u * %u) = %u\n", a, b, c, result);
    
    result = (a + b) * c;
    printf("(a + b) * c = (%u + %u) * %u = %u\n", a, b, c, result);
}

// === 規則示範 7: 檢查函數返回值 ===
typedef enum {
    BMC_OK = 0,
    BMC_ERROR_INVALID_PARAM = -1,
    BMC_ERROR_TIMEOUT = -2,
    BMC_ERROR_HARDWARE = -3
} BMCStatus;

BMCStatus read_sensor(uint32_t sensor_id, uint16_t *value) {
    // 參數檢查
    if (value == NULL) {
        return BMC_ERROR_INVALID_PARAM;
    }
    
    // 模擬感測器讀取
    if (sensor_id == 0U) {
        return BMC_ERROR_HARDWARE;
    }
    
    *value = 25U;  // 模擬溫度值
    return BMC_OK;
}

void return_value_check_demo(void) {
    printf("\n=== MISRA 規則: 檢查函數返回值 ===\n");
    
    uint16_t temperature = 0U;
    BMCStatus status;
    
    // 總是檢查返回值
    status = read_sensor(1U, &temperature);
    if (status == BMC_OK) {
        printf("成功讀取溫度: %u°C\n", temperature);
    } else {
        printf("讀取失敗，錯誤碼: %d\n", status);
    }
    
    // 測試錯誤情況
    status = read_sensor(0U, &temperature);
    if (status != BMC_OK) {
        printf("預期的錯誤: 硬體錯誤\n");
    }
}

// === 規則示範 8: 避免魔術數字 ===
// 錯誤：直接使用數字
/*
if (temperature > 75) {  // 75 是什麼？
    start_fan();
}
*/

// 正確：使用具名常數
#define TEMPERATURE_WARNING_THRESHOLD_C  75U
#define TEMPERATURE_CRITICAL_THRESHOLD_C 85U
#define FAN_SPEED_MIN_RPM               1000U
#define FAN_SPEED_MAX_RPM               5000U

void named_constants_demo(void) {
    printf("\n=== MISRA 規則: 使用具名常數 ===\n");
    
    uint16_t cpu_temp = 80U;
    
    if (cpu_temp > TEMPERATURE_WARNING_THRESHOLD_C) {
        printf("溫度警告: %u°C 超過警告閾值 %u°C\n", 
               cpu_temp, TEMPERATURE_WARNING_THRESHOLD_C);
    }
    
    if (cpu_temp > TEMPERATURE_CRITICAL_THRESHOLD_C) {
        printf("溫度危急: %u°C 超過危急閾值 %u°C\n", 
               cpu_temp, TEMPERATURE_CRITICAL_THRESHOLD_C);
    }
}

// === 規則示範 9: 適當的註解 ===
/**
 * @brief 計算風扇目標轉速
 * 
 * 根據溫度計算適當的風扇轉速。
 * 使用線性插值在最小和最大轉速之間。
 * 
 * @param temperature 當前溫度 (攝氏度)
 * @return uint16_t 目標風扇轉速 (RPM)
 */
uint16_t calculate_fan_speed(uint16_t temperature) {
    uint16_t fan_speed;
    
    /* 溫度低於警告閾值，使用最小轉速 */
    if (temperature < TEMPERATURE_WARNING_THRESHOLD_C) {
        fan_speed = FAN_SPEED_MIN_RPM;
    }
    /* 溫度高於危急閾值，使用最大轉速 */
    else if (temperature >= TEMPERATURE_CRITICAL_THRESHOLD_C) {
        fan_speed = FAN_SPEED_MAX_RPM;
    }
    /* 在警告和危急之間，線性插值 */
    else {
        uint32_t temp_range = TEMPERATURE_CRITICAL_THRESHOLD_C - 
                             TEMPERATURE_WARNING_THRESHOLD_C;
        uint32_t speed_range = FAN_SPEED_MAX_RPM - FAN_SPEED_MIN_RPM;
        uint32_t temp_offset = temperature - TEMPERATURE_WARNING_THRESHOLD_C;
        
        fan_speed = FAN_SPEED_MIN_RPM + 
                   (uint16_t)((temp_offset * speed_range) / temp_range);
    }
    
    return fan_speed;
}

// === 主程式 ===
int main(void) {
    printf("=== MISRA-C 編碼標準基礎教學 ===\n");
    printf("適用於 OpenBMC 等關鍵系統開發\n");
    
    // 執行各個示範
    type_conversion_demo();
    initialization_demo();
    static_allocation_demo();
    operator_precedence_demo();
    return_value_check_demo();
    named_constants_demo();
    
    // 示範風扇控制邏輯
    printf("\n=== 風扇控制邏輯示範 ===\n");
    uint16_t test_temps[] = {60U, 75U, 80U, 85U, 90U};
    
    for (uint32_t i = 0U; i < (sizeof(test_temps) / sizeof(test_temps[0])); i++) {
        uint16_t temp = test_temps[i];
        uint16_t fan_speed = calculate_fan_speed(temp);
        printf("溫度: %u°C -> 風扇轉速: %u RPM\n", temp, fan_speed);
    }
    
    printf("\n=== MISRA-C 重點總結 ===\n");
    printf("1. 使用固定大小的資料類型 (uint8_t, int32_t 等)\n");
    printf("2. 避免隱式類型轉換\n");
    printf("3. 初始化所有變數\n");
    printf("4. 檢查所有函數返回值\n");
    printf("5. 使用具名常數取代魔術數字\n");
    printf("6. 撰寫清晰的註解和文件\n");
    printf("7. 保持程式碼簡單且可預測\n");
    
    return 0;
}
