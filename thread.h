struct lock_t {
	uint locked;
};

struct seq_lock_t {
	uint locked;
	uint seq_num;
};

void lock_acquire(struct lock_t *);
void lock_release(struct lock_t *);
void lock_init(struct lock_t *);

void seq_lock_init(struct seq_lock_t *);
void seq_lock_acquire(struct seq_lock_t *);
void seq_lock_release(struct seq_lock_t *);
uint seq_lock_read(struct seq_lock_t *t);

int thread_create(void *(*start_routine)(void*), void *arg);

