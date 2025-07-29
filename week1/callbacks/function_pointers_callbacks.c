// function_pointers_callbacks.c - 函數指標與回調機制教學

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// === 1. 函數指標基礎 ===
// 簡單的數學運算函數
int add(int a, int b) {
    return a + b;
}

int subtract(int a, int b) {
    return a - b;
}

int multiply(int a, int b) {
    return a * b;
}

void function_pointer_basics() {
    printf("\n=== 1. 函數指標基礎 ===\n");
    
    // 宣告函數指標：返回 int，接受兩個 int 參數
    int (*operation)(int, int);
    
    // 方法 1：直接指派
    operation = add;
    printf("10 + 5 = %d\n", operation(10, 5));
    
    // 方法 2：使用 & (可選)
    operation = &subtract;
    printf("10 - 5 = %d\n", operation(10, 5));
    
    // 方法 3：透過指標呼叫
    operation = multiply;
    printf("10 * 5 = %d\n", (*operation)(10, 5));  // 也可以這樣寫
}

// === 2. 函數指標陣列 ===
void function_pointer_array() {
    printf("\n=== 2. 函數指標陣列 ===\n");
    
    // 建立函數指標陣列
    int (*operations[3])(int, int) = {add, subtract, multiply};
    const char *op_names[3] = {"加法", "減法", "乘法"};
    
    int a = 20, b = 10;
    
    for (int i = 0; i < 3; i++) {
        printf("%s: %d 和 %d = %d\n", 
               op_names[i], a, b, operations[i](a, b));
    }
}

// === 3. 回調函數範例 ===
// 定義回調函數類型
typedef void (*EventCallback)(const char *event_name, int value);

// 事件處理器結構
typedef struct {
    EventCallback on_sensor_change;
    EventCallback on_threshold_exceeded;
} EventHandlers;

// 回調函數實作
void sensor_changed_handler(const char *event_name, int value) {
    printf("[SENSOR] %s: 新數值 = %d°C\n", event_name, value);
}

void threshold_exceeded_handler(const char *event_name, int value) {
    printf("[警告] %s: 溫度過高! 當前溫度 = %d°C\n", event_name, value);
}

// 模擬 BMC 感測器讀取
void simulate_sensor_reading(EventHandlers *handlers) {
    printf("\n=== 3. 回調函數模擬 BMC 感測器 ===\n");
    
    // 模擬溫度變化
    int temperatures[] = {45, 50, 75, 85, 60};
    const int threshold = 70;
    
    for (int i = 0; i < 5; i++) {
        int temp = temperatures[i];
        
        // 觸發感測器變化事件
        if (handlers->on_sensor_change) {
            handlers->on_sensor_change("CPU溫度", temp);
        }
        
        // 檢查是否超過閾值
        if (temp > threshold && handlers->on_threshold_exceeded) {
            handlers->on_threshold_exceeded("CPU溫度", temp);
        }
        
        printf("---\n");
    }
}

// === 4. 進階回調：排序演算法 ===
// 比較函數類型
typedef int (*CompareFunc)(const void *, const void *);

// 升序比較
int compare_ascending(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

// 降序比較
int compare_descending(const void *a, const void *b) {
    return (*(int*)b - *(int*)a);
}

// 通用排序函數（使用回調）
void sort_array(int *array, int size, CompareFunc compare) {
    // 使用標準庫的 qsort，它接受回調函數
    qsort(array, size, sizeof(int), compare);
}

void sorting_callback_demo() {
    printf("\n=== 4. 排序回調示範 ===\n");
    
    int data[] = {64, 34, 25, 12, 22, 11, 90};
    int size = sizeof(data) / sizeof(data[0]);
    
    // 顯示原始陣列
    printf("原始陣列: ");
    for (int i = 0; i < size; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");
    
    // 升序排序
    sort_array(data, size, compare_ascending);
    printf("升序排序: ");
    for (int i = 0; i < size; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");
    
    // 降序排序
    sort_array(data, size, compare_descending);
    printf("降序排序: ");
    for (int i = 0; i < size; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");
}

// === 5. 函數指標作為結構成員 ===
// 定義 BMC 元件介面
typedef struct {
    char name[50];
    int (*init)(void);
    int (*read)(void);
    void (*cleanup)(void);
} BMCComponent;

// 溫度感測器實作
int temp_sensor_init(void) {
    printf("初始化溫度感測器...\n");
    return 0;  // 成功
}

int temp_sensor_read(void) {
    // 模擬讀取溫度
    return 25 + (rand() % 40);  // 25-64°C
}

void temp_sensor_cleanup(void) {
    printf("清理溫度感測器資源...\n");
}

// 風扇控制器實作
int fan_controller_init(void) {
    printf("初始化風扇控制器...\n");
    return 0;
}

int fan_controller_read(void) {
    // 模擬讀取風扇轉速 (RPM)
    return 1000 + (rand() % 2000);  // 1000-2999 RPM
}

void fan_controller_cleanup(void) {
    printf("清理風扇控制器資源...\n");
}

void component_interface_demo() {
    printf("\n=== 5. BMC 元件介面示範 ===\n");
    
    // 建立元件陣列
    BMCComponent components[] = {
        {
            .name = "CPU溫度感測器",
            .init = temp_sensor_init,
            .read = temp_sensor_read,
            .cleanup = temp_sensor_cleanup
        },
        {
            .name = "系統風扇",
            .init = fan_controller_init,
            .read = fan_controller_read,
            .cleanup = fan_controller_cleanup
        }
    };
    
    int num_components = sizeof(components) / sizeof(components[0]);
    
    // 初始化所有元件
    printf("初始化 BMC 元件:\n");
    for (int i = 0; i < num_components; i++) {
        printf("- %s: ", components[i].name);
        if (components[i].init() == 0) {
            printf("成功\n");
        } else {
            printf("失敗\n");
        }
    }
    
    // 讀取元件數據
    printf("\n讀取元件數據:\n");
    for (int j = 0; j < 3; j++) {
        printf("第 %d 次讀取:\n", j + 1);
        for (int i = 0; i < num_components; i++) {
            int value = components[i].read();
            if (i == 0) {
                printf("  - %s: %d°C\n", components[i].name, value);
            } else {
                printf("  - %s: %d RPM\n", components[i].name, value);
            }
        }
    }
    
    // 清理資源
    printf("\n清理 BMC 元件:\n");
    for (int i = 0; i < num_components; i++) {
        printf("- %s: ", components[i].name);
        components[i].cleanup();
    }
}

// 主程式
int main() {
    printf("=== OpenBMC 函數指標與回調機制教學 ===\n");
    
    // 執行各個示範
    function_pointer_basics();
    function_pointer_array();
    
    // 設定事件處理器
    EventHandlers handlers = {
        .on_sensor_change = sensor_changed_handler,
        .on_threshold_exceeded = threshold_exceeded_handler
    };
    simulate_sensor_reading(&handlers);
    
    sorting_callback_demo();
    component_interface_demo();
    
    printf("\n=== 教學完成 ===\n");
    printf("重點總結:\n");
    printf("1. 函數指標儲存函數的記憶體位址\n");
    printf("2. 回調函數讓程式更靈活，可在執行時決定行為\n");
    printf("3. 在 OpenBMC 中，回調廣泛用於事件處理和硬體抽象\n");
    
    return 0;
}
