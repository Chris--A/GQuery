
#include <GQuery.h>

char c[][6] = {{ 'a', 'b', 'c', 'd', 'e', '\0' }, { 'a', 'b', 'c', 'd', 'e', '\0' }, { 'a', 'b', 'c', 'd', 'e', '\0' }};
const char d[][6] = {{ 'A', 'B', 'C', 'D', 'E', '\0' }, { 'F', 'G', 'H', 'I', 'J', '\0' }, { 'K', 'L', 'M', 'N', 'O', '\0' }};
const char e[][6] PROGMEM = {{ 'P', 'Q', 'R', 'S', 'T', '\0' },{ 'p', 'q', 'r', 's', 't', '\0' }};


//GQ::char7 f[] = "asda";//{ 'a', 'b', 'c' };

static void Func( unsigned int idx, typeof( c[0] ) &el ){
    Serial.print( idx, DEC );
    Serial.print( F( "{ " ) );  
    _( el ).each( Func );
    Serial.println( F( " }" ) );  
}

static void Func( unsigned int idx, char el ){
    Serial.print( idx, DEC );
    Serial.print( F( ": '" ) );
    Serial.print( el ? el : '0' ); 
    if( idx < 5 ) Serial.print( F( "', " ) ); 
    else Serial.print( '\'' ); 
}

static void Func_Single( unsigned int idx, char el ){
    if( !idx ) Serial.print( "{ " );
    Serial.print( idx, DEC );
    Serial.print( F( ": '" ) );
    Serial.print( el ? el : '0' ); 
    if( idx < 5 ) Serial.print( F( "', " ) ); 
    else Serial.print( F( "\' }\r\n" ) ); 
}



void setup() {
  Serial.begin( 9600 );
  


  bool b;
  _( b ).isType( char() );

  typedef char *arp[3];
  arp g = {};
  
  //_( g ).index< 4 >();
  
  
  //typedef arp* arpp;
  //arpp eeee = c;  
  
 // _( eeee ).Index< 6 >()[ _.all ].each(  );
  
 // _( c ).Do();
  
  //_( c ).isPointer;

  //_( c ).each( Func );
  //_( c )[ _.all ].each( Func_Single );
  
  PrintArr( F( "Initial Array" ) );

  _( c ) = d;
  PrintArr( F( "Assign array to array" ) );
  
  _( c ) = 'z';
  PrintArr( F( "Assign value to array" ) );
  
  _( c )[ _.evens ][ _.odds ][ _.every.fourth ] = 1;
  PrintArr( F( "Iterate bits of an array" ) );
  
  char *meta = &c[ 0 ][ 3 ];
  char ttt[ 12 ];
  
  _( meta ).index< 12 >()= 'Q';
  PrintArr( F( "Add index to pointer and use as array ( copy value )" ) );

  
  _( c )[ 1 ] = _.PGM( e )[ 0 ];  
  _( c )[ 2 ] = _.PGM( e )[ 1 ];  
  PrintArr( F( "Assign PROGMEM array to array" ) );
  
  /*---
    Access raw bits of a unsigned/signed char.
  ---*/
  
  unsigned char f = 0;
  Serial.print( "\r\n - Access raw bits of a unsigned/signed char:\r\n F: " );
  Serial.println( f );  
  _( f )[ 7 ] = 1;
  Serial.print( F( "F: " ) );
  Serial.println( (unsigned int ) f, HEX );

}

void loop(){}

static inline void Print_Func( unsigned int idx, typeof( c[0] ) &el ){
  Serial.print( el );
  if( idx < 2 ) Serial.print( ", " );
  else Serial.print( "\r\n" );
}


void PrintArr( const __FlashStringHelper *pgm ){
  Serial.print( F( "\r\n - " ) );
  Serial.print( pgm );
  Serial.print( F( ": \r\n    " ) );
  
  //c[ 0 ][ 5 ] = 0; //Add null to print correctly.
  //c[ 1 ][ 5 ] = 0;
  //c[ 2 ][ 5 ] = 0;  
  _( c )[ _.all ][ _( c ).Second - 1 ] = 0;  
  _( c ).each( Print_Func );
}

  
  
/*
typedef bool(*new_t)[4];

char a[ 4 ];

new_t b = ( new_t ) &a;

new_t func( char (*a)[4] );
new_t func( char (*a)[4] ){
  return ( new_t ) a;
}
//bool (&b)[4] = *func( &a );
//b[ 0 ] = true;
*/
