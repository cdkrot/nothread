add_executable(contexts            contexts.cpp)
add_executable(contexts-pass-value contexts-pass-value.cpp)
add_executable(coroutine           coroutine.cpp)

target_link_libraries(contexts            nothread)
target_link_libraries(contexts-pass-value nothread)
target_link_libraries(coroutine           nothread)
