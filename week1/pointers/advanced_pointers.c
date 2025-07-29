// advanced_pointers.c - 進階指標操作教學

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// === 1. 基礎複習 ===
void basic_pointer_review() {
    printf("\n=== 1. 基礎指標複習 ===\n");
    
    int value = 42;
    int *ptr = &value;  // ptr 指向 value
    
    printf("value = %d\n", value);
    printf("&value (address) = %p\n", (void*)&value);
    printf("ptr (stores address) = %p\n", (void*)ptr);
    printf("*ptr (dereference) = %d\n", *ptr);
}

// === 2. 指標的指標 ===
void pointer_to_pointer() {
    printf("\n=== 2. 指標的指標 ===\n");
    
    int value = 100;
    int *ptr1 = &value;
    int **ptr2 = &ptr1;  // ptr2 指向 ptr1
    
    printf("value = %d\n", value);
    printf("*ptr1 = %d\n", *ptr1);
    printf("**ptr2 = %d\n", **ptr2);
    
    // 透過 ptr2 修改 value
    **ptr2 = 200;
    printf("After **ptr2 = 200, value = %d\n", value);
}

// === 3. 動態記憶體管理 ===
typedef struct {
    int id;
    char name[50];
} Person;

void dynamic_memory_demo() {
    printf("\n=== 3. 動態記憶體管理 ===\n");
    
    // 分配單一整數
    int *single_int = (int*)malloc(sizeof(int));
    if (single_int == NULL) {
        printf("記憶體分配失敗!\n");
        return;
    }
    *single_int = 42;
    printf("動態分配的整數: %d\n", *single_int);
    
    // 分配整數陣列
    int n = 5;
    int *array = (int*)calloc(n, sizeof(int));  // calloc 初始化為 0
    if (array == NULL) {
        free(single_int);
        printf("記憶體分配失敗!\n");
        return;
    }
    
    // 填充陣列
    for (int i = 0; i < n; i++) {
        array[i] = i * 10;
        printf("array[%d] = %d\n", i, array[i]);
    }
    
    // 重新分配大小
    n = 10;
    int *temp = (int*)realloc(array, n * sizeof(int));
    if (temp == NULL) {
        free(single_int);
        free(array);
        printf("記憶體重新分配失敗!\n");
        return;
    }
    array = temp;
    
    // 填充新元素
    for (int i = 5; i < n; i++) {
        array[i] = i * 10;
    }
    printf("\n重新分配後的陣列:\n");
    for (int i = 0; i < n; i++) {
        printf("array[%d] = %d\n", i, array[i]);
    }
    
    // 分配結構體
    Person *person = (Person*)malloc(sizeof(Person));
    if (person == NULL) {
        free(single_int);
        free(array);
        printf("記憶體分配失敗!\n");
        return;
    }
    
    person->id = 1001;
    strcpy(person->name, "OpenBMC Developer");
    printf("\nPerson: ID=%d, Name=%s\n", person->id, person->name);
    
    // 重要：釋放記憶體
    free(single_int);
    free(array);
    free(person);
    printf("\n記憶體已釋放\n");
}

// === 4. 指標陣列 vs 陣列指標 ===
void pointer_arrays() {
    printf("\n=== 4. 指標陣列 vs 陣列指標 ===\n");
    
    // 指標陣列：陣列中的每個元素都是指標
    int a = 10, b = 20, c = 30;
    int *ptr_array[3] = {&a, &b, &c};
    
    printf("指標陣列:\n");
    for (int i = 0; i < 3; i++) {
        printf("ptr_array[%d] = %p, *ptr_array[%d] = %d\n", 
               i, (void*)ptr_array[i], i, *ptr_array[i]);
    }
    
    // 陣列指標：指向整個陣列的指標
    int arr[3] = {100, 200, 300};
    int (*array_ptr)[3] = &arr;
    
    printf("\n陣列指標:\n");
    for (int i = 0; i < 3; i++) {
        printf("(*array_ptr)[%d] = %d\n", i, (*array_ptr)[i]);
    }
}

// === 5. 記憶體洩漏偵測示範 ===
void memory_leak_demo() {
    printf("\n=== 5. 記憶體洩漏示範 ===\n");
    
    // 錯誤示範：記憶體洩漏
    // int *leak = (int*)malloc(sizeof(int) * 100);
    // 忘記 free(leak);
    
    // 正確做法：
    int *no_leak = (int*)malloc(sizeof(int) * 100);
    if (no_leak != NULL) {
        // 使用記憶體
        no_leak[0] = 42;
        printf("正確分配並使用記憶體: no_leak[0] = %d\n", no_leak[0]);
        
        // 釋放記憶體
        free(no_leak);
        no_leak = NULL;  // 避免懸空指標
        printf("記憶體已正確釋放\n");
    }
}

// === 6. 常見錯誤示範 ===
void common_mistakes() {
    printf("\n=== 6. 常見錯誤（註解說明） ===\n");
    
    // 錯誤 1：使用未初始化的指標
    // int *bad_ptr;
    // *bad_ptr = 10;  // 崩潰！
    
    // 錯誤 2：釋放後使用
    // int *ptr = (int*)malloc(sizeof(int));
    // free(ptr);
    // *ptr = 10;  // 未定義行為！
    
    // 錯誤 3：重複釋放
    // int *ptr = (int*)malloc(sizeof(int));
    // free(ptr);
    // free(ptr);  // 崩潰！
    
    // 錯誤 4：釋放非動態記憶體
    // int x = 10;
    // int *ptr = &x;
    // free(ptr);  // 崩潰！
    
    printf("請查看程式碼中的註解以了解常見錯誤\n");
}

// 主程式
int main() {
    printf("=== OpenBMC C語言進階指標教學 ===\n");
    
    basic_pointer_review();
    pointer_to_pointer();
    dynamic_memory_demo();
    pointer_arrays();
    memory_leak_demo();
    common_mistakes();
    
    printf("\n=== 教學完成 ===\n");
    printf("建議：\n");
    printf("1. 使用 valgrind 檢查記憶體洩漏\n");
    printf("   valgrind --leak-check=full ./advanced_pointers\n");
    printf("2. 編譯時加入 -Wall -Wextra 顯示所有警告\n");
    printf("   gcc -Wall -Wextra -g advanced_pointers.c -o advanced_pointers\n");
    
    return 0;
}
