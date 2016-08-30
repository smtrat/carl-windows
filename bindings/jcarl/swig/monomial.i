


%module ModMonomial
%{
#include <carl/core/Monomial.h>
#include <carl/core/Term.h>
#include <carl/core/MultivariatePolynomial.h>
#include <carl/core/RationalFunction.h>
#include "gmp.h"
#include "gmpxx.h"
typedef mpq_class Rational;


typedef unsigned int uint;
typedef std::pair<carl::Variable,uint> VarIntPair;



typedef carl::Term<Rational> Term;

typedef carl::MultivariatePolynomial<Rational> Polynomial;
typedef carl::RationalFunction<Polynomial> RationalFunctionPoly;


%}

%include "std_string.i"
//TODO: for 32 bit support this has to be changed..
//%import <stddef> //for size_t maybe?
typedef long unsigned int size_t;  //this should be okay for 64 bits at least

typedef unsigned int uint;

%import "variable.i"
%import "term.i"
%import "polynomial.i"
%import "rationalfunction.i"

typedef std::pair<carl::Variable,uint> VarIntPair;
%include "std_pair.i"
%include "std_vector.i"
%include "std_shared_ptr.i"
%shared_ptr(carl::Monomial);


typedef carl::Term<Rational> Term;

typedef carl::MultivariatePolynomial<Rational> Polynomial;
typedef carl::RationalFunction<Polynomial> RationalFunctionPoly;



namespace std {
 %template(VarIntPair) pair<carl::Variable,uint>;
 %template(VarIntPairVector) vector<VarIntPair>;
}




namespace carl {

typedef unsigned int exponent;





class Monomial {
public:
typedef std::shared_ptr<const carl::Monomial> Arg;
typedef std::vector<VarIntPair> Content;
/*%extend {
 Monomial(carl::Variable v, carl::exponent e) {
    auto m = carl::MonomialPool::getInstance().create(v, e);
    return m;
 }
} */

explicit Monomial(Variable::Arg v, carl::exponent e = 1) :
	mExponents(1, std::make_pair(v,e)),
	mTotalDegree(e)
{
}


 carl::exponent tdeg() const;

const carl::Monomial::Content& exponents() const;


std::string toString(bool infix = true, bool friendlyVarNames = true) const;

size_t nrVariables() const;



%extend {

	Polynomial add(const Polynomial& rhs) {
	    const std::shared_ptr<const carl::Monomial> ptr($self);
	    return ptr+rhs;
	}

	Polynomial add(const Term& rhs) {
	    const std::shared_ptr<const carl::Monomial> ptr($self);
	    return ptr+rhs;
	}

	Polynomial add(const Monomial::Arg& rhs) {
	    const std::shared_ptr<const carl::Monomial> ptr($self);
	    return carl::operator+<Rational,carl::GrLexOrdering,carl::StdMultivariatePolynomialPolicies<>>(ptr,rhs);
	} 

	Polynomial add(carl::Variable::Arg rhs) {
	    const std::shared_ptr<const carl::Monomial> ptr($self);
	    return carl::operator+<Rational,carl::GrLexOrdering,carl::StdMultivariatePolynomialPolicies<>>(ptr,rhs);
	} 

	Polynomial add(const Rational& rhs) {
	    const std::shared_ptr<const carl::Monomial> ptr($self);
	    return ptr+rhs;
	}




	Polynomial sub(const Polynomial& rhs) {
	    const std::shared_ptr<const carl::Monomial> ptr($self);
	    return ptr-rhs;
	}

	Polynomial sub(const Term& rhs) {
	    const std::shared_ptr<const carl::Monomial> ptr($self);
	    return ptr-rhs;
	}

	Polynomial sub(const Monomial::Arg& rhs) {
	    const std::shared_ptr<const carl::Monomial> ptr($self);
	    return carl::operator-<Rational,carl::GrLexOrdering,carl::StdMultivariatePolynomialPolicies<>>(ptr,rhs);
	} 

	Polynomial sub(carl::Variable::Arg rhs) {
	    const std::shared_ptr<const carl::Monomial> ptr($self);
	    return carl::operator-<Rational,carl::GrLexOrdering,carl::StdMultivariatePolynomialPolicies<>>(ptr,rhs);
	} 

	Polynomial sub(const Rational& rhs) {
	    const std::shared_ptr<const carl::Monomial> ptr($self);
	    return ptr-rhs;
	}



	Polynomial mul(const Polynomial& rhs) {
	    const std::shared_ptr<const carl::Monomial> ptr($self);
	    return ptr*rhs;
	}

	Term mul(const Term& rhs) {
	    const std::shared_ptr<const carl::Monomial> ptr($self);
	    return ptr*rhs;
	}

	Polynomial mul(const Monomial::Arg& rhs) {
	    const std::shared_ptr<const carl::Monomial> ptr1($self);
	    const std::shared_ptr<const carl::Monomial> ptr2(rhs);
	    return carl::operator*(ptr1,Polynomial(ptr2));
	} 

	Polynomial mul(carl::Variable::Arg rhs) {
	    const std::shared_ptr<const carl::Monomial> ptr($self);
	    return carl::operator*(ptr,Polynomial(rhs));
	}  

	Term mul(const Rational& rhs) {
	    const std::shared_ptr<const carl::Monomial> ptr($self);
	    return ptr*rhs;
	}


	RationalFunctionPoly div(const RationalFunctionPoly& rhs) {
	const std::shared_ptr<const carl::Monomial> ptr($self);
		return RationalFunctionPoly(Polynomial(ptr)) / rhs;
	}

	RationalFunctionPoly div(const Polynomial& rhs) {
	const std::shared_ptr<const carl::Monomial> ptr($self);
		return RationalFunctionPoly(Polynomial(ptr)) / rhs;
	}

	RationalFunctionPoly div(const Term& rhs) {
	const std::shared_ptr<const carl::Monomial> ptr($self);
		return RationalFunctionPoly(Polynomial(ptr)) / rhs;
	}

	RationalFunctionPoly div(const Monomial::Arg& rhs) {
	const std::shared_ptr<const carl::Monomial> ptr($self);
		return RationalFunctionPoly(Polynomial(ptr)) / rhs;
	}

	RationalFunctionPoly div(carl::Variable::Arg rhs) {
	const std::shared_ptr<const carl::Monomial> ptr($self);
		return RationalFunctionPoly(Polynomial(ptr)) / rhs;
	}

	Term div(const Rational& rhs) {
	const std::shared_ptr<const carl::Monomial> ptr($self);
		return Term(ptr) / rhs;
	}

	Term neg() {
		const std::shared_ptr<const carl::Monomial> ptr($self);
		return ptr*Rational(-1);
	}

	carl::Monomial::Arg pow(uint exp) {
		return $self->pow(exp);
	} 



	VarIntPair getItem(std::size_t index) {
		return *($self->begin()+index);
	}

}


};

}
