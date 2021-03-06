#pragma once

#include "../../../core/logging.h"
#include "../../../core/rootfinder/RootFinder.h"
#include "../../../core/Variable.h"
#include "../../../numbers/numbers.h"
#include "../ran/RealAlgebraicNumber.h"
#include "../ran/RealAlgebraicNumberEvaluation.h"

#include <boost/optional.hpp>

#include <algorithm>
#include <iostream>
#include <tuple>

namespace carl {

/**
 * @file
 * Represent a dynamic root, also known as a "root expression".
 * It's a generalization of an algebraic real(= univariate poly having roots + root index).
 * It uses a multivariate poly with distinguished, root-variable "_z"
 * and an root index. If we want to "evaluate" this root, we need to
 * plug in a subpoint with algebraic-real values for all variables that are not "_z".
 * The result is basically an algebraic real.
 * And since this algebraic real depends on the subpoint,
 * you can also think of this as a multidimensional scalar function:
 * A root(p(x1,x2,_z), 1) value represents a function f(x1,x2) of algReal^2 to
 * an algebraic real. For example, f(a,b)= root(q(_z),1), where the resulting
 * algebraic real is the 1. root of poly q(_z) :=  p(a,b,_z) after pluggin a and b into poly p.
 */
template<typename Poly>
class MultivariateRoot {
public:
	using Number = typename UnderlyingNumberType<Poly>::type;
	using RAN = RealAlgebraicNumber<Number>;
	using EvalMap = RealAlgebraicNumberEvaluation::RANMap<Number>;
private:
  /**
   * Distinguished, globally unique root-variable
   */
	static const Variable sVar;
	Poly mPoly;
	std::size_t mK; // also called rootId, rootNumber or rootIndex
public:
  /**
   * @param poly Must mention the root-variable "_z" and
   * should have a at least 'rootIdx'-many roots in "_z" at each subpoint
   * where it is intended to be evaluated.
   * @param rootIdx The index of the root of the polynomial in "_z".
   * The first root has index 1, the second has index 2 and so on.
   */
	MultivariateRoot(const Poly& poly, std::size_t rootIdx): mPoly(poly), mK(rootIdx)
	{
		assert(rootIdx > 0);
//		assert(poly.gatherVariables().count(sVar) == 1);
	}

//	/**
//   * @param poly Must mention the root-variable "_z" and
//   * should have a at least 'rootIdx'-many roots in "_z" at each subpoint
//   * where it is intended to be evaluated.
//   * @param rootIdx The index of the root of the polynomial in "_z".
//   * @param mainPolyVar Must be a variable that appears with non-zero coefficient
//   * to be replaced by "_z".
//   */
//	MultivariateRoot(const Poly& poly, Variable mainPolyVar, std::size_t rootIdx): mPoly(poly.substitutue(mainPolyVar,Poly(sVar))), mK(rootIdx)
//	{
//		assert(rootIdx > 0);
////    assert(poly.gatherVariables().count(sVar) == 1);
//	}

	/**
	 * Return the rootIndex, also know as k, rootNumber, rootId
	 */
  [[deprecated("Use rootIdx() instead for better semantic meaning")]]
	std::size_t k() const noexcept {
		return mK;
	}

  /**
   * @return the rootIndex, also know as k, rootNumber, rootId
   */
  std::size_t rootIdx() const noexcept {
    return mK;
  }


  /**
   * @return the raw underlying polynomial that still mentions the root-variable "_z".
   */
	const Poly& poly() const noexcept {
		return mPoly;
	}

	/**
	 * @return A copy of the underlying polynomial with the
	 * root-variable replaced by the given variable.
	 */
	Poly poly(Variable var) const {
		return mPoly.substitute(sVar, Poly(var));
	}

	/**
	 * @return The globally-unique distinguished root-variable "_z"
	 * to allow you to build a polynomial with this variable yourself.
	 */
  [[deprecated("Use uniqRootVar() instead for better semantic meaning")]]
	static Variable var() noexcept {
		return sVar;
	}

  /**
   * @return The globally-unique, distinguished root-variable "_z"
   * to allow you to build a polynomial with this variable yourself.
   */
  static Variable uniqRootVar() noexcept {
    return sVar;
  }

	bool isUnivariate() const {
		return mPoly.isUnivariate();
	}

  /**
   * @return The variables mentioned in underlying polynomial, excluding
   * the root-variable "_z". For example, with an underlying poly p(x,y,_z)
   * we return {x,y}.
   */
	std::set<Variable> gatherVariables() const {
		Variables var = mPoly.gatherVariables();
		var.erase(sVar);
		return var;
	}

	/**
	 * Create a copy of the underlying polynomial with the given variable
	 * replaced by the given polynomial.
	 */
	void substituteIn(Variable var, const Poly& poly) {
		mPoly.substituteIn(var, poly);
	}

	/**
	 * Return the emerging algebraic real after pluggin in a subpoint to replace
	 * all variables with algebraic reals that are not the root-variable "_z".
	 * @param m must contain algebraic real assignments for all variables that are not "_z".
	 * @return boost::none if the underlying polynomial has no root with index 'rootIdx' at
	 * the given subpoint.
	 */
	boost::optional<RAN> evaluate(const EvalMap& m) const {
		CARL_LOG_DEBUG("carl.rootexpression", "Evaluate: " << *this << " against: " << m);
		auto poly = mPoly.toUnivariatePolynomial(sVar);
		auto roots = rootfinder::realRoots(poly, m);
		if (!roots || (roots->size() < mK)) {
			CARL_LOG_TRACE("carl.rootexpression", mK << "th root does not exist.");
			return boost::none;
		}
		CARL_LOG_TRACE("carl.rootexpression", "Take " << mK << "th of isolated roots " << *roots);
		assert(roots->size() >= mK);
		assert(mK > 0);
		CARL_LOG_DEBUG("carl.rootexpression", "Result is " << (*roots)[mK-1]);
		return (*roots)[mK-1];
	}
};

template<typename Poly>
const Variable MultivariateRoot<Poly>::sVar = carl::VariablePool::getInstance().getFreshPersistentVariable("__z");

template<typename Poly>
inline bool operator==(const MultivariateRoot<Poly>& lhs, const MultivariateRoot<Poly>& rhs) {
	return std::forward_as_tuple(lhs.rootIdx(), lhs.poly()) == std::forward_as_tuple(rhs.rootIdx(), rhs.poly());
}
template<typename Poly>
inline bool operator<(const MultivariateRoot<Poly>& lhs, const MultivariateRoot<Poly>& rhs) {
	return std::forward_as_tuple(lhs.rootIdx(), lhs.poly()) < std::forward_as_tuple(rhs.rootIdx(), rhs.poly());
}

template<typename P>
std::ostream& operator<<(std::ostream& os, const MultivariateRoot<P>& mr) {
	return os << "rootExpr(" << mr.poly() << ", " << mr.rootIdx() << ", " << MultivariateRoot<P>::uniqRootVar() << ")";
}

}

namespace std {
	template<typename Pol>
	struct hash<carl::MultivariateRoot<Pol>> {
		std::size_t operator()(const carl::MultivariateRoot<Pol>& mv) const {
			return carl::hash_all(mv.poly(), mv.rootIdx());
		}
	};
}
