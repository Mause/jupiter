MACRO(LEMON SRC DST VAR)
    IF(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${SRC}.cpp AND NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${SRC}.h)
        IF(UNIX)
            SET(COPY_OR_LINK create_symlink)
        ELSE()
            SET(COPY_OR_LINK copy_if_different)
        ENDIF()

        # these commented out sections are only required if you wish
        # to compile lemon alongside jupiter
        # ADD_CUSTOM_COMMAND(
        #     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lempar.c
        #     COMMAND cmake -E ${COPY_OR_LINK} ${CMAKE_SOURCE_DIR}/lemon/lempar.c ${CMAKE_CURRENT_BINARY_DIR}/lempar.c
        #     MAIN_DEPENDENCY ${CMAKE_SOURCE_DIR}/lemon/lempar.c
        # )

        find_program(LEMON lemon)
        if (NOT LEMON)
            message(FATAL_ERROR "Could not find lemon; ${LEMON}")
        endif()

        get_filename_component(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/${DST} PATH)
        file(MAKE_DIRECTORY ${OUTPUT_DIR})

        ADD_CUSTOM_COMMAND(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${DST}.c ${CMAKE_CURRENT_BINARY_DIR}/${DST}.cpp ${CMAKE_CURRENT_BINARY_DIR}/${DST}.h
            COMMAND cmake -E ${COPY_OR_LINK} ${CMAKE_CURRENT_SOURCE_DIR}/${SRC}.lemon ${CMAKE_CURRENT_BINARY_DIR}/${DST}.lemon
            COMMAND cmake -E chdir ${CMAKE_CURRENT_BINARY_DIR} lemon -q ${DST}.lemon
            COMMAND cmake -E ${COPY_OR_LINK} ${CMAKE_CURRENT_BINARY_DIR}/${DST}.c ${CMAKE_CURRENT_BINARY_DIR}/${DST}.cpp
            MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/${SRC}.lemon
            # DEPENDS lemon ${CMAKE_CURRENT_BINARY_DIR}/lempar.c
        )
        SET(${VAR} ${${VAR}} ${CMAKE_CURRENT_BINARY_DIR}/${DST}.cpp)
    ELSE()
        SET(${VAR} ${${VAR}} ${CMAKE_CURRENT_SOURCE_DIR}/${SRC}.cpp ${CMAKE_CURRENT_SOURCE_DIR}/${SRC}.h)
    ENDIF()
ENDMACRO()
