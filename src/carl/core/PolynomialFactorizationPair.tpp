/* 
 * File:   PolynomialFactorizationPair.tpp
 * Author: Florian Corzilius
 *
 * Created on September 5, 2014, 3:57 PM
 */

#pragma once

#include "PolynomialFactorizationPair.h"

namespace carl
{   
    template <typename P>
    std::ostream& operator<<( std::ostream& _out, const Factorization<P>& _factorization )
    {
        assert( _factorization.size() != 1 || _factorization.begin()->second > 1 );
        if( _factorization.empty() )
        {
            _out << "1";
        }
        else if( _factorization.size() == 1 )
        {
            _out << _factorization.begin()->first;
            _out << "^" << _factorization.begin()->second;
        }
        else
        {
            for( auto polyExpPair = _factorization.begin(); polyExpPair != _factorization.end(); ++polyExpPair )
            {
                if( polyExpPair != _factorization.begin() )
                    _out << ") * (";
                else
                    _out << "(";
                _out << polyExpPair->first;
                assert( polyExpPair->second != 0 );
                if( polyExpPair->second > 1 )
                {
                    _out << "^" << polyExpPair->second;
                }
            }
            _out << ")";
        }
        return _out;
    }
    
    template<typename P>
    bool factorizationsEqual( const Factorization<P>& _factorizationA, const Factorization<P>& _factorizationB )
    {
        auto iterA = _factorizationA.begin();
        auto iterB = _factorizationB.begin();
        while( iterA != _factorizationA.end() && iterB != _factorizationB.end() )
        {
            if( iterA->second != iterB->second || !(iterA->first == iterB->first) )
                break;
            ++iterA; ++iterB;
        }
        return iterA == _factorizationA.end() && iterB == _factorizationB.end();
    }
    
    template<typename P>
    PolynomialFactorizationPair<P>::PolynomialFactorizationPair( Factorization<P>&& _factorization, P* _polynomial ):
        mHash( 0 ),
        mpPolynomial( _polynomial ),
        mFactorization( std::move( _factorization ) )
    {
        rehash();
    }
    
    template<typename P>
    void PolynomialFactorizationPair<P>::rehash()
    {
        std::lock_guard<std::recursive_mutex> lock( mMutex );
        if( mpPolynomial == nullptr )
        {
            assert( mFactorization.empty() );
            mHash = 0;
            for( auto polyExpPair = mFactorization.begin(); polyExpPair != mFactorization.end(); ++polyExpPair )
            {
                mHash = (mHash << 5) | (mHash >> (sizeof(size_t)*8 - 5));
                mHash ^= std::hash<FactorizedPolynomial<P>>()( polyExpPair->first );
                mHash ^= polyExpPair->second;
            }
        }
        else
        {
            mHash = std::hash<P>()( *mpPolynomial );
        }
    }
    
    template<typename P>
    bool operator==( const PolynomialFactorizationPair<P>& _polyFactA, const PolynomialFactorizationPair<P>& _polyFactB )
    {
        if( &_polyFactA == &_polyFactB )
            return true;
        std::lock_guard<std::recursive_mutex> lockA( _polyFactA.mMutex );
        std::lock_guard<std::recursive_mutex> lockB( _polyFactB.mMutex );
        if( _polyFactA.mpPolynomial != nullptr && _polyFactB.mpPolynomial != nullptr )
        {
            return *_polyFactA.mpPolynomial == *_polyFactB.mpPolynomial;
        }
        else
        {
            return factorizationsEqual( _polyFactA.mFactorization, _polyFactB.mFactorization );
        }
    }
    
