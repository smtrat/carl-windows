/**
 * @file PolynomialParser.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "Common.h"

namespace boost { namespace spirit { namespace traits {
    template<> inline void scale(int exp, cln::cl_RA& r) {
        if (exp >= 0)
            r *= carl::pow(cln::cl_RA(10), (unsigned)exp);
        else
            r /= carl::pow(cln::cl_RA(10), (unsigned)(-exp));
    }

    template<> inline void scale(int exp, mpq_class& r) {
        if (exp >= 0)
            r *= carl::pow(mpq_class(10), (unsigned)exp);
        else
            r /= carl::pow(mpq_class(10), (unsigned)(-exp));
    }

}}}

namespace carl {
namespace parser {



template<typename Coeff>
struct ExpressionParser: public qi::grammar<Iterator,
		boost::variant< Coeff, Variable, Monomial::Arg, Term<Coeff>, Poly<Coeff>, RatFun<Coeff> >(),
		Skipper> {
	using expr_type = boost::variant< Coeff, Variable, Monomial::Arg, Term<Coeff>, Poly<Coeff>, RatFun<Coeff> >;

	class perform_addition: public boost::static_visitor<expr_type> {
	public:
		template<typename T, typename U>
		expr_type operator()(const T& lhs, const U& rhs) const {
			return Poly<Coeff>(lhs) + Poly<Coeff>(rhs);
		}

		expr_type operator()(const Coeff& lhs, const Coeff& rhs) const {
			return (lhs) + (rhs);
		}

		expr_type operator()(const RatFun<Coeff>& lhs, const Monomial::Arg& rhs) const {
			return (lhs) + Poly<Coeff>(rhs);
		}

		expr_type operator()(const RatFun<Coeff>& lhs, const Term<Coeff>& rhs) const {
			return (lhs) + Poly<Coeff>(rhs);
		}

		template<typename T>
		expr_type operator()(const RatFun<Coeff>& lhs, const T& rhs) const {
			return (lhs) + (rhs);
		}

		template<typename T>
		expr_type operator()(const T& lhs, const RatFun<Coeff>& rhs) const {
			return (rhs) + Poly<Coeff>(lhs);
		}

		expr_type operator()(const RatFun<Coeff>& lhs, const RatFun<Coeff>& rhs) const {
			return (lhs) + (rhs);
		}
	};

	class perform_subtraction: public boost::static_visitor<expr_type> {
	public:
		template<typename T, typename U>
		expr_type operator()(const T& lhs, const U& rhs) const {
			return Poly<Coeff>(lhs) - Poly<Coeff>(rhs);
		}

		expr_type operator()(const Coeff& lhs, const Coeff& rhs) const {
			return (lhs) - (rhs);
		}

		expr_type operator()(const RatFun<Coeff>& lhs, const Monomial::Arg& rhs) const {
			return (lhs) - Poly<Coeff>(rhs);
		}

		expr_type operator()(const RatFun<Coeff>& lhs, const Term<Coeff>& rhs) const {
			return (lhs) - Poly<Coeff>(rhs);
		}

		template<typename T>
		expr_type operator()(const RatFun<Coeff>& lhs, const T& rhs) const {
			return (lhs) - (rhs);
		}

		template<typename T>
		expr_type operator()(const T& lhs, const RatFun<Coeff>& rhs) const {
			return (rhs) - Poly<Coeff>(lhs);
		}

		expr_type operator()(const RatFun<Coeff>& lhs, const RatFun<Coeff>& rhs) const {
			return (lhs) - (rhs);
		}
	};

	class perform_multiplication: public boost::static_visitor<expr_type> {
	public:
		template<typename T, typename U>
		expr_type operator()(const T& lhs, const U& rhs) const {
			return lhs * rhs;
		}

		template<typename T>
		expr_type operator()(const T& lhs, const RatFun<Coeff>& rhs) const {
			return rhs * lhs;
		}

		expr_type operator()(const RatFun<Coeff>& lhs, const Monomial::Arg& rhs) const {
			return (lhs) * Poly<Coeff>(rhs);
		}

		expr_type operator()(const RatFun<Coeff>& lhs, const Term<Coeff>& rhs) const {
			return (lhs) * Poly<Coeff>(rhs);
		}

		expr_type operator()(const Monomial::Arg& lhs, const RatFun<Coeff>& rhs) const {
			return (rhs) * Poly<Coeff>(lhs);
		}

		expr_type operator()(const Term<Coeff>& lhs, const RatFun<Coeff>& rhs) const {
			return (rhs) * Poly<Coeff>(lhs);
		}
	};

	class perform_division: public boost::static_visitor<expr_type> {
	public:
		expr_type operator()(const RatFun<Coeff>& lhs, const Coeff& rhs) const {
			return lhs / rhs;
		}

		template<typename T>
		expr_type operator()(const RatFun<Coeff>& lhs, const T& rhs) const {
			return lhs / rhs;
		}

		expr_type operator()(const RatFun<Coeff>& lhs, const Monomial::Arg& rhs) const {
			return lhs / Poly<Coeff>(rhs);
		}

		expr_type operator()(const RatFun<Coeff>& lhs, const Term<Coeff>& rhs) const {
			return lhs / Poly<Coeff>(rhs);
		}

		expr_type operator()(const RatFun<Coeff>& lhs, const RatFun<Coeff>& rhs) const {
			return lhs / rhs;
		}

		template<typename T>
		expr_type operator()(const T& lhs, const Coeff& coeff) const {
			return lhs * reciprocal(coeff);
		}

		template<typename T>
		expr_type operator()(const T& lhs, const RatFun<Coeff>& rhs) const {
			// TODO: Not extremely efficient probably
			return  RatFun<Coeff>(rhs.denominator(), rhs.nominator()) * Poly<Coeff>(lhs);
		}

		template<typename T, typename U>
		expr_type operator()(const T& lhs, const U& rhs) const {
			return RatFun<Coeff>(Poly<Coeff>(lhs), Poly<Coeff>(rhs));
		}
	};

	class perform_power: public boost::static_visitor<expr_type> {
	public:
		exponent expVal;

		perform_power(exponent exp) : expVal(exp) {};

		template<typename T>
		expr_type operator()(const T& lhs) const {
			return lhs.pow(expVal);
		}

		expr_type operator()(const RatFun<Coeff>& lhs) const {
			return RatFun<Coeff>(lhs.nominator().pow(expVal), lhs.denominator().pow(expVal));
		}

		expr_type operator()(const Coeff& lhs) const {
			return carl::pow(lhs, expVal);
		}

		expr_type operator()(const Variable& lhs) const {
			return createMonomial(lhs, expVal);
		}

		expr_type operator()(const Monomial::Arg& lhs) const {
			return lhs->pow(expVal);
		}
	};

	class perform_negate: public boost::static_visitor<expr_type> {
	public:
		template<typename T>
		expr_type operator()(const T& lhs) const {
			return lhs * Coeff(-1.0);
		}
	};

	class to_poly: public boost::static_visitor<Poly<Coeff>> {
	public:
		Poly<Coeff> operator()(const Poly<Coeff>& p) const {
			return p;
		}

		Poly<Coeff> operator()(const RatFun<Coeff>& p) const {
			throw std::runtime_error("Cannot make polynomial out of rational function");
		}

		template<typename T>
		Poly<Coeff> operator()(const T& expr) const {
			return Poly<Coeff>(expr);
		}
	};

	class to_ratfun: public boost::static_visitor<RatFun<Coeff>> {
	public:
		RatFun<Coeff> operator()(const RatFun<Coeff>& expr) const {
			return expr;
		}

		RatFun<Coeff> operator()(const Monomial::Arg& expr) const {
			return RatFun<Coeff>(Poly<Coeff>(expr));
		}

		RatFun<Coeff> operator()(const Term<Coeff>& expr) const {
			return RatFun<Coeff>(Poly<Coeff>(expr));
		}

		template<typename T>
		RatFun<Coeff> operator()(const T& expr) const {
			return RatFun<Coeff>(expr);
		}
	};

	class print_expr_type: public boost::static_visitor<> {
	public:
		void operator()(const RatFun<Coeff>& expr) const {
			std::cout << "Rational function " << expr << std::endl;
		}

		void operator()(const Poly<Coeff>& expr) const {
			std::cout << "Polynomial " << expr << std::endl;
		}

		void operator()(const Term<Coeff>& expr) const {
			std::cout << "Term " << expr << std::endl;
		}

		void operator()(const Monomial::Arg& expr) const {
			std::cout << "Monomial " << expr << std::endl;
		}

		void operator()(const Coeff& expr) const {
			std::cout << "Coefficient " << expr << std::endl;
		}

		void operator()(const Variable& expr) const {
			std::cout << "Variable " << expr << std::endl;
		}
	};

	ExpressionParser(): ExpressionParser<Coeff>::base_type(main, "polynomial") {
		/** Tokens */
		operation.add("+", ADD)("-", SUB);
		operationScale.add("*", MUL)("/", DIV);
		operationPow.add("^", POW)("**", POW);
		operationSign.add("-", NEG);
		varname = qi::lexeme[ (qi::alpha | qi::char_("_")) >> *(qi::alnum | qi::char_("_"))];
		variable = varname[qi::_val = px::bind(&ExpressionParser<Coeff>::newVariable, px::ref(*this), qi::_1)];

		/** Rules */
		// operationScaleLA is a look-ahead hack to prevent the * operator from consuming ** (power)
		operationScaleLA = qi::lexeme[ operationScale >> !qi::lit("*") ][qi::_val = qi::_1];
		monomial = variable[qi::_val = qi::_1];
		atom = (monomial[qi::_val = qi::_1] | coeff[qi::_val = qi::_1]);
		expr = ("(" > expr_sum > ")")[qi::_val = qi::_1] | atom[qi::_val = qi::_1];
		expr_power = (expr >> *(operationPow > exponentVal))[qi::_val = px::bind(&ExpressionParser<Coeff>::powExpr, px::ref(*this), qi::_1, qi::_2)];
		expr_sign = (*operationSign > expr_power)[qi::_val = px::bind(&ExpressionParser<Coeff>::signExpr, px::ref(*this), qi::_1, qi::_2)];
		expr_product = (expr_sign >> *(operationScaleLA > expr_power))[qi::_val = px::bind(&ExpressionParser<Coeff>::mulExpr, px::ref(*this), qi::_1, qi::_2)];
		expr_sum = (expr_product >> *(operation > expr_product))[qi::_val = px::bind(&ExpressionParser<Coeff>::addExpr, px::ref(*this), qi::_1, qi::_2)];
		main = expr_sum;

		varname.name("varname");
		variable.name("variable");
		monomial.name("monomial");
		atom.name("atom");
		expr.name("expr");
		expr_sign.name("expr_sign");
		expr_power.name("expr_power");
		expr_product.name("expr_product");
		expr_sum.name("expr_sum");
		main.name("main");
	}
	
	void addVariable(Variable::Arg v) {
		auto s = VariablePool::getInstance().getName(v);
		varmap.add(s, v);
	}
	
