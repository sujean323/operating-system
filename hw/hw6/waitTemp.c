#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h> 
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>


int numCPU = -1;
int temp[8];

//很爛的寫法，應該寫成陣列
FILE* temp1, *temp2, *temp3, *temp4, *temp5, *temp6, *temp7;
//底下這程式碼會印出所有CPU的溫度
int printTemp(int try) {
    int eatReturn;
    int tempArray[8];
    //根據我以前的測試，這個檔案必須重新開啟作業系統才會更新裡面的數值
    //底下打開的檔案適用於AMD Threadripper 2990WX
    temp1 = fopen("/sys/class/hwmon/hwmon2/temp2_input", "r");
    assert(temp1 != NULL);
    temp2 = fopen("/sys/class/hwmon/hwmon2/temp3_input", "r");
    assert(temp2 != NULL);
    temp3 = fopen("/sys/class/hwmon/hwmon2/temp4_input", "r");
    assert(temp3 != NULL);
    temp4 = fopen("/sys/class/hwmon/hwmon2/temp5_input", "r");
    assert(temp4 != NULL);
    temp5 = fopen("/sys/class/hwmon/hwmon2/temp6_input", "r");
    assert(temp5 != NULL);
    temp6 = fopen("/sys/class/hwmon/hwmon2/temp7_input", "r");
    assert(temp6 != NULL);
    temp7 = fopen("/sys/class/hwmon/hwmon2/temp8_input", "r");
    assert(temp7 != NULL);

    for (int i=0; i<4; i++)
        tempArray[i] = -1;
    fseek(temp1, 0, SEEK_SET);
    //eatReturn是明確的把「回傳值」吃掉，讓編譯器不要出現警告訊息
    eatReturn=fscanf(temp1, "%d", &tempArray[0]);
    fseek(temp2, 0, SEEK_SET);
    eatReturn=fscanf(temp2, "%d", &tempArray[1]);
    fseek(temp3, 0, SEEK_SET);
    eatReturn=fscanf(temp3, "%d", &tempArray[2]);
    fseek(temp4, 0, SEEK_SET);
    eatReturn=fscanf(temp4, "%d", &tempArray[3]);
    fseek(temp5, 0, SEEK_SET);
    eatReturn=fscanf(temp5, "%d", &tempArray[4]);
    fseek(temp6, 0, SEEK_SET);
    eatReturn=fscanf(temp6, "%d", &tempArray[5]);
    fseek(temp7, 0, SEEK_SET);
    eatReturn=fscanf(temp7, "%d", &tempArray[6]);
    
    //根據我以前的測試，這個檔案必須重新開啟作業系統才會更新裡面的數值
    //先關掉，等一下才可以重新開啟，或許可以用reopen
    fclose(temp1); fclose(temp2); fclose(temp3); fclose(temp4); fclose(temp5); fclose(temp6); fclose(temp7);
    int maxTemp = -1;
    for(int i=0; i<7; i++) {
	if(tempArray[i] > maxTemp) 
	    maxTemp = tempArray[i];
    	printf("CPU%02d: %02d sec 💥 🔥 💥 🔥  %.2f ℃\n", i+1, try, ((float)tempArray[i])/1000);
    }
 
    printf("-------------------------------------\n");
    return maxTemp;
}

int main(int argc, char **argv) {
    //預設值等到CPU的溫度降到35度，tempture是目標溫度
    int tempture=35; 
    //使用者可以在參數列輸入溫度
    if (argc == 2) {
        sscanf(argv[1], "%d", &tempture);
    }
    fprintf(stderr, "target = %d\n", tempture);
    //授課老師自己稍微看一下，這個溫度要乘上1000才是真正的溫度
    tempture = tempture * 1000;

    //一個迴圈，每隔一秒讀取CPU的溫度，直到溫度比設定的還要低
    int temp;
    int try=1;
    while((temp = printTemp(try)) > tempture) {
        try++;
        //不要跑一個空的迴圈，這樣溫度會不斷的往上升，因此呼叫sleep暫停這個應用程式一秒鐘
        sleep(1);
    }

    
    exit(0);
}
