
#define ARRAY_SIZE 5

void nvs_init();
void store_array(int32_t *array);
void clear_nvs();
void load_array(int32_t *array);
void update_array_element(int, int32_t);
void add_new_element(int32_t);
