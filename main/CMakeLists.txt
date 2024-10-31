idf_component_register(SRCS "main.cpp" "src/nvs_handle.c" "src/http_server.cpp" "src/wifi_handler.c" "src/gpio_handler.c" "src/WebSocket.c" "src/vl53l0x_handler.cpp" "pololu_vl53l0x/i2c.cpp" "pololu_vl53l0x/vl53l0x.cpp"

                    INCLUDE_DIRS "." "./lib" "./pololu_vl53l0x"
                    EMBED_FILES "webserver/index.html"
                                "webserver/style.css"
                                "webserver/script.js")
