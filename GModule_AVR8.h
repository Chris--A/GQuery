
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
