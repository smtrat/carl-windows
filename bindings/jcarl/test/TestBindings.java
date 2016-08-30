import java.util.*;
import jcarl.*;

public class TestBindings {
  static {
    try {
        System.loadLibrary("jcarl");
    } catch (UnsatisfiedLinkError e) {
      System.err.println("Native code library failed to load. See the chapter on Dynamic Linking Problems in the SWIG Java documentation for help.\n" + e);
      System.exit(1);
    }
  }

  public static void main(String argv[]) 
  {
    // Test every class
    // Variable:
    	System.out.println("Test variable...");
	Variable v1 = new Variable("z",VariableType.VT_BOOL);
	Variable v2 = new Variable("x",VariableType.VT_REAL); 
	Variable v3 = new Variable(VariableType.VT_INT);
	System.out.println(v1.toString());
	System.out.println(v2.toString());
	System.out.println(v3.toString());
	System.out.println("Id,type and rank of v1:");
    	System.out.println(v1.getId());
	System.out.println(v1.getType());
	System.out.println(v1.getRank());
	System.out.println("v1 == v2 yields: "+v1.equals(v2));
	System.out.println("v1 == v1 yields: "+v1.equals(v1));
	System.out.println("v1 != v2 yields: "+v1.notEquals(v2));
	System.out.println("v1 != v1 yields: "+v1.notEquals(v1));
	System.out.println("v1 < v2 yields: "+v1.less(v2));
	System.out.println("v2 < v2 yields: "+v2.less(v2));
	System.out.println("v2 <= v2 yields: "+v2.lessEquals(v2));
	System.out.println("v3 > v1 yields: "+v3.greater(v1));
	System.out.println("v1 > v3 yields: "+v1.greater(v3));
	System.out.println("v3 > v3 yields: "+v3.greater(v3));
	System.out.println("v3 >= v3 yields: "+v3.greaterEquals(v3));

    //Interval:
	System.out.println("*****************************************************");
	System.out.println("                                      ");
	System.out.println("Test JInterval...");
	JInterval i1 = new JInterval(new Rational(-20.5),new Rational(3.4567));
	JInterval i2 = new JInterval(new Rational(0));
	JInterval i3 = new JInterval(new Rational(-100),BoundType.WEAK,new Rational(100),BoundType.INFTY);

	System.out.println(i1.toString());
	System.out.println(i2.toString());
	System.out.println(i3.toString());

	
        System.out.println("i1.lower: "+i1.lower());
	System.out.println("i1.upper: "+i1.upper());
	System.out.println("i3.isInfinite: "+i3.isInfinite());
	System.out.println("i1.isUnbounded: "+i1.isUnbounded());
	System.out.println("i1.isHalfBounded: "+i1.isHalfBounded());
        System.out.println("i1.isEmpty: "+i1.isEmpty());
	System.out.println("i1.isPointInterval: "+i1.isPointInterval());
	System.out.println("i2.isPointInterval: "+i2.isPointInterval());
	System.out.println("i1.isOpenInterval: "+i1.isOpenInterval());
	System.out.println("i1.isClosedInterval: "+i1.isClosedInterval());
	System.out.println("i1.isZero: "+i1.isZero());
	System.out.println("i1.isOne: "+i1.isOne());
	System.out.println("i1.isPositive: "+i1.isPositive());
	System.out.println("i1.isNegative: "+i1.isNegative());
	System.out.println("i1.isSemiPositive: "+i1.isSemiPositive());
	System.out.println("i1.isSemiNegative: "+i1.isSemiNegative());
	
	i2.setLower(new Rational(-10));
	i2.setUpper(new Rational(10));
 	System.out.println("New bounds for i2: "+i2.toString());
    

	System.out.println("i1.integralPart(): "+i1.integralPart().toString());
	System.out.println("i1.diameter(): "+i1.diameter().toString());
	System.out.println("i1.center(): "+i1.center().toString());
	System.out.println("i1.sample(): "+i1.sample().toString());
	System.out.println("i1 contains 1000: "+i1.contains(new Rational(1000)));
	System.out.println("i3 contains i2: "+i3.contains(i2));
	System.out.println("i1 meets -20.5: "+i1.meets(new Rational(-20.5)));
	System.out.println("i1 contains i2: "+i1.contains(i2));
	System.out.println("i1 isSubset i3: "+i1.isSubset(i3));
	System.out.println("i1 isProperSubset i1: "+i1.isProperSubset(i1));
	System.out.println("i3 div i2: "+i3.div(i2).toString());
	System.out.println("i3 inverse: "+i3.inverse().toString());
	System.out.println("i3 abs: "+i3.abs().toString());
	System.out.println("i2 pow 2: "+i2.pow(2).toString());
	System.out.println("i1 intersects with i3: "+i1.intersectsWith(i3));
	System.out.println("i1 intersect i2: "+i1.intersect(i2).toString());
	JInterval res1,res2;
	res1 = new JInterval(new Rational(0)); res2 = new JInterval(new Rational(0));
	i1.unite(i2,res1,res2);
	System.out.println("i1 unite i2: "+res1.toString()+" and "+res2.toString());
	i3.difference(i1,res1,res2);
	System.out.println("i3-i1: "+res1.toString()+" and "+res2.toString());
	i3.complement(res1,res2);
	System.out.println("i3 complement: "+res1.toString()+" and "+res2.toString());
	i3.symmetricDifference(i1,res1,res2);
	System.out.println("symmetric difference i3-i1: "+res1.toString()+" and "+res2.toString());
	System.out.println("floor i1: "+JInterval.floor(i1).toString());
	System.out.println("ceil i1: "+JInterval.ceil(i1).toString());
	System.out.println("quotient i1 i2: "+JInterval.quotient(i1,i2).toString());
	System.out.println("isInteger i1: "+JInterval.isInteger(i1));
	

	//Monomial:
	System.out.println("*****************************************************");
	System.out.println("                                      ");
	System.out.println("Test monomial...");
	
	Monomial m1 = new Monomial(v1);
	Monomial m2 = new Monomial(v2,3);
	Monomial m3 = new Monomial(v3,2);
	System.out.println(m1.toString()); 
	System.out.println(m2.toString()); 
   	System.out.println(m3.toString()); 
   	System.out.println("m1.nrVariables: "+m1.nrVariables());
	System.out.println("m3.totalDegree: "+m3.tdeg());   
  
	VarIntPairVector result = m2.exponents();
	System.out.println("m2.exponents: "+result.get(0).getFirst().toString()+", "+result.get(0).getSecond());

	//JTerm:
	System.out.println("*****************************************************");
	System.out.println("                                      ");
	System.out.println("Test JTerm...");
	
	JTerm t = new JTerm(new Rational(5.5),m2);
	System.out.println(t.toString());
	System.out.println("t.coeff: "+t.coeff().toString());
	System.out.println("t.monomial(): "+t.monomial().toString()); 
	


	//Polynomial:
	System.out.println("*****************************************************");
	System.out.println("                                      ");
	System.out.println("Test MultivariatePolynomial...");

	Polynomial p1 = new Polynomial(m1);
	Polynomial p2 = new Polynomial(m2);	
	Polynomial p3 = new Polynomial(v1);
	Polynomial p4 = new Polynomial(new Rational(-1));
	
	System.out.println(p1.toString());
	System.out.println(p2.toString());
	System.out.println(p3.toString());
	System.out.println(p4.toString());

	System.out.println("p1.constantPart(): "+p1.constantPart());
	System.out.println("p4.constantPart(): "+p4.constantPart());
	VarRationalMap map = new VarRationalMap();
	map.set(v2,new Rational(1));
	map.set(v1,new Rational(2));
	System.out.println("p1 evaluated at 2: "+p1.evaluate(map).toString());
	System.out.println("p1 totalDegree: "+p1.totalDegree());
	System.out.println("p1 degree z: "+p1.degree(v1));
	System.out.println("p1 degree x: "+p1.degree(v2));
	System.out.println("p1 nrTerms: "+p1.nrTerms());
	System.out.println("p1 size: "+p1.size());
	System.out.println("p4 size: "+p4.size());

	//RationalFunction:
	System.out.println("*****************************************************");
	System.out.println("                                      ");
	System.out.println("Test RationalFunction...");

	RationalFunction f1 = new RationalFunction(p1,p4);
	RationalFunction f2 = new RationalFunction(p4,p1);
	RationalFunction f3 = new RationalFunction(p2,p3);

	System.out.println(f1.toString());
	System.out.println(f2.toString());
	System.out.println(f3.toString());
	map.set(v3,new Rational(-1));
	System.out.println("Using substitution map x -> 1, z -> 2, fresh variable -> -1");
	System.out.println("evaluate f1: "+f1.evaluate(map).toString());
	System.out.println("evaluate f2: "+f2.evaluate(map).toString());
	System.out.println("evaluate f3: "+f3.evaluate(map).toString());

	System.out.println("nominator(numerator) f1: "+f1.nominator().toString());
	System.out.println("denominator f1: "+f1.denominator().toString());

	System.out.println("f1 == f1: "+f1.equals(f1));
	System.out.println("f1 == f2: "+f1.equals(f2));
	System.out.println("f3 != f3: "+f3.notEquals(f3));

	VarVector varlist = f1.gatherVariables();
	System.out.println("f1 gatherVariables: "+varlist.get(0).toString());



	//FactorizedPolynomial:
	System.out.println("*****************************************************");
	System.out.println("                                      ");
	System.out.println("Test FactorizedPolynomial...");
	
	FactorizationCache cache = new FactorizationCache();
	JFactorizedPolynomial fp1 = new JFactorizedPolynomial(new Rational(2));
	JFactorizedPolynomial fp2 = new JFactorizedPolynomial(p1,cache);
	JFactorizedPolynomial fp3 = new JFactorizedPolynomial(p2,cache);
		
	System.out.println("fp1: "+fp1.toString());
	System.out.println("fp2: "+fp2.toString());
	System.out.println("fp3: "+fp3.toString());
	System.out.println("fp1.constantPart: "+fp1.constantPart().toString());
	System.out.println("evaluate fp3: "+fp3.evaluate(map).toString()); 
	varlist = fp2.gatherVariables();
	System.out.println("fp2.gatherVariables: "+varlist.get(0).toString());
	System.out.println("fp1 == fp1: "+fp1.equals(fp1));
	System.out.println("fp1 == fp2: "+fp1.equals(fp2));
	System.out.println("fp2 != fp2: "+fp2.notEquals(fp2));
	System.out.println("fp1 != fp2: "+fp1.notEquals(fp2));


//TODO: test all operators!!






    // ----- Delete everything -----
    
    System.out.println( "\nGuess I'll clean up now" );
    
    // Note: this invokes the virtual destructor
    // You could leave this to the garbage collector

    System.out.println( "Goodbye" );
  }
}
