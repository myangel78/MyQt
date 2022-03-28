#ifndef _ENUMSTRING_H_
#define _ENUMSTRING_H_

#include <string>
#include <map>
#include <cassert>

// Helper macros

#define Begin_Enum_String(EnumerationName)                                      \
    template <> struct EcoliEnumString<EnumerationName> :                            \
        public EcoliEnumStringBase< EcoliEnumString<EnumerationName>, EnumerationName >   \
    {                                                                           \
        static void RegisterEnumerators()

#define Enum_String(EnumeratorName)                                             \
            RegisterEnumerator( EnumeratorName, #EnumeratorName );

#define End_Enum_String                                                         \
    }

// The EcoliEnumStringBase base class
template <class DerivedType, class EnumType>
class EcoliEnumStringBase
{
// Types
protected:
    typedef std::map<std::string, EnumType> AssocMap;

protected:
// Constructor / Destructor
    explicit EcoliEnumStringBase();
    ~EcoliEnumStringBase();

private:
// Copy Constructor / Assignment Operator
	EcoliEnumStringBase(const EcoliEnumStringBase &);
    const EcoliEnumStringBase &operator =(const EcoliEnumStringBase &);

// Functions
private:
    static AssocMap &GetMap();

protected:
    // Use this helper function to register each enumerator
    // and its string representation.
    static void RegisterEnumerator(const EnumType e, const std::string &eStr);

public:
    // Converts from an enumerator to a string.
    // Returns an empty string if the enumerator was not registered.
    static const std::string &From(const EnumType e);
    static const std::string &From(const int e);

    // Converts from a string to an enumerator.
    // Returns true if the conversion is successful; false otherwise.
    static bool To(EnumType &e, const std::string &str);
};

// The EnumString class
// Note: Specialize this class for each enumeration, and implement
//       the RegisterEnumerators() function.
template <class EnumType>
struct EcoliEnumString : public EcoliEnumStringBase< EcoliEnumString<EnumType>, EnumType >
{
    static void RegisterEnumerators();
};

// Function definitions

template <class D, class E>
typename EcoliEnumStringBase<D,E>::AssocMap &EcoliEnumStringBase<D,E>::GetMap()
{
    // A static map of associations from strings to enumerators
    static AssocMap assocMap;
    static bool     bFirstAccess = true;

    // If this is the first time we're accessing the map, then populate it.
    if( bFirstAccess )
    {
        bFirstAccess = false;
        D::RegisterEnumerators();
        //assert( !assocMap.empty() );
    }

    return assocMap;
}

template <class D, class E>
void EcoliEnumStringBase<D,E>::RegisterEnumerator(const E e, const std::string &eStr)
{
    const bool bRegistered = GetMap().insert( typename AssocMap::value_type( eStr, e ) ).second;
    //assert( bRegistered );
    (void)sizeof( bRegistered ); // This is to avoid the pesky 'unused variable' warning in Release Builds.
}

template <class D, class E>
const std::string &EcoliEnumStringBase<D,E>::From(const E e)
{
    for(;;) // Code block
    {
        // Search for the enumerator in our map
        typename AssocMap::const_iterator i;
        for(i = GetMap().begin(); i != GetMap().end(); ++i)
            if( (*i).second == e )
                break;

        // If we didn't find it, we can't do this conversion
        if( i == GetMap().end() )
            break;

        // Keep searching and see if we find another one with the same value
        typename AssocMap::const_iterator j( i );
        for(++j; j != GetMap().end(); ++j)
            if( (*j).second == e )
                break;

        // If we found another one with the same value, we can't do this conversion
        if( j != GetMap().end() )
            break;

        // We found exactly one string which matches the required enumerator
        return (*i).first;
    }

    // We couldn't do this conversion; return an empty string.
    static const std::string dummy;
    return dummy;
}



template <class D, class E>
const std::string &EcoliEnumStringBase<D,E>::From(const int e)
{
    for(;;) // Code block
    {
        // Search for the enumerator in our map
        typename AssocMap::const_iterator i;
        for(i = GetMap().begin(); i != GetMap().end(); ++i)
            if( (*i).second == e )
                break;

        // If we didn't find it, we can't do this conversion
        if( i == GetMap().end() )
            break;

        // Keep searching and see if we find another one with the same value
        typename AssocMap::const_iterator j( i );
        for(++j; j != GetMap().end(); ++j)
            if( (*j).second == e )
                break;

        // If we found another one with the same value, we can't do this conversion
        if( j != GetMap().end() )
            break;

        // We found exactly one string which matches the required enumerator
        return (*i).first;
    }

    // We couldn't do this conversion; return an empty string.
    static const std::string dummy;
    return dummy;
}

template <class D, class E>
bool EcoliEnumStringBase<D,E>::To(E &e, const std::string &str)
{
    // Search for the string in our map.
    const typename AssocMap::const_iterator itr( GetMap().find( str ) );

    // If we have it, then return the associated enumerator.
    if( itr != GetMap().end() )
    {
        e = (*itr).second;
        return true;
    }

    // We don't have it; the conversion failed.
    return false;
}

#endif // !_ECOLI_ENUMSTRING_H_
