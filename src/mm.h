// memory allocation for ref-based objects.

static struct TYPE *ARRAY = 0;
static unsigned int allocated = 0,
                    next_index = 0;

struct free_entry {
	int id;
	struct free_entry *next;
};
static struct free_entry *free_list = 0;

static void put_on_free_list(int id) {
	struct free_entry *old = free_list;
	free_list = (struct free_entry*)malloc(sizeof(struct free_entry));
	free_list->next = old;
	free_list->id = id;
}

static int get_from_free_list() {
	if (!free_list) return -1;
	int ret = free_list->id;
	struct free_entry *old = free_list;
	free_list = free_list->next;
	free(old);
	return ret;
}

static bool element_available_on_free_list() { return free_list != 0; }

static REF allocate_ref() {
	REF ref;
	if (element_available_on_free_list())
		ref.id = get_from_free_list();
	else {
		if (next_index >= allocated) {
			struct TYPE *old_array = ARRAY;
			unsigned int allocate = 1.5 * (allocated + 1);
			ARRAY = (struct TYPE*)malloc(sizeof(struct TYPE) * allocate);
			for (int i = 0; i < allocated; ++i)
				ARRAY[i] = old_array[i];
			allocated = allocate;
			free(old_array);
		}
		ref.id = next_index++;
	}
	return ref;
}

static bool valid_ref(REF ref) {
	return ref.id >= 0;
}

static void free_ref(REF ref) {
	if (!valid_ref(ref)) return;
	put_on_free_list(ref.id);
}


