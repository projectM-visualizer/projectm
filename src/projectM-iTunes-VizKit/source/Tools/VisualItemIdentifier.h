/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualItemIdentifier.h
 *
 */

/***************************************************************************

Copyright (c) 2004-2009 Heiko Wichmann (http://www.imagomat.de/vizkit)


This software is provided 'as-is', without any expressed or implied warranty. 
In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; 
   you must not claim that you wrote the original software. 
   If you use this software in a product, an acknowledgment 
   in the product documentation would be appreciated 
   but is not required.

2. Altered source versions must be plainly marked as such, 
   and must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.

 ***************************************************************************/

#ifndef VisualItemIdentifier_h
#define VisualItemIdentifier_h


#include "VisualTypes.h"
#include <map>


namespace VizKit {

	/**
	 * Identifier of an item (like e.g. a VisualNurbs or a VisualAnimationComponent).
	 * The internal token of the identifier stays the same across copy operations. Comparisons of copied instances yield equal result. Identifiers can be shared by supplying a name. If another VisualItemIdentifier with the same name has been created already, it is returned by VisualItemIdentifierRegistry's getVisualItemIdentifier(). 
	 */
	class VisualItemIdentifier {
	
	public:
	
		/**
		 * The constructor.
		 */
		VisualItemIdentifier();
		
		/**
		 * Another constructor.
		 * @param aName An optional identifier name (for shared VisualItemIdentifiers).
		 */
		VisualItemIdentifier(const char* const aName);
		
		/**
		 * The destructor.
		 */		
		~VisualItemIdentifier();
		
		/**
		 * Copy constructor.
		 * @param other Another VisualItemIdentifier.
		 */
		VisualItemIdentifier(const VisualItemIdentifier& other);

		/**
		 * Assignment operator.
		 */			
		VisualItemIdentifier& operator=(const VisualItemIdentifier& other);

		/**
		 * Less than operator.
		 * @param other Another VisualItemIdentifier.
		 * @remarks The less than operator is used by std::map for find() and sorting (if VisualItemIdentifier is used as key_type).
		 */
		bool operator<(const VisualItemIdentifier& other) const;
		
		/**
		 * Equality operator.
		 * @param other Another VisualItemIdentifier.
		 */
		bool operator==(const VisualItemIdentifier& other) const;

		/**
		 * Inequality operator.
		 * @param other Another VisualItemIdentifier.
		 */
		bool operator!=(const VisualItemIdentifier& other);

		/**
		 * Returns a VisualItemIdentifier identified by name. By using a name as token, the same identifier can be used at different places.
		 * @param aName The name of the VisualItemIdentifier.
		 * @return A pointer to a visualItemIdentifier identified by its name.
		 * @remarks The caller has to release the allocated memory by calling delete() on the pointer.
		 */			
		static VisualItemIdentifier* createVisualItemIdentifier(const char* const aName);

		/**
		 * Returns the name (NULL if there is none).
		 * @return The optional identifier name.
		 */		
		const char* const getName(void);

	private:

		/**
		 * Private constructor.
		 * @param aName Name of VisualItemIdentifier.
		 * @param aToken Token of VisualItemIdentifier.
		 */
		VisualItemIdentifier(const char* const aName, uint32 aToken);
		
		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualItemIdentifier.
		 */
		void copy(const VisualItemIdentifier& other);
		
		/**
		 * The optional identifier name (for shared VisualItemIdentifiers).
		 */	
		char* name;

		/**
		 * Internal method to receive the next valid token.
		 */	
		static uint32 getNextToken();
		
		/**
		 * The internal identifier of the VisualItemIdentifier.
		 * @remarks The internal identifier stays the same across copies and instances. Since it is typed as 32bit unsigned integer the number of identifiers is limited to 4294967296.
		 */	
		uint32 token;
	
		/** A VisualItemIdentifierTokenNameMap is a map of names and tokens. */
		typedef std::map<std::string, uint32> VisualItemIdentifierNameTokenMap;
		
		/** A map of names and tokens. */
		static VisualItemIdentifierNameTokenMap visualItemIdentifierNameTokenMap;
		
		/** A VisualItemIdentifierNameTokenMapIterator is an iterator of a VisualItemIdentifierNameTokenMap. */
		typedef VisualItemIdentifierNameTokenMap::iterator VisualItemIdentifierNameTokenMapIterator;

	};
	
}

#endif /* VisualItemIdentifier_h */
