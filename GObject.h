
/*---
	The MIT License (MIT)

	Copyright (c) 2014 Christopher Andrews, genx.biz

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

/*---
	The MIT License (MIT)

	Copyright (c) 2014 Christopher Andrews, genx.biz
	Development and design names Copyright (c) 1998 - 2014: Genx,
	Generation Entertainment Networks. 	A subsidurary of the collective works of
	Christopher Andrews.
	
	Provide appropriate disrimination when using Genx in your own product.

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

#ifndef HEADER_GOBJECT
	#define HEADER_GOBJECT

	/*---
		ObjectAccess implements access to raw object bytes and is also responsible for bit manipulation features.
	---*/	
	
	template< typename T > 
		struct ObjectAccess{ 
			typedef QueryInterface< T > Result; 
			static Result Call( T &t, uint8_t i ){ return Result( *( ( ( unsigned char* ) &t ) + i ) ); } 
	};

	template<> 
		struct ObjectAccess< char >{
			typedef BitRef::DataRef Result;
			static Result Call( char &t, uint8_t i ){ return Result( *( unsigned char* ) &t, i ); }			
	};		
	
	template<> 
		struct ObjectAccess< unsigned char >{
			typedef BitRef::DataRef Result;
			static Result Call( unsigned char &t, uint8_t i ){ return Result( t, i ); }			
	};
	/*
	template< template< typename > class T, typename U >
		struct ObjectAccess< T< U > >{
			typedef const typename ObjectAccess< U >::Result Result;
			static Result Call( T< U > &t, uint8_t i ){ 
				struct TempHold{ 
					TempHold( T< U > &tt ) : u( ( U ) tt ) {}
					U u;
				} th( t );
				return Result( th.u, i ); 
			}			
			
	};*/
	
#endif

//EOF