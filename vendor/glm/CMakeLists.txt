file(GLOB ROOT_INLINE *.inl)
file(GLOB ROOT_HEADER *.hpp)
file(GLOB ROOT_TEXT ../*.txt)
file(GLOB ROOT_MD ../*.md)
file(GLOB ROOT_NAT ../util/glm.natvis)

file(GLOB_RECURSE CORE_INLINE ./detail/*.inl)
file(GLOB_RECURSE CORE_HEADER ./detail/*.hpp)

file(GLOB_RECURSE EXT_INLINE ./ext/*.inl)
file(GLOB_RECURSE EXT_HEADER ./ext/*.hpp)

file(GLOB_RECURSE GTC_INLINE ./gtc/*.inl)
file(GLOB_RECURSE GTC_HEADER ./gtc/*.hpp)

file(GLOB_RECURSE GTX_INLINE ./gtx/*.inl)
file(GLOB_RECURSE GTX_HEADER ./gtx/*.hpp)

file(GLOB_RECURSE SIMD_INLINE ./simd/*.inl)
file(GLOB_RECURSE SIMD_HEADER ./simd/*.h)

source_group("Text Files" FILES ${ROOT_TEXT} ${ROOT_MD})

source_group("Core Files" FILES ${CORE_INLINE})
source_group("Core Files" FILES ${CORE_HEADER})
source_group("EXT Files" FILES ${EXT_INLINE})
source_group("EXT Files" FILES ${EXT_HEADER})
source_group("GTC Files" FILES ${GTC_INLINE})
source_group("GTC Files" FILES ${GTC_HEADER})
source_group("GTX Files" FILES ${GTX_INLINE})
source_group("GTX Files" FILES ${GTX_HEADER})
source_group("SIMD Files" FILES ${SIMD_INLINE})
source_group("SIMD Files" FILES ${SIMD_HEADER})

add_library(glm INTERFACE
		${ROOT_TEXT} ${ROOT_MD} ${ROOT_NAT}
		${ROOT_INLINE} ${ROOT_HEADER}
		${CORE_INLINE} ${CORE_HEADER}
		${EXT_INLINE} ${EXT_HEADER}
		${GTC_INLINE} ${GTC_HEADER}
		${GTX_INLINE} ${GTX_HEADER}
		${SIMD_INLINE} ${SIMD_HEADER}
		)

target_include_directories(glm
		INTERFACE
		"${CMAKE_CURRENT_SOURCE_DIR}/.."
		)

add_library(GLM::GLM ALIAS glm)
