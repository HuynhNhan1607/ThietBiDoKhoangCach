
#define ARRAY_SIZE 5

// Khởi tạo NVS
void nvs_init();              

// Tải dữ liệu từ NVS vào các mảng do người gọi cung cấp
// `data_array_out` là mảng `uint16_t` chứa dữ liệu, `time_array_out` là mảng `int64_t` chứa thời gian
void load_data(uint16_t data_array_out[ARRAY_SIZE], int64_t time_array_out[ARRAY_SIZE]);

// Thêm dữ liệu mới vào mảng và lưu lại vào NVS
// `new_data` là giá trị `uint16_t` cần thêm vào đầu mảng
void save_new_data(uint16_t new_data);

// Xóa toàn bộ dữ liệu trong NVS
void clear_data();

// Tạo JSON chứa mảng "data" và "date" từ dữ liệu trong NVS
// Trả về một chuỗi JSON, người gọi cần giải phóng bộ nhớ sau khi sử dụng
char* create_json_response();