    template<typename P>
    bool operator<( const PolynomialFactorizationPair<P>& _polyFactA, const PolynomialFactorizationPair<P>& _polyFactB )
    {
        if( &_polyFactA == &_polyFactB )
            return false;
        std::lock_guard<std::recursive_mutex> lockA( _polyFactA.mMutex );
        std::lock_guard<std::recursive_mutex> lockB( _polyFactB.mMutex );
        if( _polyFactA.mpPolynomial != nullptr && _polyFactB.mpPolynomial != nullptr )
        {
            return *_polyFactA.mpPolynomial < *_polyFactB.mpPolynomial;
        }
        else
        {
            auto iterA = _polyFactA.mFactorization.begin();
            auto iterB = _polyFactB.mFactorization.begin();
            while( iterA != _polyFactA.mFactorization.end() && iterB != _polyFactB.mFactorization.end() )
            {
                if( iterA->first < iterB->first )
                    return true;
                else if( iterA->first == iterB->first )
                {
                    if( iterA->second < iterB->second )
                        return true;
                    else if( iterA->second > iterB->second )
                        return false;
                }
                else
                    return false;
                ++iterA; ++iterB;
            }
            return false;
        }
    }
    
    template<typename P>
    bool canBeUpdated( const PolynomialFactorizationPair<P>& _toUpdate, const PolynomialFactorizationPair<P>& _updateWith )
    {
        if( &_toUpdate == &_updateWith )
            return false;
        std::lock_guard<std::recursive_mutex> lockA( _toUpdate.mMutex );
        std::lock_guard<std::recursive_mutex> lockB( _updateWith.mMutex );
        assert( _toUpdate.getHash() == _updateWith.getHash() && _toUpdate == _updateWith );
        if( _toUpdate.mpPolynomial == nullptr && _updateWith.mpPolynomial != nullptr )
            return true;
        assert( _updateWith.mpPolynomial == nullptr || (*_toUpdate.mpPolynomial) == (*_updateWith.mpPolynomial) );
        return !factorizationsEqual( _toUpdate.mFactorization, _updateWith.mFactorization );
    }

    template<typename P>
    void update( PolynomialFactorizationPair<P>& _toUpdate, PolynomialFactorizationPair<P>& _updateWith )
    {
        assert( canBeUpdated( _toUpdate, _updateWith ) ); // This assertion only ensures efficient use this method.
        assert( &_toUpdate != &_updateWith );
        std::lock_guard<std::recursive_mutex> lockA( _toUpdate.mMutex );
        std::lock_guard<std::recursive_mutex> lockB( _updateWith.mMutex );
        if( _toUpdate.mpPolynomial == nullptr && _updateWith.mpPolynomial != nullptr )
            _toUpdate.mpPolynomial = _updateWith.mpPolynomial;
        if( !factorizationsEqual( _toUpdate.mFactorization, _updateWith.mFactorization ) )
        {
            // Calculating the gcd refines both factorizations to the same factorization
            gcd( _toUpdate, _updateWith );
        }
        _toUpdate.rehash();
    }
    
    template<typename P>
    Factorization<P> gcd( PolynomialFactorizationPair<P>& _pfPairA, const PolynomialFactorizationPair<P>& _pfPairB, bool& _pfPairARefined, bool& _pfPairBRefined )
    {
        if( &_pfPairA == &_pfPairB )
            return _pfPairA.mFactorization;
        std::lock_guard<std::recursive_mutex> lockA( _pfPairA.mMutex );
        std::lock_guard<std::recursive_mutex> lockB( _pfPairB.mMutex );
        Factorization<P> result;
        // TODO: implementation
        return result;
    }
    
    template <typename P>
    std::ostream& operator<<(std::ostream& _out, const PolynomialFactorizationPair<P>& _pfPair)
    {
        if( _pfPair.factorization().size() == 1 && _pfPair.factorization().begin()->second )
        {
            assert( _pfPair.factorization().begin()->second == 1 );
            assert( _pfPair.mpPolynomial != nullptr );
            _out << *_pfPair.mpPolynomial;
        }
        else
        {   
            _out << _pfPair.factorization();
        }
        return _out;
    }
    
} // namespace carl