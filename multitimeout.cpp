#include "Multitimeout.h"

Multitimeout::Multitimeout() :
	timeout_sem_(0) {
	sleep_time_ = std::chrono::milliseconds(0);
	reference_timepoint_ = std::chrono::steady_clock::now();
	timeout_count_ = 16;
	for (int i = 0; i < timeout_count_; i++) {
		timeout_[i] = std::chrono::milliseconds(0);
		timeout_reload_[i] = std::chrono::milliseconds(0);
		timeout_active_[i] = false;
		timeout_expired_[i] = false;
		timeout_update_skip_[i] = false;
	}
	timeout_active_count_ = 0;
	thread_ready_ = false;
	thread_signal_ = 0;
	timeout_expired_counter_ = 0;
	timeout_event_handler_ = nullptr;
	timeout_thread_ = std::thread(&Multitimeout::timeout_thread_func, this);
	timeout_thread_.detach();
	while (thread_ready_ != true);
}

void Multitimeout::timeout_thread_func() {
	thread_ready_ = true;
	while (1) {
		std::unique_lock<std::mutex> access_lock(object_access_mutex_);
		bool early_return = false;
		if (timeout_active_count_ == 0) {
			access_lock.unlock();
			timeout_sem_.acquire();
		}
		else {
			access_lock.unlock();
			early_return = timeout_sem_.try_acquire_for(sleep_time_);
		}
		(void)early_return;
		access_lock.lock();
		if (thread_signal_ == 1) {
			goto timeout_thread_out;
		}
		//std::cout << "timer thread woke up" << std::endl;
		timeout_update_active_counters();
		expired_timeout_handler();
		update_sleep_time();
	}
timeout_thread_out:
	thread_ready_ = false;
	thread_signal_ = 0;
	return;

}

int Multitimeout::timeout_update_active_counters() {
	int active_cnt = 0;
	timepoint_t current_moment = std::chrono::steady_clock::now();
	ms_t passed_time = std::chrono::duration_cast<ms_t>(
		current_moment - reference_timepoint_);

	for (int i = 0; i < timeout_count_; i++) {
		if (active_cnt == timeout_active_count_)
			break;
		if (timeout_active_[i]) {
			active_cnt++;
			if (!timeout_update_skip_[i]) {
				timeout_[i] -= passed_time;
				if (timeout_[i] <= std::chrono::milliseconds(0)) {
					timeout_expired_[i] = true;
					if (timeout_repeat_[i]) {
						timeout_[i] = timeout_reload_[i];
					}
					else {
						timeout_active_[i] = false;
					}
				} //if timeout counter < 0 end if
				else {
					timeout_expired_[i] = false;
				}
			} //if not skip end if
			else {
				timeout_update_skip_[i] = false;
			}
		}

	}
	reference_timepoint_ = current_moment;
	return active_cnt;
}

int Multitimeout::update_sleep_time() {
	if (timeout_active_count_ == 0)
		return 0;
	int active_cnt = 0;
	ms_t new_sleep_time = std::chrono::milliseconds(0);
	int active_timer_min_index = 0;
	for (int i = 0; i < timeout_count_; i++) {
		if (timeout_active_[i]) {
			active_timer_min_index = i;
			break;
		};
	}
	active_cnt++;
	for (int i = active_timer_min_index + 1; i < timeout_count_; i++) {
		if (active_cnt == timeout_active_count_)
			break;
		if (timeout_active_[i]) {
			active_cnt++;
			if (timeout_[active_timer_min_index] > timeout_[i]) {
				active_timer_min_index = i;
			};
		};
	}
	sleep_time_ = timeout_[active_timer_min_index];
	return 0;
}

int Multitimeout::expired_timeout_handler() {
	for (int i = 0; i < timeout_count_; i++) {
		if (timeout_expired_[i]) {
			timeout_expired_[i] = false;
			if (timeout_repeat_[i] == false) {
				timeout_active_[i] = false;
				timeout_active_count_--;
			}
			if (timeout_event_handler_ != nullptr) {
				timeout_event_handler_(this, i);
			}
			timeout_expired_counter_++;

		};
	}
	return 0;
}

int Multitimeout::add(int timer_id, ms_t timeout, bool repeat) {
	std::unique_lock<std::mutex> access_lock(object_access_mutex_);
	if ((timer_id < 0) || (timer_id >= timeout_count_)) return -1;
	if (timeout_active_[timer_id]) return -2;
	timeout_[timer_id] = timeout;
	timeout_reload_[timer_id] = timeout;
	timeout_repeat_[timer_id] = repeat;
	timeout_active_[timer_id] = true;
	timeout_update_skip_[timer_id] = true;
	timeout_active_count_++;
	if (timeout_active_count_ == 1) reference_timepoint_ = std::chrono::steady_clock::now();
	timeout_sem_.release();
	return timer_id;
}

int Multitimeout::remove(int timer_id) {
	std::unique_lock<std::mutex> access_lock(object_access_mutex_);
	timeout_active_[timer_id] = false;
	timeout_active_count_--;
	timeout_sem_.release();
	return timer_id;
}

int Multitimeout::bind_event_handler(timeout_handler_func_t handlerfunc) {
	std::unique_lock<std::mutex> access_lock(object_access_mutex_);
	timeout_event_handler_ = handlerfunc;
	return 0;
}
int Multitimeout::unbind_event_handler() {
	std::unique_lock<std::mutex> access_lock(object_access_mutex_);
	timeout_event_handler_ = nullptr;
	return 0;
}
