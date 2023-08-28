//問題二
static inline int my_spin_lock(atomic_int*lock){
	int val=0;															//val預設為0
	if(likely(atomic_exchange_explicit(lock1,memory_order_acq_rel)==0))	//將lock設為1,並回傳原本lock的值,同時設定memory order
		return 0;														//lock為0時結束（上鎖成功）		
	do{
		do{
			asm("pause");												//使用pause,避免busy waiting
		}while(*lock !=0);												//直到unlock
		val=0;															//判斷val是否為0,若不為0就繼續執行迴圈等待,並同時設定memory order; 若val為0則lock=1上鎖
	}while(!atomic_compare_exchange_weak_explicit(lock, &val,1,memory_order_acq_rel,memory_order_relaxed));
	return 0;															//lock成功回傳
}
static inline int my_spin_unlock(atomic_int*lock){
	atomic_store_explicit(lock,0,memory_order_release);					//lock=0解鎖,release memory order
	return 0;															//unlock成功回傳
}
