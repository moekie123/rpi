#pragma ocne

#include <string>

//! Factory
/*!
 * An, template based, Factory design pattern
 */
template < class T > class Factory 
{
  public:

	//! Entity creator
	/*!
	 *	\param type The derived class from which the returned base is derived
	 *	\return Base class from the created type
	 */
	T *create( const std::string & type );
    
	//! Entity creator
	/*!
	 *	\param type The derived class from which the returned base is derived
	 *  \param name The name of created object
	 *	\return Base class from the created type
	 */
	T *create( const std::string & type, const std::string & name );
};
