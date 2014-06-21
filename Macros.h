


	/*--- An accessor for CRTP based entities. ---*/
	#define DERIVED_TO_SELF		D& self() { return *static_cast< D* >( this ); } \
								const D& self() const { return *static_cast< const D* >( this ); }	
								
								
	#define EOL					( IsSameType< typename List::Tail, NullType >::value )