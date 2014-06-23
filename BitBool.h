
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

