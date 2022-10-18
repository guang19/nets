FILE(GLOB HttpHeaders ${PROJECT_SOURCE_DIR}/nets/protocol/http/*.h)
INSTALL(FILES ${HttpHeaders} DESTINATION ${PROJECT_BINARY_DIR}/include/nets/protocol/http)

FILE(GLOB ProtocolSrcs ${PROJECT_SOURCE_DIR}/nets/protocol/http/*.cpp)

ADD_LIBRARY(nets_protocol STATIC ${ProtocolSrcs})

TARGET_LINK_LIBRARIES(nets_protocol nets_net)
INSTALL(TARGETS nets_protocol DESTINATION ${PROJECT_BINARY_DIR}/lib)

#IF (BUILD_NETS_TESTS)
#    ADD_SUBDIRECTORY(tests)
#ENDIF ()