
#include <Arduino.h>

template< bool _Reverse >
  class _BitRef{
  public:
  
    struct DataRef{

      DataRef( byte &data, const uint8_t Idx ) : Data( data ), Index( _Reverse ? ( 0x80 >> Idx ) : ( 0x1 << Idx ) ) {}

      operator const bool() const { return this->Data & this->Index; }

      DataRef &operator =( const DataRef &copy ) { return *this = ( const bool ) copy; }

      DataRef &operator =( const bool &copy ) {
        if( copy )  
          this->Data |= Index;
        else
          this->Data &= ~Index;
        return *this;
      }      

      byte &Data;
      byte const Index;
    };
    _BitRef( void *ptr ) : Start( ( byte* ) ptr ) {}
	
    template< typename T > _BitRef( T &t ) : Start( ( byte* ) &t ) {}
	
    DataRef operator []( const uint16_t index ) { return DataRef( this->Start[ index >> 0x3 ], index & 0x7 ); }
	
	byte *Start;
};

template< unsigned N, bool _Reverse >
  struct _BitBool : public _BitRef< _Reverse >{
    public:
	
      enum{ BitCount = N, ByteCount = ( BitCount / 0x8 ) + ( ( BitCount % 0x8 ) ? 0x1 : 0x0 ) };
	  
      _BitBool( void ) : _BitRef< _Reverse >( Data ) {}
	  
      byte Data[ ByteCount ];
};  

typedef _BitRef< false > BitRef;
typedef _BitRef< true > BitRefR;

template< unsigned N > class BitBool : public _BitBool< N, false >{}; 
template< unsigned N > class BitBoolR : public _BitBool< N, true >{}; 

