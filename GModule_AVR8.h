
/*---
	The MIT License (MIT)

	Copyright (c) 2014 Christopher Andrews, http://Arduino.land

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
---*/

#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#ifndef HEADER_GMODULE_AVR8
	#define HEADER_GMODULE_AVR8
	
	


	/*---
		The FlashObject type allows use of PROGMEM features found on AVR.
	---*/	
	
	template< typename T, bool _Far = false > 
		class FlashObject{
			public:
				FlashObject( T &t ) : t( t ) {}
				operator T(){ 
					if( sizeof( T ) == 0x01 ){
						//if( _Far )	return Transform< T >( pgm_read_byte_far( &this->t ) );
						//else 		return Transform< T >( pgm_read_byte_near( &this->t ) );
						return Transform< T >( pgm_read_byte_near( &this->t ) );
					}
					typename TypeTraits< T >::UnqualifiedType ret = T(); //Expecting T to be const.
					//( _Far ? memcpy_P : memcpy_P )( &ret, &this->t, sizeof( T ) );
					memcpy_P( &ret, &this->t, sizeof( T ) );
					return ret;
				}
				const bool operator ==( const T &ref ){ return ( *this ) == ref; }
			protected:
				T t;
			private:
	};
	
	template< typename T > 
		class EEPROMObject{
			public:
				EEPROMObject( T &t ) : t( t ) {}
				
				operator T(){ 
					if( sizeof( T ) == 0x01 ){
						return Transform< T >( eeprom_read_byte( &this->t ) );
					}else{
						typename TypeTraits< T >::UnqualifiedType ret = T(); //T may be const.
						char *c_Data = ( char* ) &ret;
						for( index_t( sizeof( T ) ) i = 0 ; i < sizeof( T ) ; *c_Data++ = eeprom_read_byte( &this->t + i++ ) );
						return ret;
					}
				}
				
				EEPROMObject &operator =( const T &t ){
					if( sizeof( T ) == 0x01 ){
						if( ( *this ) != t ) eeprom_write_byte( &this->t, ( byte ) t );
					}else{
						for( index_t( sizeof( T ) ) i = 0 ; i < sizeof( T ) ; i++ ){
							const char* mtp = ( ( char* ) this->t ) + i;
							const char tp = *( ( ( char* ) t ) + i );
							if( eeprom_read_byte( mtp ) != tp ) eeprom_write_byte( mtp, tp );
						}
					}
					return *this;
				}
				
				const bool operator ==( const T &ref ){ return ( *this ) == ref; }
			protected:
				T t;
			private:
	};
	
	
		
	
	template< typename T >
		struct QueryPGMType{
		
			typedef ArrayInfo< T >		Array;
			typedef PointerInfo< T > 	Pointer;

			typedef typename Select<
				Array::IsAnArray,
				typename ReplaceArrayBaseType< T, FlashObject< typename Array::UnderlyingType > >::Result,
				typename Select<
					Pointer::IsAPointer,
					typename ReplacePointerBaseType< T, FlashObject< typename Pointer::UnderlyingType > >::Result,
					FlashObject< T >
				>::Result
			>::Result PGMType;
			
			typedef typename QueryType< PGMType >::Result Result;
	};	
	
	template< typename T >
		struct QueryEEPROMType{
		
			typedef ArrayInfo< T >		Array;
			typedef PointerInfo< T > 	Pointer;

			typedef typename Select<
				Array::IsAnArray,
				typename ReplaceArrayBaseType< T, EEPROMObject< typename Array::UnderlyingType > >::Result,
				typename Select<
					Pointer::IsAPointer,
					typename ReplacePointerBaseType< T, EEPROMObject< typename Pointer::UnderlyingType > >::Result,
					EEPROMObject< T >
				>::Result
			>::Result EEPROMType;
			
			typedef typename QueryType< EEPROMType >::Result Result;
	};		
	
	
	
	/*
	template< typename T, typename U, template class Wrapper< U > >
		struct QueryWrappedType{
		
			typedef ArrayInfo< T >						Array;
			typedef PointerInfo< T > 					Pointer;
			typedef typename Select<
				Array::IsAnArray,
				typename ReplaceArrayBaseType< T, U< typename Array::UnderlyingType > >::Result,
				typename Select<
					Pointer::IsAPointer,
					typename ReplacePointerBaseType< T, U< typename Pointer::UnderlyingType > >::Result,
					U< T >
				>::Result
			>::Result WrapType;
			
			typedef typename QueryType< WrapType >::Result Result;
	};		*/

#endif

//EOF
