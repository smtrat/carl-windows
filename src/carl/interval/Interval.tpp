/**
 * The implementation for the templated interval class.
 *
 * @file Interval.tpp
 * @autor Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 * 
 * @since	2013-12-20
 * @version 2014-01-07
 */
#pragma once
#include "Interval.h"

namespace carl
{
	
	using namespace boost::numeric;

/*******************************************************************************
 * Constructors & Destructor
 ******************************************************************************/
/*
template<typename Rational, typename Number>
Interval<Number>::Interval(Rational n);

template<typename Rational, typename Number>
Interval<Number>::Interval(Rational lower, Rational upper);

template<typename Rational, typename Number>
Interval<Number>::Interval(Rational lower, BoundType lowerBoundType, Rational upper, BoundType upperBoundType);
*/
/*******************************************************************************
 * Transformations and advanced getters/setters
 ******************************************************************************/
	
template<typename Number>
Number Interval<Number>::diameter() const
	{
		return boost::numeric::width(mContent);
	}

template<typename Number>
void Interval<Number>::diameter_assign()
	{
		mContent = BoostInterval(this->diameter());
	}

template<typename Number>
	Number Interval<Number>::diameterRatio(const Interval<Number>& rhs) const
	{
		assert(rhs.diameter() != Number(0));
		return this->diameter()/rhs.diameter();
	}

template<typename Number>
	void Interval<Number>::diameterRatio_assign(const Interval<Number>& rhs)
	{
		mContent = BoostInterval(this->diameterRatio(rhs));
	}

template<typename Number>
	Number Interval<Number>::magnitude() const
	{
		return max(abs(mContent.lower()), abs(mContent.upper()));
	}

template<typename Number>
	void Interval<Number>::magnitude_assign()
	{
		mContent = BoostInterval(this->magnitude());
	}

template<typename Number>
	Number Interval<Number>::center() const
	{
		return boost::numeric::median(mContent);
	}

template<typename Number>
	void Interval<Number>::center_assign()
	{
		mContent = BoostInterval(this->center());
	}
	
	template<typename Number>
	bool Interval<Number>::contains(const Number& val) const
	{
		switch( mLowerBoundType )
        {
            case BoundType::INFTY:
                break;
            case BoundType::STRICT:
                if( mContent.lower() >= val )
                    return false;
                break;
            case BoundType::WEAK:
                if( mContent.lower() > val )
                    return false;
        }
        // Invariant: n is not conflicting with left bound
        switch( mUpperBoundType )
        {
            case BoundType::INFTY:
                break;
            case BoundType::STRICT:
                if( mContent.upper() <= val )
                    return false;
                break;
            case BoundType::WEAK:
                if( mContent.upper() < val )
                    return false;
                break;
        }
        return true;    // for open intervals: (left() < n && right() > n) || (n == 0 && left() == cln::cl_RA( 0 ) && right() == cln::cl_RA( 0 ))	}
	}
	
	template<typename Number>
	bool Interval<Number>::contains(const Interval<Number>& rhs) const
	{
		if( mContent.lower() < rhs.lower() && mContent.upper() > rhs.upper() )
		{
			return true;
		}
		
		if( mContent.lower() == rhs.lower() && mContent.upper() == rhs.upper() )
		{
			bool lowerOk = false;
			switch( mLowerBoundType )
			{
				case BoundType::INFTY:
					lowerOk = (rhs.lowerBoundType() == BoundType::INFTY);
					break;
				case BoundType::STRICT:
					lowerOk = (rhs.lowerBoundType() == BoundType::STRICT);
					break;
				case BoundType::WEAK:
					lowerOk = (rhs.lowerBoundType() != BoundType::INFTY);
			}
			if( lowerOk )
			{
				switch( mUpperBoundType )
				{
					case BoundType::INFTY:
						return rhs.upperBoundType() == BoundType::INFTY;
					case BoundType::STRICT:
						return rhs.upperBoundType() == BoundType::STRICT;
					case BoundType::WEAK:
						return rhs.upperBoundType() != BoundType::INFTY;
				}
			}
			return false; // the lower boundaries are not ok.
		}
		else
		{
			return false; // not less and not equal
		}
	}
	
