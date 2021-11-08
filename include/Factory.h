
#pragma ocne

#include <string>
template < class T > class Factory {
  public:
    T *create( const std::string & );
    T *create( const std::string &, const std::string & );
};
