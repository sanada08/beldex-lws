set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(nlohmann EXCLUDE_FROM_ALL)
add_subdirectory(bmq)
add_subdirectory(db_drivers)
