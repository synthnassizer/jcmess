project(jcmess)

add_executable(jcmess
	jcmess.h
	jcmess.cpp
	jcmess_main.cpp
)

target_link_libraries(jcmess
	PRIVATE
		asound
		jack
)
