//問題一
int __pthread_spin_lock(pthread_spinlock_t *lock) {
	int val = 0;												//val預設為0
	if(__glibc_likely(atomic_exchange_acquire(lock,1) ==0))		//lock=1,並回傳lock原本的值
		return 0;												//若lock為0結束（上鎖成功）
	do{															//上鎖不成功,進入do while迴圈
		do{
		/* TODO Back-off.*/										//隨機等待一段時間,避免有人同時執行成功
		atomic_spin_nop();										//類似pause,避免busy waiting
		val=atomic_load_relaxed(lock);							//把lock的值讀到val
		}while(val!=0);											//val為0時表示unlock
	}while(!atomic_compare_exchange_weak_acquire(lock, &val,1));//判斷val值，若為0就把lock=1,沒成功則持續執行會圈等待
	return 0;													//lock成功回傳
}

int __pthread_spin_unlock(pthread_spinlock_t*lock) {
	atomic_store_release(lock,0);								//lock=0解鎖,並release資訊
	return 0;													//unlock成功回傳
}
