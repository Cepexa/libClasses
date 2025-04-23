
file(GLOB_RECURSE SOURCE_FILES 
    src/*.cpp)
add_library(libClasses SHARED ${SOURCE_FILES})

# Собираем все подкаталоги include
###
file(GLOB_RECURSE PROJECT_INCLUDE_DIRS LIST_DIRECTORIES true "${CMAKE_CURRENT_SOURCE_DIR}/include/*")
# Фильтруем только директории, удаляя файлы
set(DIR_LIST "")
foreach(dir ${PROJECT_INCLUDE_DIRS})
    if (IS_DIRECTORY ${dir})
        list(APPEND DIR_LIST ${dir})
    endif()
endforeach()

target_include_directories(libClasses PUBLIC ${DIR_LIST})
###
target_include_directories(libClasses PUBLIC include)



