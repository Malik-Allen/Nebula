// MIT License, Copyright (c) 2019 Malik Allen

#ifndef NEBULA_TEMPLATEHELPER_H
#define NEBULA_TEMPLATEHELPER_H

namespace Nebula
{
	// Template Parameter Compile Constraint, Thanks Bjarne Stroustrup: https://www.stroustrup.com/bs_faq2.html#constraints
	template<class T, class B> struct CanConvert_From
	{
		static void constraints( T* p ) { B* pb = p; }
		// Complile-time check to see if class T can be converted to class B, valid for derivation check of class T from class B
		CanConvert_From() { void( *p )( T* ) = constraints; }
	};
}

#endif // !NEBULA_TEMPLATEHELPER_H
