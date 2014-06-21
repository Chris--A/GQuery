
/* *********************************************************************************************
	Copyright (c) 2011 2012 Christopher Andrews.  All right reserved.

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
********************************************************************************************* */
/***********************************************************************************************
	MPL.h
***********************************************************************************************/

#include <Arduino.h>

#ifndef HEADER_MPL

	#define HEADER_MPL
	#define _INLINE_		__attribute__( ( always_inline ) ) inline
	#define _NO_INLINE_		__attribute__( ( noinline ) )	
	
	#define IsPowerOfTwo( value )	( !( value & ( value - 1 ) ) && value )	

	#ifndef GCONST
		#if __cplusplus > 199711L
			#define GCONST constexpr
		#else
			#define GCONST
		#endif
	#endif
	
	namespace GQ{
		
		#include "utility/CustomLoki.h"
		//#include "Loki.h"

		/***********************************************************************************************
			CompileTimeError & Macro.
				Useful for generating custom compile time errors.
				From Loki.
		***********************************************************************************************/
		
		template< bool > struct CompileTimeError;
		template<> struct CompileTimeError< true >{};	
		#define ErrorObj( error_check, message ) GQ::CompileTimeError<!!( error_check )> ERROR_##message
		//#define static_assert( cond, msg ) { GQ::CompileTimeError<!!(cond)> ERROR_##msg; (void) ERROR_##msg; }
	
		/***********************************************************************************************
			EnableIf structure.
				Useful in situaions requiring SFINAE.
		***********************************************************************************************/
		
		template< bool V, typename T > struct EnableIf{};
		template< typename T > struct EnableIf< true, T >{ typedef T Result; };

		/***********************************************************************************************
			Transform function.
				Safely cast between types without breaking strict aliasing rules.
		***********************************************************************************************/
					
		template< typename U, typename T >
			U Transform( T t )
				{
					union{ T _t; U _u; } _t = { t };
					return _t._u;
				}		
		
		
		/***********************************************************************************************
			IsGreaterThan interface.
				Use in non-typed template parameters where '>' operator is not allowed.
				
				Example usage: 	
					IsGreaterThan< int, -1, 1 >::Value == false;
					IsGreaterThan< int, 70, sizeof( char ) >::Value == true;
		***********************************************************************************************/
					
		template< typename _Type, _Type _AVar, _Type _BVar, bool b_OrEqual = false > 
			struct IsGreaterThan{ static const bool Value = b_OrEqual ? _AVar >= _BVar : _AVar > _BVar; };	
		
		
		/***********************************************************************************************
			BestFitInt interface.
				'BestFitInt< 0xff, true >::Result' is equivalent to type 'unsigned char'
				'BestFitInt< 0xff, false >::Result' is equivalent to type 'signed int'
		***********************************************************************************************/
						
		template< uintmax_t _MaxPositiveNumber, bool _Unsigned = true >
			class BestFitInt{
				protected:
				
					enum{
						T64 = _MaxPositiveNumber >> ( _Unsigned ? 0x20 : 0x1F ),
						T32 = _MaxPositiveNumber >> ( _Unsigned ? 0x10 : 0x0F ),
						T16 = _MaxPositiveNumber >> ( _Unsigned ? 0x08 : 0x07 )
					};
					
					typedef typename Select< _Unsigned, uint8_t, int8_t >::Result	Type8;
					typedef typename Select< _Unsigned, uint16_t, int16_t >::Result	Type16;
					typedef typename Select< _Unsigned, uint32_t, int32_t >::Result	Type32;
					typedef typename Select< _Unsigned, uint64_t, int64_t >::Result	Type64;
					typedef typename Select< T16, Type16, Type8 >::Result			TypeB;
					typedef typename Select< T32, Type32, TypeB >::Result			TypeA;
				private:
				public:
					typedef typename Select< T64, Type64, TypeA >::Result 			Result;
		};
		
		//template< uintmax_t _MaxPositiveNumber, bool _Unsigned /*= true*/ >
		//	class BestFitInt		
			
			
		/* *************************************************************************************
			DivRem class.
				Compile-time modulus and 'modulus adjusted division'.
				An error is generated when '_Divisor = 0'.
				
				E.g. 
				-	DivRem< 14 >::Result 	= 2
				-	DivRem< 14 >::Remainder = 6	
		************************************************************************************* */
	  
		template< const unsigned long  _Dividend, const unsigned long _Divisor = 0x8 > 
			class DivRem{ 
			public:
				enum{ 
					Remainder  	= _Dividend % _Divisor,
					Result 		= ( _Dividend / _Divisor ) + ( ( Remainder > 0 ) ? 1 : 0 )
				};
			protected:
			private:
				ErrorObj( _Divisor != 0, _Divide_by_zero );			
		};
		
		
		/* *************************************************************************************
			BitsIn2N class.
				BitsIn2N< _Size >::Value returns the number of bits needed to store _Size.
				_Size must be in the form 2^n and a compile time error will be generated 
				on bad inputs.
		************************************************************************************* */
		
		template< int _Size > class BitsIn2N{ 
			public:
				enum{ Value = 0x01 + BitsIn2N< _Size / 0x02 >::Value };
			protected:
			private:
				ErrorObj( IsPowerOfTwo( _Size ), _Size_is_not_a_power_of_two );
		};
		template<> struct BitsIn2N< 0x01 >{ enum{ Value, }; };	
		
		
		/***********************************************************************************************
			MPLArray class.
		***********************************************************************************************/
						
		template< typename _Type, uint64_t _Len > 
			struct MPLArray{
			
				enum{ 
					Length 	= _Len,
					Size 	= sizeof( _Type ) * _Len,
				};
				
				_Type& operator[]( typename BestFitInt< _Len >::Result i_Index ) 	{ return this->t_Data[ i_Index ]; }
				operator _Type*()													{ return this->t_Data; }
				operator const _Type*() const 										{ return this->t_Data; }
				
				_Type t_Data[ Length ];
		};	
			
			
		/***********************************************************************************************
			SwapValues function.
		***********************************************************************************************/
					
		template< typename T >
			inline void SwapValues( T &t_A, T &t_B )
				{
					T t_C = t_A;
					t_A = t_B;
					t_B = t_C;
				}
				
		//Specialisation for pointer types, they are dereferenced before swapping.
		//template< typename T > inline void SwapValues( T *t_A, T *t_B ) { SwapValues( *t_A, *t_B ); }	
		
		/***********************************************************************************************
			CompareType function.
				Returns true if 'a' is the same type as 'b'
				
				E.g.
					CompareType( 1, 2.0f ); //false
					CompareType( 'c', 'g' ); //true				
		***********************************************************************************************/
					
		template< typename _A, typename _B > 
			GCONST inline const bool CompareType( _A a, _B b )
				{ return false; }

		template< typename _A > 
			GCONST inline const bool CompareType( _A a, _A b )
				{ return true; }		


		/***********************************************************************************************
			IsSameType structure.
				This is the same as CompareType(), but allows using the value in constant expressions.
		***********************************************************************************************/
			
		//template < typename T, typename U > struct IsSameType{ enum { Value = false }; };
		//template < typename T > struct IsSameType< T, T > { enum { Value = true }; };
			
			
		/***********************************************************************************************
			IsIntType function.
				Will return true if t is a integer type ( 8 to 64 bit, signed / unsigned ).			
		***********************************************************************************************/
					
		template< typename T > 
			GCONST inline const bool IsIntType( T t )
				{
					return	CompareType( t, uint8_t( 0x00 ) ) || CompareType( t, int8_t( 0x00 ) ) ||
							CompareType( t, uint16_t( 0x00 ) ) || CompareType( t, int16_t( 0x00 ) ) ||
							CompareType( t, uint32_t( 0x00 ) ) || CompareType( t, int32_t( 0x00 ) ) ||
							CompareType( t, uint64_t( 0x00 ) ) || CompareType( t, int64_t( 0x00 ) );					
				}		
				
		/***********************************************************************************************
			IsInt structure.
				'Value' will be set to true if t is a integer type ( 8 to 64 bit, signed / unsigned ).
				This is the compile time constant version of IsIntType(). 
		***********************************************************************************************/
			
			
		template< typename T >
			struct IsInt{
				enum{
				
					V8 = IsSameType< T, uint8_t >::Value || IsSameType< T, int8_t >::Value,
					V16 = IsSameType< T, uint16_t >::Value || IsSameType< T, int16_t >::Value,
					V32 = IsSameType< T, uint32_t >::Value || IsSameType< T, int32_t >::Value,
					V64 = IsSameType< T, uint64_t >::Value || IsSameType< T, int64_t >::Value,
					Value = V8 || V16 || V32 || V64
				};
		};
		
		template< typename T > struct IsFloat{ enum{ Value = IsSameType< T, float >::Value || IsSameType< T, double >::Value }; };
		
		template< typename T > struct IsNumber{ enum{ Value = IsFloat< T >::Value || IsInt< T >::Value }; };
		
		template< typename T > struct IsCharType{ enum{ Value = IsSameType< T, uint8_t >::Value || IsSameType< T, int8_t >::Value }; };
		
		template< typename T > struct IsConst{ enum{ Value = false, }; };
		template< typename T > struct IsConst< const T >{ enum{ Value = true }; };
		
		template< typename T > struct IsVolatile{ enum{ Value = false, }; };
		template< typename T > struct IsVolatile< volatile T >{ enum{ Value = true }; };
		
		
		/* *************************************************************************************
			IsArrayType structure.
				Useful to disambiguate T *t, and T(&t)[N] situations.
				Switch use over to ArrayInfo as it exposes more information.
		************************************************************************************* */
					
		template< typename T > struct IsArrayType{ enum { Value = false }; };
		template< typename T, size_t N > struct IsArrayType< T[N] > { enum { Value = true }; };			
		template< typename T, size_t A, size_t N > struct IsArrayType< T[A][N] > { enum { Value = true }; };
		template< typename T, size_t B, size_t A, size_t N > struct IsArrayType< T[B][A][N] > { enum { Value = true }; };
		
		
		/* *************************************************************************************
			IsPointerType structure.
				Useful to disambiguate T *t, and T(&t)[N] situations.
				
				Deprecated while using Loki::TypeTraits
		************************************************************************************* */
		/*
		template< typename T > struct IsPointerType{ enum { Value = false }; };
		template< typename T > struct IsPointerType< T* >{ enum { Value = true }; };	
		template< typename T > struct IsPointerType< T*& >{ enum { Value = true }; };
		*/
		/***********************************************************************************************
			GreaterSizeType structure.
				Returns _TypeA if sizeof _TypeA is >= sizeof _TypeB
				Otherwise _TypeB.
		***********************************************************************************************/
						
		template< typename _TypeA, typename _TypeB >
			struct GreaterSizeType{

				typedef typename Select<
							IsGreaterThan< size_t, sizeof( _TypeA ), sizeof( _TypeB ), true >::Value,
							_TypeA,
							_TypeB
						>::Result Result;
			};
			
			
			
			
			
		//template< typename T >
		//	struct 
			
			
			
			
			
			
			
			

		
		/* *************************************************************************************
			sizeofArray function.
				
				template< typename T, size_t N > 
					An alternative way to determine the length of an array.
					This can be considered a compile time constant when using C++11
					and paramaters passed to the function are constant.
					
				template< typename T, size_t A, size_t N >

				template< typename T, size_t B, size_t A, size_t N > 
					
		************************************************************************************* */
		
		template< typename T, size_t N > 
			GCONST inline const size_t sizeofArray( T (&)[N] ) { return N; }
			
		template< typename T, size_t A, size_t N > 
			GCONST inline const size_t sizeofArray( T (&)[A][N], const char item = 0x01 ) { return item ? A : N; }			
			
		template< typename T, size_t B, size_t A, size_t N > 
			GCONST inline const size_t sizeofArray( T (&)[B][A][N], const char item = 0x01 ) { return item ? ( item == 1 ? A : B ) : N; }	
			
		/* *************************************************************************************
			ArrayInfo structure.
				A useful way to disambiguate multi dimension array's. 
				Also T *t, and T(&t)[N] situations can be solved using IsAnArray value.
		************************************************************************************* */
		
		template< typename T > 							/*--- 'Not an array' specialisation. ---*/
			struct ArrayInfo{ 
				enum{
					IsAnArray = false,
					First = 0x00,
					Dimensions = 0x00,
				};

				typedef T			ThisType;
				typedef NullType	NextType;
				typedef ThisType	UnderlyingType;
		};
		
		template< typename T, size_t N > 
			struct ArrayInfo< T[N] >{ 					/*--- Single dimension specialisation. ---*/
				enum{
					IsAnArray = true,
					First = N,
					Dimensions = 0x01,
				};
				static const size_t Indicies[ Dimensions ];
				typedef T ThisType[N];
				typedef T NextType;
				typedef T UnderlyingType;
		}; 
			
		template< typename T, size_t A, size_t N > 
			struct ArrayInfo< T[A][N] >{				/*--- Two dimensional array specialisation. ---*/
				enum{
					IsAnArray = true,
					First = A,
					Second = N,
					Dimensions = 0x02,
				};
				static const size_t Indicies[ Dimensions ];
				typedef T ThisType[A][N];
				typedef T NextType[N];
				typedef T UnderlyingType;			
		}; 	

		template< typename T, size_t B, size_t A, size_t N > 
			struct ArrayInfo< T[B][A][N] >{ 			/*--- Three dimensional array specialisation. ---*/
				enum{
					IsAnArray = true,
					First = B,
					Second = A,
					Third = N,
					Dimensions = 0x03,
				};
				static const size_t Indicies[ Dimensions ];
				typedef T ThisType[B][A][N];
				typedef T NextType[A][N];
				typedef T UnderlyingType;
		};	

		template< typename T, size_t C, size_t B, size_t A, size_t N > 
			struct ArrayInfo< T[C][B][A][N] >{			/*--- Four dimensional array specialisation. ---*/
				enum{
					IsAnArray = true,
					First = C,
					Second = B,
					Third = A,
					Fourth = N,
					Dimensions = 0x04,
				};
				static const size_t Indicies[ Dimensions ];
				typedef T ThisType[C][B][A][N];
				typedef T NextType[B][A][N];
				typedef T UnderlyingType;
		};		
		/*
		template< typename T >
			const size_t ArrayInfo< T >::Indicies[ ArrayInfo< T >::Dimensions ] = { 
				ArrayInfo< T >::First
		};*/
		
		template< typename T, size_t N >
			const size_t ArrayInfo< T[N] >::Indicies[ ArrayInfo< T[N] >::Dimensions ] = { 
				ArrayInfo< T[N] >::First
		};
		
		template< typename T, size_t A, size_t N >
			const size_t ArrayInfo< T[A][N] >::Indicies[ ArrayInfo< T[A][N] >::Dimensions ] = { 
				ArrayInfo< T[A][N] >::First, 
				ArrayInfo< T[A][N] >::Second
		};
		
		template< typename T, size_t B, size_t A, size_t N >
			const size_t ArrayInfo< T[B][A][N] >::Indicies[ ArrayInfo< T[B][A][N] >::Dimensions ] = { 
				ArrayInfo< T[B][A][N] >::First, 
				ArrayInfo< T[B][A][N] >::Second, 
				ArrayInfo< T[B][A][N] >::Third
		};
		
		template< typename T, size_t C, size_t B, size_t A, size_t N >
			const size_t ArrayInfo< T[C][B][A][N] >::Indicies[ ArrayInfo< T[C][B][A][N] >::Dimensions ] = { 
				ArrayInfo< T[C][B][A][N] >::First, 
				ArrayInfo< T[C][B][A][N] >::Second, 
				ArrayInfo< T[C][B][A][N] >::Third,
				ArrayInfo< T[C][B][A][N] >::Fourth,
		};	
		
		
		
		/*
		template< typename T, typename U >
			struct ArraysEqual{
		
				typedef ArrayInfo< T > AT;
				typedef ArrayInfo< U > AU;
				
				enum{
					EqualDimensions = AT::Dimensions == AU::Dimensions,
					
					
		};*/

		
		template< typename T >
			struct PointerInfo{
				enum{ 
					IsAPointer = false,
					References = 0
				};
				typedef T			ThisType;
				typedef NullType	NextType;				
				typedef T 			UnderlyingType;			
			};
			
		template< typename T >
			struct PointerInfo< T* >{
				enum{ 
					IsAPointer = true,
					References = 1
				};
				typedef T*		ThisType;
				typedef T		NextType;			
				typedef T		UnderlyingType;			
			};
			
		template< typename T >
			struct PointerInfo< T** >{
				enum{ 
					IsAPointer = true,
					References = 2
				};
				typedef T**		ThisType;
				typedef T*		NextType;			
				typedef T		UnderlyingType;			
			};

		template< typename T >
			struct PointerInfo< T*** >{
				enum{ 
					IsAPointer = true,
					References = 3
				};
				typedef T***	ThisType;
				typedef T**		NextType;
				typedef T		UnderlyingType;
			};

		template< typename T >
			struct PointerInfo< T**** >{
				enum{ 
					IsAPointer = true,
					References = 4
				};
				typedef T****	ThisType;
				typedef T***	NextType;
				typedef T		UnderlyingType;
			};		

		/*---
			PointerToMeta
			Represent a pointer type as an array with meta data. Or an array of pointers
			as a multidimensional array.
			
			This is designed to fail for any type that is not a pointer/array of pointers.
		---*/
		
		template< typename T, size_t N > struct PointerToMeta;
			
		template< typename T, size_t N >
			struct PointerToMeta< T*, N >{ typedef T Result[ N ]; };
			
		template< typename T, size_t N, size_t A >
			struct PointerToMeta< T*[ N ], A >{ typedef T Result[ A ][ N ]; };
			
		template< typename T, size_t A, size_t N, size_t B >
			struct PointerToMeta< T*[ A ][ N ], B >{ typedef T Result[ B ][ A ][ N ]; };
			
		template< typename T, size_t B, size_t A, size_t N, size_t C >
			struct PointerToMeta< T*[ B ][ A ][ N ], C >{ typedef T Result[ C ][ B ][ A ][ N ]; };
			
			
		/*---
			Re-type an array while preserving the indicies.
		---*/
		
		template< typename T, typename U > 
			struct ReplaceArrayBaseType{ typedef U Result; };
			
		template< typename T, size_t N, typename U > 
			struct ReplaceArrayBaseType< T[ N ], U >{ typedef U Result[ N ]; };
			
		template< typename T, size_t A, size_t N, typename U > 
			struct ReplaceArrayBaseType< T[ A ][ N ], U >{ typedef U Result[ A ][ N ]; };
			
		template< typename T, size_t B, size_t A, size_t N, typename U > 
			struct ReplaceArrayBaseType< T[ B ][ A ][ N ], U >{ typedef U Result[ B ][ A ][ N ]; };
			
		template< typename T, size_t C, size_t B, size_t A, size_t N, typename U > 
			struct ReplaceArrayBaseType< T[ C ][ B ][ A ][ N ], U >{ typedef U Result[ C ][ B ][ A ][ N ]; };
			
		/*---
			Similar to the array version above, this will re-type a pointer.
		---*/
			
		template< typename T, typename U > 
			struct ReplacePointerBaseType{ typedef U Result; };
			
		template< typename T, typename U > 
			struct ReplacePointerBaseType< T*, U >{ typedef U* Result; };
			
		template< typename T, typename U > 
			struct ReplacePointerBaseType< T**, U >{ typedef U** Result; };	
			
		template< typename T, typename U > 
			struct ReplacePointerBaseType< T***, U >{ typedef U*** Result; };	
			
		template< typename T, typename U > 
			struct ReplacePointerBaseType< T****, U >{ typedef U**** Result; };				

			
		/* *************************************************************************************
			Compile time counter. 
				Restricted by size of unsigned int.
			
				AVR8
					Max 16 unique numbers: {0, 15}
					
			REFERENCES
			
			Original can be found here:
			http://stackoverflow.com/questions/6166337/does-c-support-compile-time-counters
		************************************************************************************* */
		
		template< size_t _Size >								// This type returns a number through function lookup.
			struct cn{											// The function returns cn<n>.
				char data[ _Size + 1 ];
		};														// The caller uses (sizeof fn() - 1).

		template< typename _Id, size_t _Size, size_t _Acc >
			cn< _Acc > seen( _Id, cn< _Size >, cn< _Acc > );	// Default fallback case.

		/**
			Evaluate the counter by finding the last defined overload.
			Each function, when defined, alters the lookup sequence for 
			lower-order functions.
		*/
		
		#define counter_read( id ) \
			( sizeof seen( id(), cn< 1 >(), cn< \
			( sizeof seen( id(), cn< 2 >(), cn< \
			( sizeof seen( id(), cn< 4 >(), cn< \
			( sizeof seen( id(), cn< 8 >(), cn< \
			( sizeof seen( id(), cn< 16 >(), cn< \
			( sizeof seen( id(), cn< 32 >(), cn< \
			( sizeof seen( id(), cn< 64 >(), cn< \
			( sizeof seen( id(), cn< 128 >(), cn< \
			( sizeof seen( id(), cn< 256 >(), cn< \
			( sizeof seen( id(), cn< 512 >(), cn< \
			( sizeof seen( id(), cn< 1024 >(), cn< \
			( sizeof seen( id(), cn< 2048 >(), cn< \
			( sizeof seen( id(), cn< 4096 >(), cn< \
			( sizeof seen( id(), cn< 8192 >(), cn< \
			( sizeof seen( id(), cn< 16384 >(), cn< 0 \
			>() ) - 1 ) \
			>() ) - 1 ) \
			>() ) - 1 ) \
			>() ) - 1 ) \
			>() ) - 1 ) \
			>() ) - 1 ) \
			>() ) - 1 ) \
			>() ) - 1 ) \
			>() ) - 1 ) \
			>() ) - 1 ) \
			>() ) - 1 ) \
			>() ) - 1 ) \
			>() ) - 1 ) \
			>() ) - 1 ) \
			>() ) - 1 )

		/* 
			Define a single new function with place-value equal to the bit 
			flipped to 1 by the increment operation.
			This is the lowest-magnitude function yet undefined in the 
			current context of defined higher-magnitude functions. 
		*/
		#define counter_inc( id ) \
			cn< counter_read( id ) + 1 > \
				seen( id, cn< ( counter_read( id ) + 1 ) & ~ counter_read( id ) >, \
					  cn< ( counter_read( id ) + 1 ) & counter_read( id ) > )
					  
					  
		#define TokenMove	counter_inc
		#define TokenValue	counter_read
		#define TOKEN		struct			
	};
#endif

//EOF
	