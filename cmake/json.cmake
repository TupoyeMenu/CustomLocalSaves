include(FetchContent)

set(JSON_MultipleHeaders OFF)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)
message("json")
FetchContent_MakeAvailable(json)
