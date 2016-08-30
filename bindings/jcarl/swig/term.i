
%module ModTerm 
%{
#include <carl/core/Term.h>
#include <carl/core/MultivariatePolynomial.h>
#include <carl/core/RationalFunction.h>
#include "gmp.h"
#include "gmpxx.h"

typedef mpq_class Rational;


typedef carl::Term<Rational> Term;

typedef carl::MultivariatePolynomial<Rational> Polynomial;
typedef carl::RationalFunction<Polynomial> RationalFunctionPoly;

%}


%include "std_string.i"
%import "monomial.i"
%import "rational.i"
%import "polynomial.i"
%import "rationalfunction.i"

typedef carl::MultivariatePolynomial<Rational> Polynomial;
typedef carl::RationalFunction<Polynomial> RationalFunctionPoly;


namespace carl {


template<typename Coefficient>
class Term {
 public:
  Term(const Coefficient& c, const carl::Monomial::Arg& m);
  
  Coefficient& coeff();

  const carl::Monomial& monomial() const;
  std::string toString(bool infix=true, bool friendlyVarNames=true) const;

  carl::Term<Coefficient> pow(uint exp) const;
  

  %extend {
   Polynomial add(const carl::Term<Coefficient>& rhs) {
	return *($self)+rhs;
   } 	

   Polynomial add(const Polynomial& rhs) {
 	return *($self)+rhs;
   } 

   Polynomial add(const Monomial::Arg& rhs) {
 	return *($self)+rhs;
   } 

   Polynomial add(carl::Variable::Arg rhs) {
 	return *($self)+rhs;
   } 

   Polynomial add(const Coefficient& rhs) {
 	return *($self)+rhs;
   } 


   Polynomial sub(const carl::Term<Coefficient>& rhs) {
	return *($self)-rhs;
   } 	

   Polynomial sub(const Polynomial& rhs) {
 	return *($self)-rhs;
   } 

   Polynomial sub(const Monomial::Arg& rhs) {
 	return *($self)-rhs;
   } 

   Polynomial sub(carl::Variable::Arg rhs) {
 	return *($self)-rhs;
   } 

   Polynomial sub(const Coefficient& rhs) {
 	return *($self)-rhs;
   } 


   carl::Term<Coefficient> mul(const carl::Term<Coefficient>& rhs) {
	return *($self)*rhs;
   } 	

   Polynomial mul(const Polynomial& rhs) {
 	return *($self)*rhs;
   } 

   carl::Term<Coefficient> mul(const Monomial::Arg& rhs) {
 	return *($self)*rhs;
   } 

   carl::Term<Coefficient> mul(carl::Variable::Arg rhs) {
 	return *($self)*rhs;
   } 

   carl::Term<Coefficient> mul(const Coefficient& rhs) {
 	return *($self)*rhs;
   } 


   RationalFunctionPoly div(const RationalFunctionPoly& rhs) {
	return RationalFunctionPoly(Polynomial(*($self))) / rhs;
   }

   RationalFunctionPoly div(const Polynomial& rhs) {
	return RationalFunctionPoly(Polynomial(*($self))) / rhs;
   }

   RationalFunctionPoly div(const carl::Term<Coefficient>& rhs) {
	return RationalFunctionPoly(Polynomial(*($self))) / rhs;
   }

   RationalFunctionPoly div(const Monomial::Arg& rhs) {
	return RationalFunctionPoly(Polynomial(*($self))) / rhs;
   }

   RationalFunctionPoly div(carl::Variable::Arg rhs) {
	return RationalFunctionPoly(Polynomial(*($self))) / rhs;
   }

   carl::Term<Coefficient> neg() {
	return *$self*Coefficient(-1);
   }

     

  }
};
}


%template(JTerm) carl::Term<Rational>;
