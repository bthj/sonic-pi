MACRO(ENABLE_WARNINGS flag)
	ADD_C_FLAG_IF_SUPPORTED(-W${flag})
ENDMACRO()

MACRO(DISABLE_WARNINGS flag)
	ADD_C_FLAG_IF_SUPPORTED(-Wno-${flag})
ENDMACRO()

IF(ENABLE_WERROR)
	IF(MSVC)
		ADD_COMPILE_OPTIONS(-WX)
	ELSE()
		ADD_C_FLAG_IF_SUPPORTED(-Werror)
	ENDIF()
ENDIF()
