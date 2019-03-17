struct lock_t {
	uint locked;
};

void lock_acquire(struct lock_t *);
void lock_release(struct lock_t *);
void lock_init(struct lock_t *);
int thread_create(void *(*start_routine)(void*), void *arg);

