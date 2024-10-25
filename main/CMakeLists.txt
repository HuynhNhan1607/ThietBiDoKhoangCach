idf_component_register(SRCS "main.c" "src/nvs_handle.c" "src/http_server.c" "src/wifi_handler.c" "src/gpio_handler.c"
                    INCLUDE_DIRS "." "./lib"
                    EMBED_FILES "webserver/index.html"
                                "webserver/style.css"
                                "webserver/script.js")
