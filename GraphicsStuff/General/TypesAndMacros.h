#pragma once
#include <stdint.h>


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

typedef size_t sizeT;

typedef bool b8;

namespace Internal {

	template<sizeT Size>
	struct EnumSizeUIntType
	{
		using type = u64;
	};
	template<>
	struct EnumSizeUIntType<4>
	{
		using type = u32;
	};
	template<>
	struct EnumSizeUIntType<2>
	{
		using type = u16;
	};
	template<>
	struct EnumSizeUIntType<1>
	{
		using type = u8;
	};

	template<typename T>
	struct EnumUIntType
	{
		using type = typename EnumSizeUIntType<sizeof(T)>::type;
	};
}

#define ENABLE_ENUM_FLAG_OPERATORS(Enum)\
	inline Enum operator~(Enum e) { using UnderlyingType = typename Internal::EnumUIntType<Enum>::type; return Enum(~UnderlyingType(e)); }\
	inline Enum operator|(Enum e0, Enum e1) { using UnderlyingType = typename Internal::EnumUIntType<Enum>::type; return Enum(UnderlyingType(e0) | UnderlyingType(e1)); }\
	inline Enum operator&(Enum e0, Enum e1) { using UnderlyingType = typename Internal::EnumUIntType<Enum>::type; return Enum(UnderlyingType(e0) & UnderlyingType(e1)); }\
	inline Enum operator^(Enum e0, Enum e1) { using UnderlyingType = typename Internal::EnumUIntType<Enum>::type; return Enum(UnderlyingType(e0) ^ UnderlyingType(e1)); }\
	inline Enum& operator|=(Enum& e0, Enum e1) { using UnderlyingType = typename Internal::EnumUIntType<Enum>::type; e0 = Enum(UnderlyingType(e0) | UnderlyingType(e1)); return e0; }\
	inline Enum& operator&=(Enum& e0, Enum e1) { using UnderlyingType = typename Internal::EnumUIntType<Enum>::type; e0 = Enum(UnderlyingType(e0) & UnderlyingType(e1)); return e0; }\
	inline Enum& operator^=(Enum& e0, Enum e1) { using UnderlyingType = typename Internal::EnumUIntType<Enum>::type; e0 = Enum(UnderlyingType(e0) ^ UnderlyingType(e1)); return e0; }

