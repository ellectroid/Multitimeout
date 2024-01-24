#ifndef Multitimeout_H_
#define Multitimeout_H_

#include <thread>
#include <mutex>
#include <semaphore>
#include <chrono>

class Multitimeout{
public:
	typedef std::chrono::steady_clock::time_point timepoint_t;
	typedef std::chrono::duration<int, std::milli> ms_t;
	typedef void (*timeout_handler_func_t)(Multitimeout* Multitimeout, int timer_id); //void func(Multitimeout* this, int timer_id)
private:
	//threads
	std::thread timeout_thread_;
	std::mutex object_access_mutex_;
	std::counting_semaphore<16> timeout_sem_;
	ms_t sleep_time_;
	timepoint_t reference_timepoint_;
	volatile bool thread_ready_;
	int thread_signal_;
	timeout_handler_func_t timeout_event_handler_;

	ms_t timeout_[16];
	ms_t timeout_reload_[16];
	bool timeout_active_[16];
	bool timeout_expired_[16];
	bool timeout_repeat_[16];
	bool timeout_update_skip_[16];
	int timeout_count_;
	int timeout_active_count_;
	int timeout_expired_counter_;


private:
	void timeout_thread_func();
private:
	int timeout_update_active_counters();
	int update_sleep_time();
	int expired_timeout_handler();
public:
	Multitimeout();
	int add(int timer_id, ms_t timeout, bool repeat);
	int remove(int timer_id);
	int bind_event_handler(timeout_handler_func_t handlerfunc);
	int unbind_event_handler();
};




#endif /* Multitimeout_H_ */