private:
	enum Operation { ADD, SUB, MUL, DIV, POW, NEG };
	
	Variable newVariable(const std::string& s) {
		Variable* vptr = nullptr;
		if ((vptr = varmap.find(s)) != nullptr) {
			return *vptr;
		}
		Variable v = freshRealVariable(s);
		varmap.add(s, v);
		return v;
	}

	expr_type addExpr(const expr_type& first, const std::vector<boost::fusion::vector2<Operation,expr_type>>& ops) {
		expr_type res = first;
		for (const auto& op: ops) {
			switch (boost::fusion::at_c<0>(op)) {
				case ADD: {
					res = boost::apply_visitor( perform_addition(), res, boost::fusion::at_c<1>(op) );
					break;
				}
				case SUB: {
					res = boost::apply_visitor( perform_subtraction(), res, boost::fusion::at_c<1>(op) );
					break;
				}
			}
		}
		return res;
	}
	expr_type mulExpr(const expr_type& first, const std::vector<boost::fusion::vector2<Operation,expr_type>>& ops) {
		expr_type res = first;
		for (const auto& op: ops) {
			switch (boost::fusion::at_c<0>(op)) {
				case MUL: {
					res = boost::apply_visitor( perform_multiplication(), res, boost::fusion::at_c<1>(op) );
					break;
				}
				case DIV: {
					res = boost::apply_visitor( perform_division(), res, boost::fusion::at_c<1>(op) );
					break;
				}
			}
		}
		return res;
	}
	
	expr_type powExpr(const expr_type& first, const std::vector<boost::fusion::vector2<Operation,exponent>>& ops) {
		expr_type res = first;
		for (const auto& op: ops) {
			switch (boost::fusion::at_c<0>(op)) {
				case POW: {
					res = boost::apply_visitor( perform_power(boost::fusion::at_c<1>(op)), res );
					break;
				}
			}
		}
		return res;
	}

	expr_type signExpr(const std::vector<Operation>& ops, const expr_type& first) {
		expr_type res = first;
		for(auto op : ops) {
			switch (op) {
				case NEG: {
					res = boost::apply_visitor( perform_negate(), res );
					break;
				}
				default:
					throw std::runtime_error("Unknown unary operator");
			}
		}
		return res;
	}

	qi::symbols<char, Operation> operation;
	qi::symbols<char, Operation> operationScale;
	qi::symbols<char, Operation> operationPow;
	qi::symbols<char, Operation> operationSign;
	qi::symbols<char, Variable> varmap;

	qi::rule<Iterator, std::string(), Skipper> varname;
	qi::uint_parser<exponent,10,1,-1> exponentVal;
	qi::real_parser<Coeff,RationalPolicies<Coeff>> coeff;
	qi::rule<Iterator, Variable(), Skipper> variable;

	qi::rule<Iterator, Operation(), Skipper> operationScaleLA;

	qi::rule<Iterator, expr_type(), Skipper> monomial;
	qi::rule<Iterator, expr_type(), Skipper> atom;

	qi::rule<Iterator, expr_type(), Skipper> expr;
	qi::rule<Iterator, expr_type(), Skipper> expr_sign;
	qi::rule<Iterator, expr_type(), Skipper> expr_product;
	qi::rule<Iterator, expr_type(), Skipper> expr_power;
	qi::rule<Iterator, expr_type(), Skipper> expr_sum;
	qi::rule<Iterator, expr_type(), Skipper> main;
};

}
}