	template<typename Number>
	bool Interval<Number>::subset(const Interval<Number>& rhs) const
	{
		return this->contains(rhs);
	}
	
	template<typename Number>
	bool Interval<Number>::proper_subset(const Interval<Number>& rhs) const
	{
		if( mContent.lower() < rhs.lower() && mContent.upper() > rhs.upper() )
		{
			return true;
		}
		
		if( mContent.lower() == rhs.lower() && mContent.upper() == rhs.upper() )
		{
			bool lowerOk = false;
			switch( mLowerBoundType )
			{
				case BoundType::INFTY:
					lowerOk = (rhs.lowerBoundType() == BoundType::INFTY);
					break;
				case BoundType::STRICT:
					lowerOk = (rhs.lowerBoundType() == BoundType::STRICT);
					break;
				case BoundType::WEAK:
					lowerOk = (rhs.lowerBoundType() != BoundType::INFTY);
			}
			if( lowerOk )
			{
				switch( mUpperBoundType )
				{
					case BoundType::INFTY:
						return rhs.upperBoundType() == BoundType::INFTY;
					case BoundType::STRICT:
						return rhs.upperBoundType() == BoundType::STRICT;
					case BoundType::WEAK:
						return rhs.upperBoundType() != BoundType::INFTY;
				}
			}
			return false; // the lower boundaries are not ok.
		}
		else
		{
			return false; // not less and not equal
		}
	}
	
template<typename Number>
	void Interval<Number>::bloat_by(const Number& width)
	{
		mContent = boost::numeric::widen(mContent, width);
	}

template<typename Number>
void Interval<Number>::bloat_times(const Number& factor)
	{
		mContent = BoostInterval(mContent.lower() - (this->diameter / Number(2) * factor), mContent.upper() + (this->diameter / Number(2) * factor));
	}

template<typename Number>
void Interval<Number>::shrink_by(const Number& width)
	{
		this->bloat_by(Number(-width));
	}

template<typename Number>
void Interval<Number>::shrink_times(const Number& factor)
	{
		this->bloat_times(Number(-factor));
	}

template<typename Number>
std::pair<Interval<Number>, Interval<Number>> Interval<Number>::split() const
	{
		std::pair<BoostInterval, BoostInterval> bisection = boost::numeric::bisect(mContent);
		return std::pair<Interval<Number>, Interval<Number> >(Interval(bisection.first, mLowerBoundType, BoundType::STRICT), Interval(bisection.second, BoundType::WEAK, mUpperBoundType));
	}

template<typename Number>
std::list<Interval<Number>> Interval<Number>::split(unsigned n) const
	{
		std::list<Interval<Number> > result;
		if(n == 0)
		{
			result.push_back(*this);
			return result;
		}
		Number diameter = this->diameter();
        diameter /= n;
		
        Interval<Number> tmp;
        tmp.set(mContent.lower(), mContent.lower()+diameter);
        tmp.setLowerBoundType(mLowerBoundType);
        tmp.setUpperBoundType(BoundType::STRICT);
        result.push_back(tmp);
		
        for( unsigned i = 1; i < (n-1); ++i )
        {
            tmp.set(diameter*i, diameter*(i+1));
            result.push_back(tmp);
        }
		
        tmp.set(diameter*(n-1),diameter*n);
        tmp.setUpperBoundType(mUpperBoundType);
        result.push_back(tmp);
		return result;
	}
	
template<typename Number>
	std::string Interval<Number>::toString() const
	{
		std::ostringstream oss;
		switch (mLowerBoundType) {
			case BoundType::INFTY:
				oss << std::string("]-INF, ");
				break;
			case BoundType::STRICT:
				oss << std::string("]") << mContent.lower() << ", ";
				break;
			case BoundType::WEAK:
				oss << std::string("[") << mContent.lower() << ", ";
		}
		switch (mUpperBoundType) {
			case BoundType::INFTY:
				oss << std::string("INF[");
				break;
			case BoundType::STRICT:
				oss << mContent.upper() << std::string("[");
				break;
			case BoundType::WEAK:
				oss << mContent.upper() << std::string("]");
		}
		return oss.str();
	}

/*******************************************************************************
 * Arithmetic functions
 ******************************************************************************/

template<typename Number>
Interval<Number> Interval<Number>::add(const Interval<Number>& rhs) const
	{
		return Interval<Number>( mContent + rhs.content(),
                              getWeakestBoundType( mLowerBoundType, rhs.lowerBoundType() ),
                              getWeakestBoundType( mUpperBoundType, rhs.upperBoundType() ) );
	}

template<typename Number>
void Interval<Number>::add_assign(const Interval<Number>& rhs)
	{
		*this = this->add(rhs);
	}

template<typename Number>
Interval<Number> Interval<Number>::sub(const Interval<Number>& rhs) const
	{
		return this->add(rhs.inverse());
	}

template<typename Number>
void Interval<Number>::sub_assign(const Interval<Number>& rhs)
	{
		*this = this->sub(rhs);
	}

template<typename Number>
Interval<Number> Interval<Number>::mul(const Interval<Number>& rhs) const
	{
		BoundType leftType = BoundType::WEAK;
        BoundType rightType = BoundType::WEAK;
        if( (mLowerBoundType == BoundType::INFTY && (rhs.upper() > 0 || rhs.upperBoundType() == BoundType::INFTY))
		   || (mUpperBoundType == BoundType::INFTY && (rhs.lower() < 0 || rhs.lowerBoundType() == BoundType::INFTY))
		   || (rhs.lowerBoundType() == BoundType::INFTY && (mContent.upper() > 0 || mUpperBoundType == BoundType::INFTY))
		   || (rhs.upperBoundType() == BoundType::INFTY && (mContent.upper() < 0 || (mContent.lower() < 0 || mLowerBoundType == BoundType::INFTY))) )
        {
            leftType = BoundType::INFTY;
        }
        if( (mLowerBoundType == BoundType::INFTY && (rhs.upper() < 0 || (rhs.lower() < 0 || rhs.lowerBoundType() == BoundType::INFTY)))
		   || (upperBoundType() == BoundType::INFTY && (rhs.lower() > 0 || (rhs.upper() > 0 || rhs.upperBoundType() == BoundType::INFTY)))
		   || (rhs.lowerBoundType() == BoundType::INFTY && (mContent.upper() < 0 || (mContent.lower() < 0 || mLowerBoundType == BoundType::INFTY)))
		   || (rhs.upperBoundType() == BoundType::INFTY && (mContent.lower() > 0 || (mContent.upper() > 0 || mUpperBoundType == BoundType::INFTY))) )
        {
            rightType = BoundType::INFTY;
        }
        return Interval<Number>(BoostInterval( mContent*rhs.content() ), leftType, rightType );
	}

template<typename Number>
void Interval<Number>::mul_assign(const Interval<Number>& rhs)
	{
		*this = this->mul(rhs);
	}

template<typename Number>
Interval<Number> Interval<Number>::div(const Interval<Number>& rhs) const
	{
		BoundType leftType = BoundType::WEAK;
        BoundType rightType = BoundType::WEAK;
        if( (mLowerBoundType == BoundType::INFTY && (rhs.upper() > 0 || rhs.upperBoundType() == BoundType::INFTY))
		   || (mUpperBoundType == BoundType::INFTY && (rhs.lower() < 0 || rhs.lowerBoundType() == BoundType::INFTY))
		   || (rhs.lowerBoundType() == BoundType::INFTY && ( mContent.upper() > 0 || mUpperBoundType == BoundType::INFTY))
		   || (rhs.upperBoundType() == BoundType::INFTY && ( mContent.upper() < 0 || ( mContent.lower() < 0 || mLowerBoundType == BoundType::INFTY))) )
        {
            leftType = BoundType::INFTY;
        }
        if( (mLowerBoundType == BoundType::INFTY && (rhs.upper() < 0 || (rhs.lower() < 0 || rhs.lowerBoundType() == BoundType::INFTY)))
		   || (mUpperBoundType == BoundType::INFTY && (rhs.lower() > 0 || (rhs.upper() > 0 || rhs.upperBoundType() == BoundType::INFTY)))
		   || (rhs.lowerBoundType() == BoundType::INFTY && ( mContent.upper() < 0 || ( mContent.lower() < 0 || mLowerBoundType == BoundType::INFTY)))
		   || (rhs.upperBoundType() == BoundType::INFTY && ( mContent.lower() > 0 || ( mContent.upper() > 0 || mUpperBoundType == BoundType::INFTY))) )
        {
            rightType = BoundType::INFTY;
        }
        return Interval<Number>(BoostInterval( mContent/rhs.content() ), leftType, rightType );
	}

template<typename Number>
void Interval<Number>::div_assign(const Interval<Number>& rhs)
	{
		*this = this->div(rhs);
	}

template<typename Number>
bool Interval<Number>::div_ext(const Interval<Number>& rhs, Interval<Number>& a, Interval<Number>& b) const
	{
		Interval<Number> reciprocalA, reciprocalB;
        bool          splitOccured;
		
        if( rhs.lowerBoundType() != BoundType::INFTY && rhs.lower() == 0 && rhs.upperBoundType() != BoundType::INFTY && rhs.upper() == 0 )    // point interval 0
        {
            splitOccured = false;
            if( this->contains( 0 ))
            {
                a = unboundedInterval();
            }
            else
            {
                a = emptyInterval();
            }
            return false;
        }
        else
        {
            if( rhs.isUnbounded() )
            {
                a = unboundedInterval();
                return false;
            }    // o.unbounded
            else
            {
                //default case
                splitOccured = rhs.reciprocal( reciprocalA, reciprocalB );
                if( !splitOccured )
                {
                    a = this->mul( reciprocalA );
                    return false;
                }
                else
                {
                    a = this->mul( reciprocalA );
                    b = this->mul( reciprocalB );
					
                    if( a == b )
                    {
                        return false;
                    }
                    else
                    {
                        return true;
                    }
					
                }
            }    // !rhs.unbounded
			
        }    // !pointinterval 0
	}

template<typename Number>
Interval<Number> Interval<Number>::inverse() const
	{
		return Interval<Number>( -mContent.upper(), mUpperBoundType, -mContent.lower(), mLowerBoundType );
	}

template<typename Number>
void Interval<Number>::inverse_assign()
	{
		*this = this->inverse();
	}

template<typename Number>
bool Interval<Number>::reciprocal(Interval<Number>& a, Interval<Number>& b) const
	{
		if( this->isUnbounded() )
        {
            a = emptyInterval();
            return false;
        }
        else if( this->contains( 0 ) && mContent.lower() != 0 && mContent.upper() != 0 )
        {
            if( mLowerBoundType == BoundType::INFTY )
            {
                a = Interval<Number>(Number(0), BoundType::INFTY,Number(0), BoundType::WEAK );
                b = Interval<Number>(BoostInterval( Number(1) ) /BoostInterval( mContent.upper() ), BoundType::WEAK, BoundType::INFTY );
            }
            else if( mUpperBoundType == BoundType::INFTY )
            {
                a = Interval<Number>(BoostInterval( Number(1) ) /BoostInterval( mContent.lower() ), BoundType::INFTY, BoundType::WEAK );
                b = Interval<Number>(Number(0), BoundType::WEAK,Number(0), BoundType::INFTY );
            }
            else if( mContent.lower() == 0 && mContent.upper() != 0 )
            {
                a = Interval<Number>(Number(0), BoundType::INFTY, Number(0), BoundType::INFTY );
                b = Interval<Number>(BoostInterval( Number(1) ) /BoostInterval( mContent.upper() ), BoundType::WEAK, BoundType::INFTY );
            }
            else if( mContent.lower() != 0 && mContent.upper() == 0 )
            {
                a = Interval<Number>(BoostInterval( Number(1) ) /BoostInterval( mContent.lower() ), BoundType::INFTY, BoundType::WEAK );
                b = unboundedInterval(); // todo: really the whole interval here?
            }
            else if( mContent.lower() == 0 && mContent.upper() == 0 )
            {
                a = unboundedInterval();
                return false;
            }
            else
            {
                a = Interval<Number>(BoostInterval( Number(1) ) /BoostInterval( mContent.lower() ), BoundType::INFTY, BoundType::WEAK );
                b = Interval<Number>(BoostInterval( Number(1) ) /BoostInterval( mContent.upper() ), BoundType::WEAK, BoundType::INFTY );
            }
            return true;
        }
        else
        {
            if( mLowerBoundType == BoundType::INFTY && mContent.upper() != 0 )
            {
                a = Interval<Number>(  Number(1) / mContent.upper() , mUpperBoundType,Number(0),  BoundType::WEAK );
            }
            else if( mLowerBoundType == BoundType::INFTY && mContent.upper() == 0 )
            {
                a = Interval<Number>(Number(0), BoundType::INFTY, Number(0), BoundType::WEAK );
            }
            else if( mUpperBoundType == BoundType::INFTY && mContent.lower() != 0 )
            {
                a = Interval<Number>(  0 , BoundType::WEAK, Number(1)  /  mContent.lower(), mLowerBoundType );
            }
            else if( mUpperBoundType == BoundType::INFTY && mContent.lower() == 0 )
            {
                a = Interval<Number>(Number(0), BoundType::WEAK,Number(0), BoundType::INFTY );
            }
            else if( mContent.lower() != 0 && mContent.upper() != 0 )
            {
                a = Interval<Number>(BoostInterval( Number(1) ) / mContent, mUpperBoundType, mLowerBoundType );
            }
            else if( mContent.lower() == 0 && mContent.upper() != 0 )
            {
                a = Interval<Number>(BoostInterval( Number(1) ) /BoostInterval( mContent.upper() ), mUpperBoundType, BoundType::INFTY );
            }
            else if( mContent.lower() != 0 && mContent.upper() == 0 )
            {
                a = Interval<Number>(BoostInterval( Number(1) ) /BoostInterval( mContent.lower() ), BoundType::INFTY, mLowerBoundType );
            }
			
            return false;
        }
	}

template<typename Number>
Interval<Number> Interval<Number>::power(unsigned exp) const
	{
		assert(exp <= INT_MAX );
        if( exp % 2 == 0 )
        {
            if( mLowerBoundType == BoundType::INFTY && mUpperBoundType == BoundType::INFTY )
            {
                return unboundedInterval();
            }
            else if( mLowerBoundType == BoundType::INFTY )
            {
                if( contains( 0 ) )
                {
                    return Interval<Number>( Number(0), BoundType::WEAK, Number(0), BoundType::INFTY );
                }
                else
                {
                    return Interval<Number>( boost::numeric::pow( mContent, (int)exp ), mUpperBoundType, BoundType::INFTY );
                }
            }
            else if( mUpperBoundType == BoundType::INFTY )
            {
                if( contains( 0 ) )
                {
                    return Interval<Number>( Number(0), BoundType::WEAK, Number(0), BoundType::INFTY );
                }
                else
                {
                    return Interval<Number>( boost::numeric::pow( mContent, (int)exp ), mLowerBoundType, BoundType::INFTY );
                }
            }
            else
            {
                BoundType rType = mUpperBoundType;
                BoundType lType = mLowerBoundType;
                if( abs( mContent.lower() ) > abs( mContent.upper() ) )
                {
                    rType = mLowerBoundType;
                    lType = mUpperBoundType;
                }
                if( contains( 0 ) )
                {
                    return Interval<Number>( boost::numeric::pow( mContent, (int)exp ), BoundType::WEAK, rType );
                }
                else
                {
                    return Interval<Number>( boost::numeric::pow( mContent, (int)exp ), lType, rType );
                }
            }
        }
        else
        {
            return Interval<Number>( boost::numeric::pow( mContent, (int)exp ), mLowerBoundType, mUpperBoundType );
        }

	}

template<typename Number>
void Interval<Number>::power_assign(unsigned exp)
	{
		*this = this->power(exp);
	}

template<typename Number>
Interval<Number> Interval<Number>::sqrt() const
	{
		return Interval<Number>(boost::numeric::sqrt(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::sqrt_assign()
	{
		*this = this->sqrt();
	}

	
template<typename Number>
Interval<Number> Interval<Number>::root(unsigned deg) const
	{
		return Interval<Number>(boost::numeric::nth_root(mContent, deg), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::root_assign(unsigned deg)
	{
		*this = this->root(deg);
	}

template<typename Number>
Interval<Number> Interval<Number>::log() const
	{
		assert( mContent.lower() > 0 );
        return Interval<Number>(boost::numeric::log(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::log_assign()
	{
		mContent = boost::numeric::log(mContent);
	}

/*******************************************************************************
 * Trigonometric functions
 ******************************************************************************/

template<typename Number>
Interval<Number> Interval<Number>::sin() const
	{
		return Interval<Number>(boost::numeric::sin(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::sin_assign()
	{
		mContent = boost::numeric::sin(mContent);
	}

template<typename Number>
Interval<Number> Interval<Number>::cos() const
	{
		return Interval<Number>(boost::numeric::cos(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::cos_assign()
	{
		mContent = boost::numeric::cos(mContent);
	}

template<typename Number>
Interval<Number> Interval<Number>::tan() const
	{
		return Interval<Number>(boost::numeric::tan(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::tan_assign()
	{
		mContent = boost::numeric::tan(mContent);
	}

template<typename Number>
Interval<Number> Interval<Number>::asin() const
	{
		return Interval<Number>(boost::numeric::asin(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::asin_assign()
	{
		mContent = boost::numeric::asin(mContent);
	}

template<typename Number>
Interval<Number> Interval<Number>::acos() const
	{
		return Interval<Number>(boost::numeric::acos(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::acos_assign()
	{
		mContent = boost::numeric::acos(mContent);
	}

template<typename Number>
Interval<Number> Interval<Number>::atan() const
	{
		return Interval<Number>(boost::numeric::atan(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::atan_assign()
	{
		mContent = boost::numeric::atan(mContent);
	}

template<typename Number>
Interval<Number> Interval<Number>::sinh() const
	{
		return Interval<Number>(boost::numeric::sinh(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::sinh_assign()
	{
		mContent = boost::numeric::sinh(mContent);
	}

template<typename Number>
Interval<Number> Interval<Number>::cosh() const
	{
		return Interval<Number>(boost::numeric::cosh(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::cosh_assign()
	{
		mContent = boost::numeric::cosh(mContent);
	}

template<typename Number>
Interval<Number> Interval<Number>::tanh() const
	{
		return Interval<Number>(boost::numeric::tanh(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::tanh_assign()
	{
		mContent = boost::numeric::tanh(mContent);
	}

template<typename Number>
Interval<Number> Interval<Number>::asinh() const
	{
		return Interval<Number>(boost::numeric::asinh(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::asinh_assign()
	{
		mContent = boost::numeric::asinh(mContent);
	}

template<typename Number>
Interval<Number> Interval<Number>::acosh() const
	{
		return Interval<Number>(boost::numeric::acosh(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::acosh_assign()
	{
		mContent = boost::numeric::acosh(mContent);
	}

template<typename Number>
Interval<Number> Interval<Number>::atanh() const
	{
		return Interval<Number>(boost::numeric::atanh(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::atanh_assign()
	{
		mContent = boost::numeric::atanh(mContent);
	}

/*******************************************************************************
 * Overloaded arithmetics operators
 ******************************************************************************/

template<typename Number>
inline const Interval<Number> operator +(const Interval<Number>& lhs, const Interval<Number>& rhs)
	{
		return lhs.add(rhs);
	}

template<typename Number>
inline const Interval<Number> operator +(const Number& lhs, const Interval<Number>& rhs)
	{
		return Interval<Number>(lhs + rhs, rhs.lowerBoundType(), rhs.UpperBoundType());
	}

template<typename Number>
inline const Interval<Number> operator +(const Interval<Number>& lhs, const Number& rhs)
	{
		return rhs + lhs;
	}

template<typename Number>
inline const Interval<Number> operator -(const Interval<Number>& lhs, const Interval<Number>& rhs)
	{
		return lhs.sub(rhs);
	}

template<typename Number>
inline const Interval<Number> operator -(const Number& lhs, const Interval<Number>& rhs)
	{
		return Interval<Number>(lhs).sub_assign(rhs);
	}

template<typename Number>
inline const Interval<Number> operator -(const Interval<Number>& lhs, const Number& rhs)
	{
		return lhs.sub(Interval<Number>(rhs));
	}

template<typename Number>
inline const Interval<Number> operator *(const Interval<Number>& lhs, const Interval<Number>& rhs)
	{
		return lhs.mul(rhs);
	}

template<typename Number>
inline const Interval<Number> operator *(const Number& lhs, const Interval<Number>& rhs)
	{
		return Interval<Number>( lhs * rhs, rhs.lowerBoundType(), rhs.upperBoundType());
	}

template<typename Number>
inline const Interval<Number> operator *(const Interval<Number>& lhs, const Number& rhs)
	{
		return rhs * lhs;
	}

template<typename Number>
inline const Interval<Number> operator /(const Interval<Number>& lhs, const Interval<Number>& rhs)
	{
		return lhs.div(rhs);
	}

template<typename Number>
inline const Interval<Number> operator /(const Number& lhs, const Interval<Number>& rhs)
	{
		return Interval<Number>(lhs).div_assign(rhs);
	}

template<typename Number>
inline const Interval<Number> operator /(const Interval<Number>& lhs, const Number& rhs)
	{
		return lhs.div(Interval<Number>(rhs));
	}

/*******************************************************************************
 * Comparison operators
 ******************************************************************************/

template<typename Number>
inline bool operator ==(const Interval<Number>& lhs, const Interval<Number>& rhs)
	{
		return lhs.content().lower() == rhs.content().lower() && lhs.content().upper() == rhs.content().upper() && lhs.lowerBoundType() == rhs.lowerBoundType() && lhs.upperBoundType() == rhs.upperBoundType();
	}

template<typename Number>
inline bool operator !=(const Interval<Number>& lhs, const Interval<Number>& rhs)
	{
		return !(lhs == rhs);
	}

template<typename Number>
inline bool operator <=(const Interval<Number>& lhs, const Interval<Number>& rhs)
	{
		if( lhs < rhs )
		{
			return true;
		}
		
		if( lhs.upper() == rhs.lower() )
		{
			switch (lhs.upperBoundType()) {
				case BoundType::STRICT:
				case BoundType::WEAK:
					return rhs.lowerBoundType() != BoundType::INFTY;
				default:
					return false;
			}
		}
		else
		{
			return false;
		}
		
	}

template<typename Number>
inline bool operator >=(const Interval<Number>& lhs, const Interval<Number>& rhs)
	{
		return rhs <= lhs;
	}

template<typename Number>
inline bool operator <(const Interval<Number>& lhs, const Interval<Number>& rhs)
	{
		if( lhs.upper() < rhs.lower() )
		{
			return true;
		}
		
		if( lhs.upper() == rhs.lower() )
		{
			switch (lhs.upperBoundType()) {
				case BoundType::STRICT:
					return rhs.lowerBoundType() != BoundType::INFTY;
				case BoundType::WEAK:
					return rhs.lowerBoundType() == BoundType::STRICT;
				default:
					return false;
			}
		}
		else
		{
			return false;
		}
	}

template<typename Number>
inline bool operator >(const Interval<Number>& lhs, const Interval<Number>& rhs)
	{
		return rhs < lhs;
	}
	
/*******************************************************************************
 * Other operators
 ******************************************************************************/

template<typename Number>
inline std::ostream& operator <<(std::ostream& str, const Interval<Number>& i)
	{
		str << i.toString();
		return str;
	}
	
}