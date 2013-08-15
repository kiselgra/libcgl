divert(-1)
define(`define_mm',` dnl)
	divert(2)
	#include <assert.h>
	static struct $1 *$2 = 0;
	static unsigned int $1s_allocated = 0,
	                    next_$1_index = 0;
	
	struct free_$1_entry {
		int id;
		struct free_$1_entry *next;
	};
	static struct free_$1_entry *free_$1_list = 0;
	
	static void put_$1_on_free_list(int id) {
		struct free_$1_entry *old = free_$1_list;
		free_$1_list = (struct free_$1_entry*)malloc(sizeof(struct free_$1_entry));
		free_$1_list->next = old;
		free_$1_list->id = id;
	}
	
	static int get_$1_from_free_list() {
		if (!free_$1_list) return -1;
		int ret = free_$1_list->id;
		struct free_$1_entry *old = free_$1_list;
		free_$1_list = free_$1_list->next;
		free(old);
		return ret;
	}
	
	static bool $1_element_available_on_free_list() { return free_$1_list != 0; }
	
	static $3 allocate_$1_ref() {
		$3 ref;
		if ($1_element_available_on_free_list())
			ref.id = get_$1_from_free_list();
		else {
			if (next_$1_index >= $1s_allocated) {
				struct $1 *old_array = $2;
				unsigned int allocate = 1.5 * ($1s_allocated + 1);
				$2 = (struct $1*)malloc(sizeof(struct $1) * allocate);
				for (int i = 0; i < $1s_allocated; ++i)
					$2[i] = old_array[i];
				$1s_allocated = allocate;
				free(old_array);
			}
			ref.id = next_$1_index++;
		}
		return ref;
	}
	
	bool valid_$1_ref($3 ref) {
		return ref.id >= 0;
	}
	
	static void free_$1_ref($3 ref) {
		if (!valid_$1_ref(ref)) return;
		put_$1_on_free_list(ref.id);
	}

	struct $1_array make_$1_array(void) {
		struct $1_array a;
		a.capacity = 4;
		a.size = 0;
		a.element = ($3*)malloc(sizeof($3)*a.capacity);
		return a;
	}

	void push_$1_to_array($3 ref, struct $1_array *array) {
		if (array->size == array->capacity) {
			array->capacity *= 1.5;
			array->element = ($3*)realloc(array->element, sizeof($3)*array->capacity);
		}
		array->element[array->size++] = ref;
	}

	$3 pop_$1_from_array(struct $1_array *array) {
		assert(array->size > 0);
		return array->element[--array->size];
	}

	void push_$1_list_to_array(struct $1_list *list, struct $1_array *array) {
		for (struct $1_list *run = list; run; run = run->next)
			push_$1_to_array(run->ref, array);
	}
	divert(-1)
	')

undivert(2)
dnl
dnl vim: syntax=c
