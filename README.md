# OpenBMC Training - Firmware Development Journey 🚀

📋 專案簡介
這是我為了準備 OpenBMC 韌體工程師所進行的系統性訓練專案。透過實作導向的學習方式，深入掌握嵌入式系統開發的核心技術。

🎯 學習目標

精通 C 語言在嵌入式系統中的應用
理解並實作 OpenBMC 相關技術
掌握 Linux 核心程式設計基礎
熟悉業界標準的編碼規範（MISRA-C）
建立良好的軟體工程實踐

📁 專案結構

```bash
openbmc-training/
├── README.md
├── .gitignore
└── week1/                              # 第一週：C 語言進階
    ├── pointers/                       # 進階指標操作
    │   └── advanced_pointers.c
    ├── callbacks/                      # 函數指標與回調機制
    │   └── function_pointers_callbacks.c
    ├── misra/                          # MISRA-C 編碼標準
    │   └── misra_c_basics.c
    └── state-machine/                  # 狀態機實作
        └── fan_control_state_machine.c
```
        
🔧 Week 1: C 語言進階技術
1️⃣ 進階指標操作 (pointers/)
學習重點：

指標的指標（多層指標）
動態記憶體管理（malloc/calloc/realloc/free）
記憶體洩漏預防與偵測
指標陣列 vs 陣列指標

核心概念展示：

```bash
// 動態二維陣列分配
int **matrix = (int**)malloc(rows * sizeof(int*));
// 記憶體安全釋放模式
```

2️⃣ 函數指標與回調機制 (callbacks/)
學習重點：

函數指標語法與應用
事件驅動程式設計
回調函數在嵌入式系統中的應用
模擬 BMC 感測器事件系統

實作特色：

BMC 元件介面抽象化
事件處理器架構
可擴展的系統設計

3️⃣ MISRA-C 編碼標準 (misra/)
學習重點：

嵌入式系統安全編碼實踐
固定大小資料類型（uint8_t, int32_t）
避免未定義行為
適當的錯誤處理模式

應用範例：

類型安全的感測器數據處理
單一退出點函數設計
明確的運算順序

4️⃣ 狀態機實作 (state-machine/)
專案亮點： 完整的 BMC 風扇控制系統模擬
實作功能：

6 種系統狀態（IDLE → NORMAL → WARNING → CRITICAL → EMERGENCY → SHUTDOWN）
基於溫度的智能轉換邏輯
事件驅動架構
回調函數整合

```bash
狀態轉換圖：
[IDLE] --初始化--> [NORMAL] --溫度上升--> [WARNING]
                      ↑                        ↓
                      └──── 溫度正常 ←─────────┘
                                          溫度危急
                                              ↓
[SHUTDOWN] <--失敗-- [EMERGENCY] <---- [CRITICAL]
```

💻 編譯與執行
環境需求

Ubuntu 22.04 LTS
GCC 11.4.0 或更高版本
Git 2.34.1 或更高版本

編譯指令

```bash
git clone https://github.com/Sonny092910847/openbmc-training.git
cd openbmc-training
```

# 編譯單一程式

```bash
cd week1/state-machine
gcc -Wall -Wextra -g -o fan_control_state_machine fan_control_state_machine.c
./fan_control_state_machine
```

# 使用更嚴格的編譯選項

```bash
gcc -Wall -Wextra -Werror -std=c99 -pedantic -g -o output input.c
```

記憶體檢查
# 使用 valgrind 檢查記憶體洩漏
```bash
valgrind --leak-check=full ./advanced_pointers
```

📈 學習成果

✅ 掌握 C 語言記憶體管理的細節
✅ 理解函數指標在嵌入式系統中的應用
✅ 實作事件驅動的系統架構
✅ 遵循業界編碼標準

實際應用
這些技能直接對應到 OpenBMC 開發中的：

D-Bus 事件處理（回調機制）
硬體狀態管理（狀態機）
感測器數據處理（MISRA-C 規範）
系統資源管理（記憶體管理）

📚 學習資源

OpenBMC 官方文檔
MISRA-C 編碼指南
Linux Kernel Coding Style

🗓️ 未來計畫
2: Linux 核心與硬體介面

 I2C/SPI 驅動程式開發
 Device Tree 配置
 核心模組開發

3: D-Bus 與 OpenBMC 架構

 D-Bus 服務開發
 Phosphor 框架整合
 systemd 服務管理

4: Redfish API 與系統整合

 RESTful API 設計
 BMC Web 服務開發
 完整系統測試

🤝 聯絡方式

GitHub: Sonny092910847
Email: jasongod5737@gmail.com

📄 授權
本專案採用 MIT License - 詳見 LICENSE 檔案
